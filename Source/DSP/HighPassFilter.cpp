/*
  ==============================================================================

    HighPassFilter.cpp
    Created: 23 Oct 2023 5:39:25pm
    Author:  TaroPie

  ==============================================================================
*/

#include "HighPassFilter.h"

HighPassFilter::HighPassFilter() {}

void HighPassFilter::updateHighPassFilter()
{
    auto highPassCoefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(highPassSampleRate, highPassFrequency, highPassQuality);

    *leftChain.get<ChainPositions::HighPass>().coefficients = *highPassCoefficients;
    *rightChain.get<ChainPositions::HighPass>().coefficients = *highPassCoefficients;
}


void HighPassFilter::prepare(juce::dsp::ProcessSpec& spec)
{
    juce::dsp::ProcessSpec monoChainSpec;
    monoChainSpec.maximumBlockSize = spec.maximumBlockSize;
    monoChainSpec.numChannels = 1;
    monoChainSpec.sampleRate = spec.sampleRate;

    leftChain.prepare(monoChainSpec);
    rightChain.prepare(monoChainSpec);

    highPassSampleRate = spec.sampleRate;
}

void HighPassFilter::process(juce::dsp::AudioBlock<float>& block)
{
    updateHighPassFilter();

    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);
}
