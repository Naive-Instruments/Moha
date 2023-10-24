/*
  ==============================================================================

    Moha.cpp
    Created: 23 Oct 2023 5:18:36pm
    Author:  linyudong

  ==============================================================================
*/

#include "Moha.h"

void Moha::pwm(double freq, std::vector<float*>& _data)
{
    for (size_t i = 0; i < _data.size(); ++i) {
        if (phase > sampleRate / 2) *_data[i] = 0;
    }
    phase += freq / sampleRate;
    if (phase >= sampleRate) phase -= sampleRate;
}

void Moha::GetBufferAvgLevel(juce::dsp::AudioBlock<float>& in_audioBlock, double& _out)
{
    _out = 0;
    for (size_t i = 0; i < in_audioBlock.getNumChannels(); ++i) {
        auto* channelptr = in_audioBlock.getChannelPointer(i); 
        for (size_t j = 0; j < in_audioBlock.getNumSamples(); ++j) {
            _out += abs(channelptr[j]);
        }
    }
    _out /= (in_audioBlock.getNumChannels() * in_audioBlock.getNumSamples());
}

void Moha::GetHistoryAvgLevel(double& _out) {
    _out = 0;
    if (secondary_buffer == nullptr) return;
    if (secondary_buffer->size() == 0) return;
    for (size_t i = 0; i < secondary_buffer->size(); ++i) {
        _out += abs(secondary_buffer->data()[i]);
    }
    _out /= secondary_buffer->size();
}

void Moha::PushToHistory(float _in)
{
    secondary_buffer->push_back(_in);
}

void Moha::prepare(juce::dsp::ProcessSpec& in_spec)
{
    sampleRate = in_spec.sampleRate;
    channel = in_spec.numChannels;

    hpf_pre.prepare(in_spec);
    lpf_pre.prepare(in_spec);
    lpf_shift.prepare(in_spec);

    hpf_pre.highPassQuality = sqrt(2);
    lpf_pre.lowPassQuality = sqrt(2);
    lpf_shift.lowPassQuality = sqrt(2);

    hpf_pre.highPassFrequency = 20;
    lpf_pre.lowPassFrequency = 20000;
    lpf_shift.lowPassFrequency = 20000;

    secondary_buffer = new std::vector<float>(MAX_HISTORY_SAMPLES);
    std::fill(secondary_buffer->begin(), secondary_buffer->end(), 0);
}

void Moha::process(juce::dsp::AudioBlock<float>& in_audioBlock) {
    static std::vector<float*> frame;
    static double pwmFrequency = 0;

    // 1. Pre-filtering the material
    hpf_pre.process(in_audioBlock);
    lpf_pre.process(in_audioBlock);

    // 2. Volume-controlled auto-wah
    // Cast level to cutoff frequency
    GetHistoryAvgLevel(level_in_decibel);
    level_in_decibel = unity_to_dB(level_in_decibel);
    double splitFreq = MIN_CUTOFF_FREQ + darkness * (20000 - MIN_CUTOFF_FREQ);
    cutoffFrequency = splitFreq + pow_cast(
        level_in_decibel,
        MIN_PWM_TRIGGER_LEVEL_IN_DB * pow_cast(sensitivity),
        0,
        1 + darkness) 
        * (20000 - splitFreq);
    lpf_shift.lowPassFrequency = cutoffFrequency;
    lpf_shift.process(in_audioBlock);

    // 3. PWM modulating
    // Cast level to pwm frequency
    pwmFrequency = pow_cast(
        level_in_decibel,
        MIN_PWM_TRIGGER_LEVEL_IN_DB * pow_cast(sensitivity),
        0,
        (1 - speed) < 0.01 ? 0.01 : (1 - speed)) * MAX_PWM_FREQ;
    // Modulate
    for (size_t i = 0; i < in_audioBlock.getNumSamples(); ++i) {
        for (size_t j = 0; j < in_audioBlock.getNumSamples(); ++j) {
            auto var = &(in_audioBlock.getChannelPointer(j)[i]);
            frame.push_back(var);
            PushToHistory(*var);
        }
        pwm(pwmFrequency, frame);
        frame.clear();
    }
    
}