
#pragma once
#include <JuceHeader.h>
#include "AarrowLookAndFeel.h"

namespace Gui
{
	class HorizontalMeter : public Component, private Slider::Listener
	{
	public:
		HorizontalMeter()
		{
			
			meterBar.setLookAndFeel(&meterLookAndFeel);
			meterBar.setSliderStyle(juce::Slider::LinearBar);
			meterBar.setRange(-60.0f, 0.0f, 0.1f);
			meterBar.setValue(-40.0f);
			//meterBar.setTextBoxStyle(juce::Slider::NoTextBox, true, 20, 10);
			meterBar.setTextBoxStyle(juce::Slider::TextBoxRight, true, 20, 10);
		
			meterBar.setColour(juce::Slider::thumbColourId, juce::Colours::whitesmoke);
		
		}
		void paint(Graphics& g) override
		{
			auto bounds = getLocalBounds().toFloat();


			//the meter background
			g.setColour(Colours::grey.withBrightness(0.5f));
			g.fillRoundedRectangle(bounds, 1.f);

			//the outline
			Path outline;
			outline.addRoundedRectangle(bounds, 8.f);
			g.setColour(meterBar.findColour(Slider::rotarySliderOutlineColourId));
			g.strokePath(outline, PathStrokeType(12, PathStrokeType::curved, PathStrokeType::rounded));


			//the RMS value
			if (level > -30.f)
			{
				meterLookAndFeel.setThumbColour(Colours::orange);
				const auto scaledLevel =
					jmap(level, -30.f, 0.f, 0.f, 1.f);
				const auto scaledX =
					jmap(level, -30.f, 6.f, 0.f, static_cast<float>(getWidth()));

			    gradientStartColour = Colours::white.withBrightness(saturation);
				gradientEndColour = meterLookAndFeel.analogousColour.withSaturation(saturation);
				juce::ColourGradient gradient(gradientStartColour, bounds.getX(), bounds.getY(), gradientEndColour, bounds.getX() + scaledX, bounds.getY(), false);
				//gradient.addColour(0.5, meterLookAndFeel.analogousColour2);
				g.setGradientFill(gradient);
				g.fillRoundedRectangle(bounds.removeFromLeft(scaledX).reduced(0, 8).translated(8, 0), 5.f);
			}

		}

		void setLevel(const float value)
		{
			level = value;
		}
		void setSaturation(const float value)
		{
			saturation = value;
		}

		void setThreshold(float threshold)
		{
			meterBar.setValue(threshold);
		}

		void resized() override
		{
			meterBar.setBounds(getLocalBounds());
		}

		Gui::HorizontalMeter::~HorizontalMeter()
		{
			setLookAndFeel(nullptr);
		}

		void setTreeState(AudioProcessorValueTreeState* vts)
		{
			TreeState = vts;
			threshholdParam = TreeState->getParameter("threshhold");
			attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*TreeState, "threshhold", meterBar);
			//meterBar.addListener(this);
			meterBar.setRange(-20.f, 0.f);
			auto barBounds = getLocalBounds();
			meterBar.setBounds(barBounds.removeFromRight(barBounds.getWidth()*.67f));
			meterBar.setColour(juce::Slider::trackColourId, juce::Colours::whitesmoke);
			//meterBar.onValueChange = [this] { sliderValueChanged(&meterBar); };
			addAndMakeVisible(meterBar);
		}
		
	private:
		//juce::LookAndFeel_V4 meterLookAndFeel;
	    AarrowLookAndFeel meterLookAndFeel;
		ColourGradient gradient;
		Colour gradientStartColour;
		Colour gradientEndColour;
		juce::Slider meterBar;

		AudioProcessorValueTreeState* TreeState;
		AudioProcessorParameter* threshholdParam;
		std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attach;

		float level = -30.f; //-60.f
		float saturation = 0.8f;
		bool  init = true;

		void sliderValueChanged(Slider* slider) override
		{
			if (slider == &meterBar)
			{
				// Calculate the new threshold value based on the horizontal position of the meterBar
				float newThreshold = jmap<float>(slider->getX(), getX(), getX() + getWidth(), slider->getMinimum(), slider->getMaximum());				
				threshholdParam->setValue(newThreshold);

			}
		}

	};
	
}