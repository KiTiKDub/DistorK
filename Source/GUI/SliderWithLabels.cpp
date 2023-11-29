/*
  ==============================================================================

    SliderWithLabels.cpp
    Created: 28 Nov 2023 3:55:24pm
    Author:  kylew

  ==============================================================================
*/

#include "SliderWithLabels.h"

void SliderWithLabels::paint(juce::Graphics& g)
{
    using namespace juce;
    auto range = getRange();
    auto bounds = getLocalBounds();
    auto style = getSliderStyle();
    g.setColour(Colours::white);

    auto numChoices = labels.size();

    if (style == juce::Slider::LinearVertical)
    {
        auto sliderBounds = getVerticalSliderBounds();

        getLookAndFeel().drawLinearSlider(g,
            sliderBounds.getX(),
            sliderBounds.getY(),
            sliderBounds.getWidth(),
            sliderBounds.getHeight(),
            jmap(getValue(), range.getStart(), range.getEnd(), (double)sliderBounds.getBottom(), (double)sliderBounds.getTopLeft().getY()),
            sliderBounds.getBottom(),
            sliderBounds.getWidth(),
            style,
            *this);

        for (int i = 0; i < numChoices; ++i)
        {
            Rectangle<int> labelBox;

            auto pos = labels[i].pos;
            auto str = labels[i].label;
            auto textHeight = labels[i].fontSize;
            g.setFont(textHeight);
            auto strWidth = g.getCurrentFont().getStringWidth(str);

            if (pos == 1)
            {
                labelBox = bounds.removeFromTop(bounds.getHeight() * .05);
            }

            else if (pos == 2)
            {
                labelBox = bounds.removeFromBottom(bounds.getHeight() * .05);
            }

            g.drawFittedText(str, labelBox, juce::Justification::centred, 1);
        }
    }
    else if (style == juce::Slider::LinearHorizontal)
    {
        auto sliderBounds = getHorizontalSliderBounds();

        getLookAndFeel().drawLinearSlider(g,
            sliderBounds.getX(),
            sliderBounds.getY(),
            sliderBounds.getWidth(),
            sliderBounds.getHeight(),
            jmap(getValue(), range.getStart(), range.getEnd(), (double)sliderBounds.getTopLeft().getX(), (double)sliderBounds.getRight()),
            sliderBounds.getBottom(),
            sliderBounds.getWidth(),
            style,
            *this);


        for (int i = 0; i < numChoices; ++i)
        {
            Rectangle<int> labelBox;

            auto pos = labels[i].pos;
            auto str = labels[i].label;
            auto textHeight = labels[i].fontSize;
            g.setFont(textHeight);
            auto strWidth = g.getCurrentFont().getStringWidth(str);

            if (pos == 1)
            {
                labelBox = bounds.removeFromTop(bounds.getHeight() * .45);
            }

            else if (pos == 2)
            {
                labelBox = bounds.removeFromBottom(bounds.getHeight() * .75);
            }

            g.drawFittedText(str, labelBox, juce::Justification::centred, 1);
        }
    }

    
}

juce::Rectangle<int> SliderWithLabels::getHorizontalSliderBounds() const
{
    auto bounds = getLocalBounds();

    auto boundsShrink = bounds.reduced(bounds.getWidth() * .05, 0);

    return boundsShrink;
}

juce::Rectangle<int> SliderWithLabels::getVerticalSliderBounds() const
{
    auto bounds = getLocalBounds();
    
    auto boundsShrink = bounds.reduced(0, bounds.getHeight() * .1);

    return boundsShrink;
}

void SliderWithLabels::changeParam(juce::RangedAudioParameter* p)
{
    param = p;
    repaint();
}

juce::String getValString(const juce::RangedAudioParameter& param, bool isName, juce::String suffix)
{
    juce::String str;

    auto name = param.getName(20);
    auto val = param.getValue();
    auto range = param.getNormalisableRange();
    auto normVal = juce::jmap(val, 0.f, 1.f, range.start, range.end);
    normVal = floor(normVal * 100);
    normVal /= 100;

    if (isName)
    {
        str << name;
    }
    else
    {
       str << normVal << suffix;
    }

    return str;
}
