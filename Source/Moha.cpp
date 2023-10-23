/*
  ==============================================================================

    Moha.cpp
    Created: 23 Oct 2023 5:18:36pm
    Author:  linyudong

  ==============================================================================
*/

#include "Moha.h"

void Moha::pwm(double freq, float& _data)
{
    phase += freq / sampleRate;
    if (phase > sampleRate / 2) _data = 0;
    if (phase >= sampleRate) phase -= sampleRate;
}

void Moha::GetLevel(juce::dsp::AudioBlock<float>& in_audioBlock, double& _out)
{
    _out = 0;
    for (size_t i = 0; i < in_audioBlock.getNumChannels(); ++i) {
        auto* channelptr = in_audioBlock.getChannelPointer(i); 
        for (size_t j = 0; j < in_audioBlock.getNumSamples(); ++j) {
            _out += channelptr[j];
        }
    }
    _out /= (in_audioBlock.getNumChannels() * in_audioBlock.getNumSamples());
}

void Moha::prepare(juce::dsp::ProcessSpec& in_spec)
{
    hpf_pre.highPassFrequency = 20;

}
