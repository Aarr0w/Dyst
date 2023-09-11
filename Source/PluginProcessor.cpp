/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <algorithm>
#include "utils.h"

//==============================================================================
DystAudioProcessor::DystAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
                    treeState(*this, nullptr, "PARAMETER_TREE", createParameterLayout()),
                    lowpassFilter(dsp::IIR::Coefficients<float>::makeLowPass(44100,20500.f,0.4f))
#endif
{
    treeState.state.addListener(this);
}
juce::AudioProcessorValueTreeState::ParameterLayout DystAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout params;

    params.add(std::make_unique<juce::AudioParameterInt>("time", "Time", 50,500,60));

    params.add(std::make_unique<juce::AudioParameterFloat>("drive", "Drive", 0.1f, 6.f, 1.f));
    params.add(std::make_unique<juce::AudioParameterFloat>("range", "Range", 0.1f, 6.f, 1.f));
    params.add(std::make_unique<juce::AudioParameterFloat>("curve", "Curve", 0.f, 1.f, 0.01f));
    params.add(std::make_unique<juce::AudioParameterFloat>("input_gain", "Input", -20.f, 12.f, 0.f));
    params.add(std::make_unique<juce::AudioParameterFloat>("output_gain", "Output", -20.f, 12.f, 0.f));
    params.add(std::make_unique<juce::AudioParameterFloat>("boost_gain", "Boost", 0.f, 12.f, 0.f));
    params.add(std::make_unique<juce::AudioParameterFloat>("threshhold", "Threshhold", -20.f, 0.f, 0.1f));

    params.add(std::make_unique<juce::AudioParameterBool>("dynamics", "Dynamics", true));
    params.add(std::make_unique<juce::AudioParameterBool>("aggressive", "Aggressive", false));
    params.add(std::make_unique<juce::AudioParameterBool>("inverse", "Inverse", false)); //Direct or Inverse
    params.add(std::make_unique<juce::AudioParameterBool>("clip", "Clip", false)); 
    params.add(std::make_unique<juce::AudioParameterBool>("midside", "MidSide", false)); 

    //the strings are actually added manually in editor constructor
    params.add(std::make_unique<juce::AudioParameterChoice>("style", "Style", juce::Array<juce::String>{ "Sigmoid", "ArcTan", "HyperTan", "Arraya" }, 0));
    

    return params;
}
DystAudioProcessor::~DystAudioProcessor()
{
}

//==============================================================================
const juce::String DystAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DystAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DystAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DystAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DystAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DystAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DystAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DystAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DystAudioProcessor::getProgramName (int index)
{
    return {};
}

void DystAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DystAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    srate = getSampleRate();
    rms_counter = 0;
    rmsList.resize(srate * 1000);
    std::fill(rmsList.begin(), rmsList.end(), 0);

    threshhold = Decibels::decibelsToGain(static_cast<float>(*th));
    rms_size = floor((*rms_time / 1000) * srate);
    in_gain = Decibels::decibelsToGain(static_cast<float>(*treeState.getRawParameterValue("input_gain")));
    out_gain = Decibels::decibelsToGain(static_cast<float>(*treeState.getRawParameterValue("output_gain")));
    C = Lerp(*drive * .5f, (*drive + *intensity) - *drive / 2, *curve);
    style = treeState.getParameter("style")->getCurrentValueAsText();
    K= *drive;

    dsp::ProcessSpec spec;
    spec.sampleRate = srate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    lowpassFilter.prepare(spec);
    lowpassFilter.reset();
}

void DystAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    savedState = treeState.copyState();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DystAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DystAudioProcessor::valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property)
{
    shouldUpate = true;   
}

//void DystAudioProcessor::updateFilter()
//{
//   *lowpassFilter.state = *dsp::IIR::Coefficients<float>::makeLowPass(srate,{cutoff_frequency}, {Q});
//}
//
void DystAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    if (shouldUpate)
    {
        threshhold = Decibels::decibelsToGain(static_cast<float>(*th));
        style = treeState.getParameter("style")->getCurrentValueAsText();
        out_gain = Decibels::decibelsToGain(static_cast<float>(*treeState.getRawParameterValue("output_gain")));
        in_gain = Decibels::decibelsToGain(static_cast<float>(*treeState.getRawParameterValue("input_gain")));
        boost_gain = Decibels::decibelsToGain(static_cast<float>(*treeState.getRawParameterValue("boost_gain")));
        C = Lerp(*drive * .5f, (*drive + *intensity) - *drive / 2, *curve);
        rms_size = floor((*rms_time / 1000) * srate);
        K = *drive;
    }
  
    
    auto left_channelData = buffer.getWritePointer(0);
    auto right_channelData = buffer.getWritePointer(1);

    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
            
        spl0 = left_channelData[i];
        spl1 = right_channelData[i];

        if (*midside)
        {
            mid = (spl0 + spl1) / 2;
            side = (spl0 - spl1) / 2;
            //side *= side_scale_factor;
            spl0 = mid;
            spl1 = side;
        }

        // INPUT GAIN
        spl0 *= in_gain;
        spl1 *= in_gain;

        //  CALCULATE RMS  
        squareSample = jmax(abs(spl0), abs(spl1)) * jmax(abs(spl0),abs(spl1));
        squareSum += squareSample;
        rmsList[rms_counter % rms_size] = squareSample;

        if (rms_counter < rms_size)
        {
            leastRecent = rms_counter == 0 ?
                squareSum : rmsList[0];
        }
        else
        {
            leastRecent = rmsList[(rms_counter + 1) % rms_size];
            squareSum -= leastRecent;
        }
        RMS = sqrt(squareSum / jmin(rms_counter, rms_size)) * 1.4142135623730950488;
        RMS = sqrt(squareSum / jmin(rms_counter, rms_size));

        if(RMS < rms_low || low_counter >= floor((*rms_time / 500) * srate))
        {
            rms_low = RMS;
            low_counter = 0;
        }
        else
        {   low_counter += 1;}
        rms_counter += 1;


        
        // INTERPOLATION
        low = *inverse ?
            rms_low : .001;
        ratio = 1 - abs((threshhold - jmin(RMS, threshhold)) / (threshhold - low));

        if (*dynamic && *aggressive)
        {
            // Inverse 
            if (*inverse)
            {
                if (RMS < threshhold)
                {
                    saturation_level = 1 - jmin(1.f, ratio);
                    K = beziLerp(*drive, *drive + *intensity, C, 1 - jmin(1.f, ratio));
                }
                else
                {
                    saturation_level = 0;
                    K = *drive;
                }
            }
            // Direct
            else
            {

                if (RMS < threshhold)
                {
                    saturation_level = jmin(1.f, ratio);
                    K = beziLerp(*drive, *drive + *intensity, C, jmin(1.f, ratio));
                }
                else
                {
                    saturation_level = 1;
                    K = *drive + *intensity;
                }
            }
        }
        else
        {
            K = *drive;
        }

        // SATURATION STAGE
        if (style == "Sigmoid")
        {
            sat_spl0 = (2 / (1 + exp(-K * spl0))) - 1;
            sat_spl1 = (2 / (1 + exp(-K * spl1))) - 1;
        }
        else if (style == "ArcTan")
        {
            sat_spl0 = (2.12 / 3.14159265) * atan(K * spl0);
            sat_spl1 = (2.12 / 3.14159265) * atan(K * spl1);
        }
        else if (style == "HyperTan")
        {
            sat_spl0 = (exp(2 * K * spl0) - 1) / (exp(2 * K * spl0) + 1);
            sat_spl1 = (exp(2 * K * spl1) - 1) / (exp(2 * K * spl1) + 1);
        }
        else if (style == "Arraya")
        {
            sat_spl0 = (3 * spl0 / 2) * (1 - (square(spl0) / 2));
            sat_spl1 = (3 * spl1 / 2) * (1 - (square(spl1) / 2));

            //*************Double Saturate?***********
            sat_spl0 = (2 / (1 + exp(-K * sat_spl0))) - 1;
            sat_spl1 = (2 / (1 + exp(-K * sat_spl1))) - 1;
            //*****************************************
        }

        // CHOOSE INTERPOLATION METHOD
        if ((*dynamic && *aggressive) || (!*dynamic))
        {
            spl0 = sat_spl0;
            spl1 = sat_spl1;
        }
        else
        {
            if (*inverse)
            {
                saturation_level = 1 - jmin(1.f, ratio);
                spl0 = Lerp(spl0, sat_spl0, 1 - jmin(1.f, ratio));
                spl1 = Lerp(spl1, sat_spl1, 1 - jmin(1.f, ratio));
            }
            else
            {
                saturation_level = jmin(1.f, ratio);
                spl0 = Lerp(spl0, sat_spl0, jmin(1.f, ratio));
                spl1 = Lerp(spl1, sat_spl1, jmin(1.f, ratio));
            }


        }

        // Pre-Clip BOOST GAIN
        spl0 *= boost_gain;
        spl1 *= boost_gain;

        // reset stereo image
        if (*midside)
        {
           
            mid = (spl0 + spl1);
            side = (spl0 - spl1);
            //side /= side_scale_factor;
            spl0 = mid;
            spl1 = side;
        }


        inputSampleL = spl0;
        inputSampleR = spl1;
        // Clipper - Courtesy of AirWindows:OnlyClip
        if(*clip)
        {

            if (abs(spl0) > 4.0) spl0 = 4.0 * sign(spl0);
            if (abs(spl1) > 4.0) spl1 = 4.0 * sign(spl1);
     

            if (wasPosClipL == true) { //current will be over
                if (inputSampleL < lastSampleL) { //next one will NOT be over
                    lastSampleL = ((refclip * hardness) + (inputSampleL * softness));
                }
                else { //still clipping, still chasing the target
                    lastSampleL = ((lastSampleL * hardness) + (refclip * softness));
                }
            }
            wasPosClipL = false;
            if (inputSampleL > refclip) { //next will be over the true clip level. otherwise we directly use it
                wasPosClipL = true; //set the clip flag
                inputSampleL = ((refclip * hardness) + (lastSampleL * softness));
            }

            if (wasNegClipL == true) { //current will be -over
                if (inputSampleL > lastSampleL) { //next one will NOT be -over
                    lastSampleL = ((-refclip * hardness) + (inputSampleL * softness));
                }
                else { //still clipping, still chasing the target
                    lastSampleL = ((lastSampleL * hardness) + (-refclip * softness));
                }
            }
            wasNegClipL = false;
            if (inputSampleL < -refclip) { //next will be -refclip.  otherwise we directly use it
                wasNegClipL = true; //set the clip flag
                inputSampleL = ((-refclip * hardness) + (lastSampleL * softness));
            }

            if (wasPosClipR == true) { //current will be over
                if (inputSampleR < lastSampleR) { //next one will NOT be over
                    lastSampleR = ((refclip * hardness) + (inputSampleR * softness));
                }
                else { //still clipping, still chasing the target
                    lastSampleR = ((lastSampleR * hardness) + (refclip * softness));
                }
            }
            wasPosClipR = false;
            if (inputSampleR > refclip) { //next will be over the true clip level. otherwise we directly use it
                wasPosClipR = true; //set the clip flag
                inputSampleR = ((refclip * hardness) + (lastSampleR * softness));
            }

            if (wasNegClipR == true) { //current will be -over
                if (inputSampleR > lastSampleR) { //next one will NOT be -over
                    lastSampleR = ((-refclip * hardness) + (inputSampleR * softness));
                }
                else { //still clipping, still chasing the target
                    lastSampleR = ((lastSampleR * hardness) + (-refclip * softness));
                }
            }
            wasNegClipR = false;
            if (inputSampleR < -refclip) { //next will be -refclip.  otherwise we directly use it
                wasNegClipR = true; //set the clip flag
                inputSampleR = ((-refclip * hardness) + (lastSampleR * softness));
            }

            spl0 = lastSampleL;
            spl1 = lastSampleR;
            lastSampleL = inputSampleL;
            lastSampleR = inputSampleR;

        }


        //  OUTPUT GAIN
        spl0 *= out_gain;
        spl1 *= out_gain;

       

        // OUTPUT TO CHANNELS
        left_channelData[i] = spl0;
        right_channelData[i] = spl1;                            
    }
    dsp::AudioBlock<float> block(buffer);
    lowpassFilter.process(dsp::ProcessContextReplacing<float>(block));
   
}

//==============================================================================
bool DystAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DystAudioProcessor::createEditor()
{
    return new DystAudioProcessorEditor (*this);
}

//==============================================================================
void DystAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = treeState.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);

    //MemoryOutputStream stream(destData, false);
    //treeState.state.writeToStream(stream);
}

void DystAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(treeState.state.getType()))
            treeState.replaceState(juce::ValueTree::fromXml(*xmlState));
}

float DystAudioProcessor::getRmsValue() const
{
    return Decibels::gainToDecibels(RMS);
}
float DystAudioProcessor::getSaturationLevel() const
{
    return saturation_level;
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DystAudioProcessor();
}
