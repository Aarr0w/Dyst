/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DystAudioProcessorEditor::DystAudioProcessorEditor (DystAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    //id's must be non-zero, 0 indicates no selection
    //but id(1) = index(0)
    StyleCombo.addItem("Sigmoid", 1);
    StyleCombo.addItem("ArcTan", 2);
    StyleCombo.addItem("HyperTan", 3);
    StyleCombo.addItem("Arraya", 4);


    driveSlider.setRange(0.1f, 6.f, 0.01f);
    driveSlider.setValue(1.f);
    driveSlider.setSkewFactor(.7f);
    rangeSlider.setRange(0.1f, 6.f, 0.01f);
    rangeSlider.setValue(1.f);
    driveSlider.setSkewFactor(.5f);
    curveSlider.setRange(0.f, 1.f, .01f);
    curveSlider.setValue(0.5f);
    timeSlider.setSkewFactor(.2f);


    outputMeter.setTreeState(&audioProcessor.treeState);
    setSize (420, 420);
    LookAndFeel::setDefaultLookAndFeel(&DystLookAndFeel);
    addAndMakeVisible(outputMeter);
    startTimerHz(10);

    inputSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    outputSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    boostSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);


    driveSlider.setSliderStyle(Slider::SliderStyle::Rotary);
    rangeSlider.setSliderStyle(Slider::SliderStyle::Rotary);
    curveSlider.setSliderStyle(Slider::SliderStyle::Rotary);

    DystLookAndFeel.setColour(Slider::thumbColourId, Colours::orange.withSaturation(0.7f));

    inputSlider.setTextBoxStyle(Slider::TextBoxAbove, true, 40, 40);
    outputSlider.setTextBoxStyle(Slider::TextBoxAbove, true, 40, 40);
    boostSlider.setTextBoxStyle(Slider::TextBoxAbove, true, 40, 40);
    inputSlider.setColour(Slider::textBoxTextColourId, Colours::orange.withLightness(.7f));
    outputSlider.setColour(Slider::textBoxTextColourId, Colours::orange.withLightness(.7f));


    driveSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 40, 20);
    rangeSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 30, 20);
    curveSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 30, 20);
    boostSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 30, 20);

    DystLookAndFeel.setThumbColour(Colours::orange.withSaturation(0.7f));
    driveLabel.setColour(Label::textColourId, Colours::orange.withBrightness(1.1f).withSaturation(.9f));
    dynamicsButton.setColour(TextButton::textColourOffId, Colours::slategrey);

    timeSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    timeSlider.setLookAndFeel(&DystLookAndFeel);
    timeSlider.setTextValueSuffix("ms");

    choiceArctan.setRadioGroupId(1);
    choiceSigmoid.setRadioGroupId(1);
    choiceHypertan.setRadioGroupId(1);
    choiceArraya.setRadioGroupId(1);

    choiceSigmoid.setButtonText("A");
    choiceArctan.setButtonText("B");
    choiceHypertan.setButtonText("C");
    choiceArraya.setButtonText("D");
    dynamicsButton.setButtonText("DYNAMICS");
    responseToggle.setButtonText("INVERSE");
    aggressiveToggle.setButtonText("DISTORT");
    clipToggle.setButtonText("Clip");
    midsideToggle.setButtonText("M/S");

    driveLabel.setText("DRIVE", dontSendNotification);
    curveLabel.setText("curve", dontSendNotification);
    rangeLabel.setText("range", dontSendNotification);
    inputLabel.setText("input", dontSendNotification);
    outputLabel.setText("output", dontSendNotification);
    boostLabel.setText("boost", dontSendNotification);

    driveLabel.setJustificationType(Justification::centred);
    curveLabel.setJustificationType(Justification::centredBottom);
    rangeLabel.setJustificationType(Justification::centredBottom);
    inputLabel.setJustificationType(Justification::centredBottom);
    outputLabel.setJustificationType(Justification::centredBottom);
    boostLabel.setJustificationType(Justification::centredTop);
 
    responseToggle.setClickingTogglesState(true);
    aggressiveToggle.setClickingTogglesState(true);
    clipToggle.setClickingTogglesState(true);
    midsideToggle.setClickingTogglesState(true);
    dynamicsButton.setClickingTogglesState(true);

    auto dynamicsLambda = [this]() 
    {
          if(dynamicsButton.getToggleState())
          {
             curveSlider.setColour(Slider::thumbColourId, Colours::orange.withLightness(.7f)); 
             aggressiveToggle.setColour(ToggleButton::tickColourId, Colours::orange.withBrightness(1.f).withSaturation(.4f).withHue(.1f));
             aggressiveToggle.setColour(ToggleButton::textColourId, Colours::orange.withBrightness(1.1f).withSaturation(.8f));
             responseToggle.setColour(ToggleButton::tickColourId, Colours::orange.withBrightness(1.f).withSaturation(.4f).withHue(.1f));
             responseToggle.setColour(ToggleButton::textColourId, Colours::orange.withBrightness(1.1f).withSaturation(.8f));

             if (aggressiveToggle.getToggleState())
             {
                 rangeSlider.setColour(Slider::thumbColourId, Colours::orange.withLightness(.7f));
             }
             else
             {
                 rangeSlider.setColour(Slider::thumbColourId, Colours::slategrey);
             }

          }       
          else
          { 
             aggressiveToggle.setColour(ToggleButton::tickColourId, Colours::slategrey);
             aggressiveToggle.setColour(ToggleButton::textColourId, Colours::slategrey);
             responseToggle.setColour(ToggleButton::tickColourId, Colours::slategrey);
             responseToggle.setColour(ToggleButton::textColourId, Colours::slategrey);
             curveSlider.setColour(Slider::thumbColourId, Colours::slategrey);  
             rangeSlider.setColour(Slider::thumbColourId, Colours::slategrey);  
          }  

          /*if (choiceArraya.getToggleState())
          {
              rangeSlider.setColour(Slider::thumbColourId, Colours::slategrey);
              driveSlider.setColour(Slider::thumbColourId, Colours::slategrey);
              aggressiveToggle.setColour(ToggleButton::tickColourId, Colours::slategrey);
              aggressiveToggle.setColour(ToggleButton::textColourId, Colours::slategrey);
          }
          else
          {
              driveSlider.setColour(Slider::thumbColourId, Colours::orange.withLightness(.7f));
              
          }*/

    };
    
    dynamicsButton.onClick = dynamicsLambda;
    aggressiveToggle.onClick = dynamicsLambda;

    auto createLambdas = [this](int id, Button& b) {
        return [this, id,&b]() {
            updateToggleState(b, StyleCombo, id);

            /*if (choiceArraya.getToggleState()) {
                rangeSlider.setColour(Slider::thumbColourId, Colours::slategrey);
                driveSlider.setColour(Slider::thumbColourId, Colours::slategrey);
                aggressiveToggle.setColour(ToggleButton::tickColourId, Colours::slategrey);
                aggressiveToggle.setColour(ToggleButton::textColourId, Colours::slategrey);
            }*/
            //else 
            //{
            driveSlider.setColour(Slider::thumbColourId, Colours::orange.withLightness(.7f));

            if(dynamicsButton.getToggleState())
            {
                aggressiveToggle.setColour(ToggleButton::tickColourId, Colours::orange.withBrightness(1.f).withSaturation(.4f).withHue(.1f));
                aggressiveToggle.setColour(ToggleButton::textColourId, Colours::orange.withBrightness(1.1f).withSaturation(.8f));
            }
             
            if (aggressiveToggle.getToggleState() && dynamicsButton.getToggleState())
            {
                rangeSlider.setColour(Slider::thumbColourId, Colours::orange.withLightness(.7f));
            }
            //}
        };
    };

    // Create lambda functions with different ids
    auto choiceLambda0 = createLambdas(0,choiceSigmoid);
    auto choiceLambda1 = createLambdas(1,choiceArctan);
    auto choiceLambda2 = createLambdas(2,choiceHypertan);
    auto choiceLambda3 = createLambdas(3,choiceArraya);

    choiceSigmoid.onClick = choiceLambda0;
    choiceArctan.onClick = choiceLambda1;
    choiceHypertan.onClick = choiceLambda2;
    choiceArraya.onClick = choiceLambda3;


    drive_attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "drive", driveSlider);
    range_attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "range", rangeSlider);
    curve_attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "curve", curveSlider);
    input_attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "input_gain", inputSlider);
    output_attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "output_gain", outputSlider);
    boost_attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "boost_gain", boostSlider);
    time_attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "time", timeSlider);

    style_attachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.treeState, "style", StyleCombo);

    dynamics_attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.treeState, "dynamics", dynamicsButton);
    response_attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.treeState, "inverse", responseToggle);
    aggressive_attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.treeState, "aggressive", aggressiveToggle);
    clip_attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.treeState, "clip", clipToggle);
    midside_attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.treeState, "midside", midsideToggle);

    if (audioProcessor.savedState.isValid())
    {
        audioProcessor.treeState.replaceState(audioProcessor.savedState);
        //Logger::outputDebugString("state replaced_style combo value: " + to_string(StyleCombo.getSelectedItemIndex()));
        switch (StyleCombo.getSelectedItemIndex())
        {
            case 0: choiceSigmoid.setToggleState(true, false); break;
            case 1: choiceArctan.setToggleState(true, false); break;
            case 2: choiceHypertan.setToggleState(true, false); break;
            case 3: choiceArraya.setToggleState(true, false); break;
            default: choiceSigmoid.setToggleState(true, false); break;
        }
    }

    addAndMakeVisible(inputSlider);
    addAndMakeVisible(outputSlider);
    addAndMakeVisible(rangeSlider);
    addAndMakeVisible(driveSlider);
    addAndMakeVisible(curveSlider);
    addAndMakeVisible(timeSlider);
    addAndMakeVisible(boostSlider);

    addAndMakeVisible(dynamicsButton);
    addAndMakeVisible(responseToggle);
    addAndMakeVisible(aggressiveToggle);
    addAndMakeVisible(clipToggle);
    addAndMakeVisible(midsideToggle);

    addAndMakeVisible(choiceArctan);
    addAndMakeVisible(choiceHypertan);
    addAndMakeVisible(choiceArraya);
    addAndMakeVisible(choiceSigmoid);

    addAndMakeVisible(driveLabel);
    addAndMakeVisible(styleLabel);
    addAndMakeVisible(curveLabel);
    addAndMakeVisible(rangeLabel);
    addAndMakeVisible(inputLabel);
    addAndMakeVisible(outputLabel);
    addAndMakeVisible(boostLabel);
}

DystAudioProcessorEditor::~DystAudioProcessorEditor()
{
}

void DystAudioProcessorEditor::updateToggleState(juce::Button& button,ComboBox& cb, int index)
{
    //auto state = button.getToggleState();

    //juce::String stateString = state ? "ON" : "OFF";
    button.setToggleState(true, false);
    cb.setSelectedItemIndex(index);

}


void DystAudioProcessorEditor::timerCallback()
{
    //juce::Logger::outputDebugString("updating...");
    outputMeter.setLevel(audioProcessor.getRmsValue());
    outputMeter.setSaturation(audioProcessor.getSaturationLevel());
    outputMeter.repaint();
}
//==============================================================================
void DystAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colours::darkgrey);
    //g.setColour (juce::Colours::white);
    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);  

}

void DystAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    bounds = getLocalBounds();
    auto fullHeight = bounds.getHeight();
    auto fullWidth = bounds.getWidth();
    bounds.reduce(fullWidth / 20, 0);
    outputMeter.setBounds(bounds.removeFromTop(40).reduced(10,5));
    auto low_bounds = bounds.removeFromBottom(20).reduced(20, 0);
    clipToggle.setBounds(low_bounds.removeFromRight(50));
    midsideToggle.setBounds(low_bounds.removeFromLeft(50));
    boostSlider.setBounds(low_bounds);
    

    dynamicsButton.setPaintingIsUnclipped(true);
    inputSlider.setPaintingIsUnclipped(true);
    outputSlider.setPaintingIsUnclipped(true);
    timeSlider.setPaintingIsUnclipped(true);
    responseToggle.setPaintingIsUnclipped(true);
    aggressiveToggle.setPaintingIsUnclipped(true);
    clipToggle.setPaintingIsUnclipped(true);
    midsideToggle.setPaintingIsUnclipped(true);
    curveSlider.setPaintingIsUnclipped(true);
    boostSlider.setPaintingIsUnclipped(true);
    boostLabel.setPaintingIsUnclipped(true);

    timeLabel.setPaintingIsUnclipped(true);
    styleLabel.setPaintingIsUnclipped(true);
    driveLabel.setPaintingIsUnclipped(true);
    curveLabel.setPaintingIsUnclipped(true);
    rangeLabel.setPaintingIsUnclipped(true);
    inputLabel.setPaintingIsUnclipped(true);
    outputLabel.setPaintingIsUnclipped(true);
    boostLabel.setPaintingIsUnclipped(true);

    auto vertical_slider_bounds = bounds.removeFromLeft(fullWidth/14);

    vertical_slider_bounds.reduce(0, 40);
    inputSlider.setBounds(vertical_slider_bounds.expanded(10, 0));
    vertical_slider_bounds = bounds.removeFromRight(30);
    vertical_slider_bounds.reduce(0, 40);
    outputSlider.setBounds(vertical_slider_bounds.expanded(10,0));

    timeSlider.setBounds(bounds.removeFromTop(bounds.getHeight() / 15));

    auto distortion_section = bounds.removeFromTop(bounds.getHeight() / 3).translated(0,-fullHeight/15);
    distortion_section.reduce(10,distortion_section.getHeight()/3);
    choiceSigmoid.setBounds(distortion_section.removeFromLeft(distortion_section.getWidth() / 4).reduced(10,0));
    choiceArctan.setBounds(distortion_section.removeFromLeft(distortion_section.getWidth() / 3).reduced(10,0));
    choiceHypertan.setBounds(distortion_section.removeFromLeft(distortion_section.getWidth() / 2).reduced(10,0));
    choiceArraya.setBounds(distortion_section.reduced(10,0));
 
    driveSlider.setBounds(bounds.translated(0,-bounds.getHeight()/4).reduced(bounds.getWidth()/4,0));
    
    auto right_column = bounds.removeFromRight(bounds.getWidth() / 3);
    right_column.removeFromTop(bounds.getHeight()/2);
    rangeSlider.setBounds(right_column.translated(0, -bounds.getHeight() / 5).reduced(5));
    curveSlider.setBounds(right_column.translated(-bounds.getWidth(), -bounds.getHeight() / 5).reduced(5));
    dynamicsButton.setBounds(right_column.translated(-bounds.getWidth()/2,0).reduced(10,40));

    auto bottom_section = bounds;
    bottom_section = bottom_section.removeFromTop(bounds.getHeight() / 2);
    auto dynamics_row = bottom_section.removeFromTop(bottom_section.getHeight() / 3)
        .withRight(right_column.getRight())
        .translated(0, -20);

    responseToggle.setBounds(dynamics_row.removeFromLeft(dynamics_row.getWidth() * .7f).translated(dynamics_row.getWidth()*.05f, 0));
    aggressiveToggle.setBounds(dynamics_row.translated(10,0));
   
    driveLabel.setBounds(driveSlider.getBounds().removeFromBottom(driveSlider.getHeight()/3));
    curveLabel.setBounds(curveSlider.getBounds().removeFromBottom(curveSlider.getHeight()/5));
    rangeLabel.setBounds(rangeSlider.getBounds().removeFromBottom(rangeSlider.getHeight()/5));
    inputLabel.setBounds(inputSlider.getBounds().removeFromBottom(inputSlider.getHeight()/7).translated(0,fullHeight/15).expanded(10,0));
    outputLabel.setBounds(outputSlider.getBounds().removeFromBottom(outputSlider.getHeight()/7).translated(-10,fullHeight/15).expanded(10, 0));
    boostLabel.setBounds(boostSlider.getBounds().translated(0,-20));

    //driveSlider.setBounds(bottom_section.removeFromLeft(bottom_section.getWidth() / 3).reduced(10));
    //rangeSlider.setBounds(bottom_section.removeFromRight(bottom_section.getWidth() / 2).reduced(10));
   
    //outputMeter.setBounds(100, 20, 200, 15);

}
