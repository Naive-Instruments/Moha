/*
  ==============================================================================

    HighPassFilter.h
    Created: 23 Oct 2023 5:39:25pm
    Author:  TaroPie

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class HighPassFilter
{
public:
    HighPassFilter();
    void prepare(juce::dsp::ProcessSpec& spec);
    void process(juce::dsp::AudioBlock<float>& block);

    float highPassSampleRate{ 0 }, highPassFrequency{ 0 }, highPassQuality{ 0 };

private:
    using Filter = juce::dsp::IIR::Filter<float>;
    using MonoChain = juce::dsp::ProcessorChain<Filter>;

    MonoChain leftChain, rightChain;

    enum ChainPositions
    {
        HighPass
    };

    void updateHighPassFilter();
};