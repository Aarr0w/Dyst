/*
  ==============================================================================

    AarrowLookAndFeel.h
    Created: 18 May 2023 12:27:25am
    Author:  Silver

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace Gui
{
	class AarrowLookAndFeel : public LookAndFeel_V4
	{
	public:
        AarrowLookAndFeel()
        {
            setColour(TextButton::textColourOnId, Colours::orange.withBrightness(0.3f).withHue(.9f).withSaturation(.5f));
            setColour(TextButton::textColourOnId, Colours::cyan);
            setColour(TextButton::textColourOffId, Colours::orange.withHue(.1f).withSaturation(0.3f));
      
            setColour(ToggleButton::textColourId, Colours::orange.withBrightness(1.1f).withSaturation(.8f));
            setColour(ToggleButton::tickDisabledColourId, findColour(Slider::backgroundColourId));
            setColour(ToggleButton::tickColourId, Colours::orange.withBrightness(1.f).withSaturation(.4f).withHue(.1f));

            setColour(Label::outlineColourId, Colours::transparentWhite);
            //setColour(Label::backgroundColourId, juce::Colours::transparentWhite);
            //setColour(Label::textColourId, Colours::orange.withLightness(.9f));
            setColour(Label::textColourId, Colours::orange.withBrightness(1.1f).withSaturation(.9f));

            setColour(Slider::textBoxOutlineColourId, Colours::transparentWhite);
            //setColour(Slider::textBoxBackgroundColourId, Colours::transparentWhite);     
            setColour(Slider::textBoxTextColourId, Colours::orange.withHue(.25f).withSaturation(0.5f));

            setColour(PropertyComponent::labelTextColourId, juce::Colours::pink);
            setColour(TextPropertyComponent::outlineColourId, juce::Colours::pink);

            /*textColourId 	
            The colour to use for the button's text.

            tickColourId 	
            The colour to use for the tick mark.

            tickDisabledColourId*/

           // setColour(TextButton::buttonOnColourId, Colours::pink);
            //setColour(juce::TextButton::buttonColourId, juce::Colours::hotpink);
        }
        void setThumbColour(Colour newThumbColour)
        {
            //setColour(ToggleButton::tickDisabledColourId, findColour(Slider::backgroundColourId));
            //setColour(ToggleButton::tickColourId, Colours::white);

            analogousColour = newThumbColour.withBrightness(0.3f).withHue(.9f).withSaturation(.5f);
            analogousColour2 = newThumbColour.withHue(.25f).withSaturation(0.5f);
            lightShade = newThumbColour.withSaturation(0.2f);
            keyColour = newThumbColour;
            setColour(ToggleButton::textColourId, newThumbColour.withBrightness(1.1f).withSaturation(.8f));
            setColour(TextButton::textColourOnId, newThumbColour.withBrightness(0.9f).withHue(.9f).withSaturation(.7f));
            setColour(Label::textColourId, newThumbColour.withBrightness(1.1f).withSaturation(.9f));
            
            /*
            analagousColour = ;
            complimentColour = ;
            backgroundColour = ;
            outlineColour = ;*/
        }

        juce::Colour getColourAt(const juce::Colour& startColour, const juce::Colour& endColour, float position)
        {
            // Interpolate the RGB components of the start and end colours
            int red = static_cast<int>((1.0f - position) * startColour.getRed() + position * endColour.getRed());
            int green = static_cast<int>((1.0f - position) * startColour.getGreen() + position * endColour.getGreen());
            int blue = static_cast<int>((1.0f - position) * startColour.getBlue() + position * endColour.getBlue());

            // Create the resulting colour
            return juce::Colour(red, green, blue);
        }

        void drawLinearSlider(Graphics& g, int x, int y, int width, int height,
            float sliderPos,
            float minSliderPos,
            float maxSliderPos,
            const Slider::SliderStyle style, Slider& slider) override
        {


            if (slider.isBar())
            {
                g.setColour(slider.findColour(Slider::thumbColourId).withHue(0.2f));
                g.fillRect(slider.isHorizontal() ? 
                      Rectangle<float>(sliderPos - (float)x, (float)y + 0.5f, 10.f, (float)height + 1.0f)
                    : Rectangle<float>((float)x + 0.5f, sliderPos, (float)width - 1.0f, (float)y + ((float)height - sliderPos)));
            }
            else
            {

                auto isTwoVal = (style == Slider::SliderStyle::TwoValueVertical || style == Slider::SliderStyle::TwoValueHorizontal);
                auto isThreeVal = (style == Slider::SliderStyle::ThreeValueVertical || style == Slider::SliderStyle::ThreeValueHorizontal);

                auto trackWidth = jmin(6.0f, slider.isHorizontal() ? (float)height * 0.25f : (float)width * 0.25f);

                Point<float> startPoint(slider.isHorizontal() ? (float)x : (float)x + (float)width * 0.5f,
                    slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));

                Point<float> endPoint(slider.isHorizontal() ? (float)(width + x) : startPoint.x,
                    slider.isHorizontal() ? startPoint.y : (float)y);

                Path backgroundTrack;
                backgroundTrack.startNewSubPath(startPoint);
                backgroundTrack.lineTo(endPoint);
                g.setColour(slider.findColour(Slider::backgroundColourId).withBrightness(0.2f));
                g.strokePath(backgroundTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

                Path valueTrack;
                Point<float> minPoint, maxPoint, thumbPoint;

                if (isTwoVal || isThreeVal)
                {
                    minPoint = { slider.isHorizontal() ? minSliderPos : (float)width * 0.5f,
                                 slider.isHorizontal() ? (float)height * 0.5f : minSliderPos };

                    if (isThreeVal)
                        thumbPoint = { slider.isHorizontal() ? sliderPos : (float)width * 0.5f,
                                       slider.isHorizontal() ? (float)height * 0.5f : sliderPos};

                    maxPoint = { slider.isHorizontal() ? maxSliderPos : (float)width * 0.5f,
                                 slider.isHorizontal() ? (float)height * 0.5f : maxSliderPos };
                }
                else
                {
                    auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
                    auto ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;

                    minPoint = startPoint;
                    maxPoint = { kx, ky };
                }

                auto thumbWidth = getSliderThumbRadius(slider);

                valueTrack.startNewSubPath(minPoint);
                valueTrack.lineTo(isThreeVal ? thumbPoint : maxPoint);
                g.setColour(slider.findColour(Slider::thumbColourId).withSaturation(0.2f));
                g.strokePath(valueTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

                if (!isTwoVal)
                {
                    g.setColour(slider.findColour(Slider::thumbColourId).withBrightness(0.3f).withHue(.9f).withSaturation(.5f));
                    //g.fillEllipse(Rectangle<float>(static_cast<float> (thumbWidth), static_cast<float> (thumbWidth*2)).withCentre(isThreeVal ? thumbPoint : maxPoint));
                    if (slider.isHorizontal())
                    {
                        g.fillRoundedRectangle(Rectangle<float>(static_cast<float> (thumbWidth*2.f), static_cast<float> (thumbWidth)).withCentre(isThreeVal ? thumbPoint : maxPoint), 5.f);
                    }
                    else
                    {
                        g.fillRoundedRectangle(Rectangle<float>(static_cast<float> (thumbWidth), static_cast<float> (thumbWidth * 2)).withCentre(isThreeVal ? thumbPoint : maxPoint), 5.f);
                    }
                    
                }

                if (isTwoVal || isThreeVal)
                {
                    auto sr = jmin(trackWidth, (slider.isHorizontal() ? (float)height : (float)width) * 0.4f);
                    auto pointerColour = slider.findColour(Slider::thumbColourId);

                    if (slider.isHorizontal())
                    {
                        drawPointer(g, minSliderPos - sr,
                            jmax(0.0f, (float)y + (float)height * 0.5f - trackWidth * 2.0f),
                            trackWidth * 2.0f, pointerColour, 2);

                        drawPointer(g, maxSliderPos - trackWidth,
                            jmin((float)(y + height) - trackWidth * 2.0f, (float)y + (float)height * 0.5f),
                            trackWidth * 2.0f, pointerColour, 4);
                    }
                    else
                    {
                        drawPointer(g, jmax(0.0f, (float)x + (float)width * 0.5f - trackWidth * 2.0f),
                            minSliderPos - trackWidth,
                            trackWidth * 2.0f, pointerColour, 1);

                        drawPointer(g, jmin((float)(x + width) - trackWidth * 2.0f, (float)x + (float)width * 0.5f), maxSliderPos - sr,
                            trackWidth * 2.0f, pointerColour, 3);
                    }
                }
            }
        }


        void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
            const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override
        {
            // Calculate the center of the knob
            float centerX = x + width * 0.5f;
            float centerY = y + height * 0.5f;

            //draw outer circle
            auto outline = slider.findColour(Slider::rotarySliderOutlineColourId);
            auto fill = slider.findColour(Slider::rotarySliderFillColourId);

            auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(10);

            auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
            auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
            auto lineW = jmin(8.0f, radius * 0.5f);
            auto arcRadius = radius - lineW * 0.5f;

            Path backgroundArc;
            backgroundArc.addCentredArc(bounds.getCentreX(),
                bounds.getCentreY(),
                arcRadius,
                arcRadius,
                0.0f,
                0.0f,
                2 * MathConstants<float>::pi,
                true);

            g.setColour(outline);
            //g.strokePath(backgroundArc, PathStrokeType(lineW*0.5f, PathStrokeType::curved, PathStrokeType::rounded));



            float valueRadius = (arcRadius * 0.5f + radius - lineW * 0.5f) * 0.5f;
            if (slider.isEnabled())
            {    
                Path valueArc;

            /*    for (float angle = rotaryStartAngle; angle < toAngle; angle += 0.02f)
                {*/
                    valueArc.addCentredArc(bounds.getCentreX(),
                        bounds.getCentreY(),
                        arcRadius,
                        arcRadius,
                        0.0f,
                        rotaryStartAngle,
                        toAngle,
                        true);

                    g.setColour(slider.findColour(Slider::thumbColourId).withSaturation(0.2f));
                    g.strokePath(valueArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));                    
            }

            Path innerArc;
            auto innerArc_radius = arcRadius * .7f;
            innerArc.addCentredArc(bounds.getCentreX(),
                bounds.getCentreY(),
                arcRadius *.7f,
                arcRadius *.7f,
                0.0f,
                0.f,
                2.f * MathConstants<float>::pi,
                true);

            g.setColour(outline);
            g.strokePath(innerArc, PathStrokeType(lineW*0.5, PathStrokeType::curved, PathStrokeType::rounded));

            auto thumbWidth = lineW;
            Point<float> thumbPoint(bounds.getCentreX() + (innerArc_radius - lineW*.5f) * std::cos(toAngle - MathConstants<float>::halfPi),
                bounds.getCentreY() + (innerArc_radius - lineW*.5f) * std::sin(toAngle - MathConstants<float>::halfPi));

            g.setColour(slider.findColour(Slider::thumbColourId));
            //g.fillEllipse(Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));

            Point<float> thumbPoint2(bounds.getCentreX() + (innerArc_radius - lineW) * std::cos(toAngle - MathConstants<float>::halfPi),
                bounds.getCentreY() + (innerArc_radius - lineW) * std::sin(toAngle - MathConstants<float>::halfPi));

          
            g.setColour(slider.findColour(Slider::thumbColourId).withSaturation(0.2f));
            g.fillEllipse(Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint2));
            
   

        }
        void drawTickBox(Graphics& g, Component& component,
            float x, float y, float w, float h,
            const bool ticked,
            const bool isEnabled,
            const bool shouldDrawButtonAsHighlighted,
            const bool shouldDrawButtonAsDown) override
        {
            ignoreUnused(isEnabled, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

            Rectangle<float> tickBounds(x, y, w, h);

            g.setColour(component.findColour(ToggleButton::tickDisabledColourId));
            g.fillRoundedRectangle(Rectangle<float>(tickBounds), 4.0f);

            if (ticked)
            {
                g.setColour(component.findColour(ToggleButton::tickColourId));
                g.fillRoundedRectangle(Rectangle<float>(tickBounds), 4.0f);
                //auto tick = getTickShape(0.75f);
                //g.fillPath(tick, tick.getTransformToScaleToFit(tickBounds.reduced(4, 5).toFloat(), false));
            }
        }

        void drawButtonBackground(Graphics& g,
            Button& button,
            const Colour& backgroundColour,
            bool shouldDrawButtonAsHighlighted,
            bool shouldDrawButtonAsDown) override
        {
            auto cornerSize = 6.0f;
            auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);

            auto baseColour = backgroundColour.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
                .withMultipliedAlpha(button.isEnabled() ? 2.0f : 0.5f);

            //clicking 'flash' effect
            if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
                baseColour = baseColour.contrasting(shouldDrawButtonAsDown ? 0.5f : 0.05f);

            g.setColour(baseColour);

            auto flatOnLeft = button.isConnectedOnLeft();
            auto flatOnRight = button.isConnectedOnRight();
            auto flatOnTop = button.isConnectedOnTop();
            auto flatOnBottom = button.isConnectedOnBottom();

            //draw outline
            if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom)
            {
                Path path;
                path.addRoundedRectangle(bounds.getX(), bounds.getY(),
                    bounds.getWidth(), bounds.getHeight(),
                    cornerSize, cornerSize,
                    !(flatOnLeft || flatOnTop),
                    !(flatOnRight || flatOnTop),
                    !(flatOnLeft || flatOnBottom),
                    !(flatOnRight || flatOnBottom));

                g.fillPath(path);

                g.setColour(button.findColour(ComboBox::outlineColourId));
                g.strokePath(path, PathStrokeType(1.0f));
            }
            else
            {
                g.fillRoundedRectangle(bounds, cornerSize);
                g.setColour(button.findColour(ComboBox::outlineColourId));
                //g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
            }
        }
        Colour contrastColour;
        Colour keyColour;
        Colour analogousColour;
        Colour analogousColour2;
        Colour lightShade;
        Colour backgroundColour;
        Colour outlineColour;
    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AarrowLookAndFeel);
	};

}
