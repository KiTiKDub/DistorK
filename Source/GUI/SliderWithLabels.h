/*
  ==============================================================================

    SliderWithLabels.h
    Created: 28 Nov 2023 3:55:24pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct SliderWithLabels : juce::Slider {
   SliderWithLabels(juce::RangedAudioParameter* rap, const juce::String title, const juce::Slider::SliderStyle style) :
        juce::Slider(),
        param(rap)
    {
        setName(title);
        setSliderStyle(style);
        setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 50);
    }

    struct LabelPos {
        int pos = 0;
        juce::String label;
        int fontSize = 0;
    };

    juce::Array<LabelPos> labels;

    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getVerticalSliderBounds() const;
    juce::Rectangle<int> getHorizontalSliderBounds() const;

    void changeParam(juce::RangedAudioParameter* p);

protected:
    juce::RangedAudioParameter* param;

};

template <
    typename Attachment,
    typename APVTS,
    typename ParamName,
    typename SliderType
>
void makeSliderAttachment(std::unique_ptr<Attachment>& attachment,
    APVTS& apvts,
    const ParamName& name,
    SliderType& slider)
{
    attachment = std::make_unique<Attachment>(apvts,
        name,
        slider);
}

template <
    typename APVTS,
    typename Name
>
juce::RangedAudioParameter& getSliderParam(APVTS& apvts, const Name& name)
{
    auto param = apvts.getParameter(name);

    return *param;
}

template <
    typename Labels,
    typename ParamType,
    typename SuffixType
>
void addLabelPairs(Labels& labels, const ParamType& param, const SuffixType& suffix, const int fontSize = 14)
{
    labels.clear();
    labels.add({ 1, getValString(param, true, suffix), fontSize });
    labels.add({ 2, getValString(param, false, suffix), fontSize });
}

juce::String getValString(const juce::RangedAudioParameter& param, bool getLow, juce::String suffix);