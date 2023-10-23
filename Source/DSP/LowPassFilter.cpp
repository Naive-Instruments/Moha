/*
  ==============================================================================

    LowPassFilter.cpp
    Created: 23 Oct 2023 5:39:36pm
    Author:  TaroPie

  ==============================================================================
*/

#include "LowPassFilter.h"

LowPassFilter::LowPassFilter() {}

void LowPassFilter::updateLowPassFilter()
{
    auto lowPassCoefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(lowPassSampleRate, lowPassFrequency, lowPassQuality);

    *leftChain.get<ChainPositions::LowPass>().coefficients = *lowPassCoefficients;
    *rightChain.get<ChainPositions::LowPass>().coefficients = *lowPassCoefficients;
}


void LowPassFilter::prepare(juce::dsp::ProcessSpec& spec)
{
    juce::dsp::ProcessSpec monoChainSpec;
    monoChainSpec.maximumBlockSize = spec.maximumBlockSize;
    monoChainSpec.numChannels = 1;
    monoChainSpec.sampleRate = spec.sampleRate;

    leftChain.prepare(monoChainSpec);
    rightChain.prepare(monoChainSpec);

    lowPassSampleRate = spec.sampleRate;
}

void LowPassFilter::process(juce::dsp::AudioBlock<float>& block)
{
    updateLowPassFilter();

    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);
}