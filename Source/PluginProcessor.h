/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>
#include <vector>


using namespace std;
//==============================================================================
/**
*/
class DystAudioProcessor  : public juce::AudioProcessor, public ValueTree::Listener
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    DystAudioProcessor();
    ~DystAudioProcessor() override;

    juce::AudioProcessorValueTreeState treeState;
    juce::ValueTree savedState;

    //==============================================================================
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property) override;
   /* void adjustThreshhold();
    void adjustRMS_Size();
    void adjustInputGain();
    void adjustOutputGain();
    void adjustStyle();
    void adjustDrive();
    void adjustRange();*/


    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    float getRmsValue() const;
    float getSaturationLevel() const;

private:
    //==============================================================================
    std::atomic <bool> shouldUpate = false;
    dsp::ProcessorDuplicator<dsp::IIR::Filter<float>,dsp::IIR::Coefficients<float>> lowpassFilter;

    float rmsLeft;
    float rmsRight;
    float RMS = 0;
    float saturation_level = 0;
    float squareSample = 0;
    float squareSum = 0;
    float leastRecent = 0;
    float rms_low = 0;
    float low = 0;
    float threshhold;
    float side_scale_factor = 2.f;
     
    std::atomic<float>* th = treeState.getRawParameterValue("threshhold");
    std::atomic<float>* rms_time = treeState.getRawParameterValue("time");
    std::atomic<float>* drive = treeState.getRawParameterValue("drive");
    std::atomic<float>* intensity = treeState.getRawParameterValue("range");
    std::atomic<float>* dynamic = treeState.getRawParameterValue("dynamics");
    std::atomic<float>* clip = treeState.getRawParameterValue("clip");
    std::atomic<float>* midside = treeState.getRawParameterValue("midside");
    std::atomic<float>* aggressive = treeState.getRawParameterValue("aggressive");
    std::atomic<float>* curve = treeState.getRawParameterValue("curve");
    std::atomic<float>* inverse = treeState.getRawParameterValue("inverse");
    
    float in_gain = Decibels::decibelsToGain(static_cast<float>(*treeState.getRawParameterValue("input_gain")));
    float out_gain = Decibels::decibelsToGain(static_cast<float>(*treeState.getRawParameterValue("output_gain")));
    float boost_gain = Decibels::decibelsToGain(static_cast<float>(*treeState.getRawParameterValue("boost_gain")));
    float sat_spl0;
    float sat_spl1;
    float spl0;
    float spl1;
    float mid;
    float side;
    float ratio;
    float C;
    float K;


    float hardness = 0.7390851332151606; // x == cos(x)
    float softness = 1.0 - hardness;
    float refclip = 0.9549925859; // -0.2dB

    float inputSampleL = 0.f;
    float lastSampleL = 0.f;
    bool wasPosClipL = false;
    bool wasNegClipL = false;

    float inputSampleR = 0.f;
    float lastSampleR = 0.f;
    bool wasPosClipR = false;
    bool wasNegClipR = false;

    String style;

    int rms_counter;
    int low_counter;
    int srate;
    
    int rms_size;
    
    std::vector<float> rmsList;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DystAudioProcessor)
};
