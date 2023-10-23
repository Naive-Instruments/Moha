/*
  ==============================================================================

    LowPassFilter.h
    Created: 23 Oct 2023 5:39:36pm
    Author:  TaroPie

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class LowPassFilter
{
public:
    LowPassFilter();
    void prepare(juce::dsp::ProcessSpec& spec);
    void process(juce::dsp::AudioBlock<float>& block);

    float lowPassSampleRate{ 0 }, lowPassFrequency{ 0 }, lowPassQuality{ 0 };

private:
    using Filter = juce::dsp::IIR::Filter<float>;
    using MonoChain = juce::dsp::ProcessorChain<Filter>;

    MonoChain leftChain, rightChain;

    enum ChainPositions
    {
        LowPass
    };

    void updateLowPassFilter();
};