/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Component/OutputMeter.h"
//#include "Component/AarrowLookAndFeel.h"

//==============================================================================
/**
*/
class DystAudioProcessorEditor  : public juce::AudioProcessorEditor, public Timer
{
public:
    DystAudioProcessorEditor (DystAudioProcessor&);
    ~DystAudioProcessorEditor() override;

    //==============================================================================
    void timerCallback() override;
    void updateToggleState(Button& button, ComboBox& cb, int id);
    void toggleDynamics();
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DystAudioProcessor& audioProcessor;
    juce::Rectangle<int> bounds;
    Gui::HorizontalMeter outputMeter;
    Gui::AarrowLookAndFeel DystLookAndFeel;
    bool init = true;

    juce::Slider driveSlider;
    juce::Slider rangeSlider;
    juce::Slider curveSlider;
    juce::Slider inputSlider;
    juce::Slider outputSlider;
    juce::Slider boostSlider;
    juce::Slider timeSlider;

    juce::ToggleButton aggressiveToggle;
    juce::ToggleButton responseToggle;
    juce::ToggleButton clipToggle;
    juce::ToggleButton midsideToggle;

    juce::ComboBox StyleCombo;

    juce::TextButton choiceSigmoid;
    juce::TextButton choiceArctan;
    juce::TextButton choiceHypertan;
    juce::TextButton choiceArraya;
    juce::TextButton dynamicsButton;

    juce::Label timeLabel;
    juce::Label styleLabel;
    juce::Label driveLabel;
    juce::Label curveLabel;
    juce::Label rangeLabel;
    juce::Label inputLabel;
    juce::Label outputLabel;
    juce::Label boostLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> drive_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> range_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> curve_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> input_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> output_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> boost_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> time_attachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> dynamics_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> response_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> aggressive_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> clip_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> midside_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> style_attachment;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DystAudioProcessorEditor)
};
