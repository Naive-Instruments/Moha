/*
  ==============================================================================

    Moha.cpp
    Created: 23 Oct 2023 5:18:36pm
    Author:  linyudong

  ==============================================================================
*/

#include "Moha.h"

void Moha::pwm(double freq, juce::dsp::AudioBlock<float>& block, size_t& frameIndex)
{
    static double t_trans = 0;
    static double gain = 0;
    static bool is_rising = true;
    static double k = 0;
    static double t_tick = 0.01;
    static double period = 0.01;

    t_tick = 1.0 / sampleRate;
    k = log(10) / (rise_n_fall_time / 1000 * lin_cast(speed, 0, 1, 0.5, 1) * t_tick);
    period = freq / sampleRate;

    if (phase > period / 2) {
        if (is_rising) {
            t_trans = 0;
            is_rising = false;
        }
        else {
            t_trans += t_tick;
        }
        gain -= k * exp(-k * t_trans);
    }
    else {
        if (!is_rising) {
            t_trans = 0;
            is_rising = true;
        }
        else {
            t_trans += t_tick;
        }
        gain += k * exp(-k * t_trans);
    }

    gain = limit(gain, 0, 1) * dB_to_unity(volume);

    for (size_t i = 0; i < block.getNumChannels(); ++i) {
        auto var = block.getSample(i, frameIndex);
        var *= gain;
        block.setSample(i, frameIndex, var);
    }
    
    phase += t_tick;
    if (phase >= period) phase -= period;
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
    for (size_t i = secondary_buffer->size() - 1; i > 0; --i) {
        secondary_buffer->data()[i] = secondary_buffer->data()[i - 1];
    }
    secondary_buffer->data()[0] = _in;
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
    static double pwmFrequency = 0;
    static size_t subblock_count = 0;
    static std::vector<juce::dsp::AudioBlock<float>> subblock;
    
    subblock.clear();
    if (in_audioBlock.getNumSamples() > 128) {
        
        for (size_t i = 0; i < in_audioBlock.getNumSamples() / 128; ++i) {
            size_t sample_count = (in_audioBlock.getNumSamples() - i * 128 > 128) ? 128 : (in_audioBlock.getNumSamples() - i * 128);
            subblock.push_back(in_audioBlock.getSubBlock(i * 128, sample_count));
        }
    }
    else {
        subblock.push_back(in_audioBlock);
    }

    for (size_t block_index = 0; block_index < subblock.size(); ++block_index) {
        // 1. Pre-filtering the material (passed)
        hpf_pre.process(subblock[block_index]);
        lpf_pre.process(subblock[block_index]);

        GetHistoryAvgLevel(level_in_decibel);
        level_in_decibel = unity_to_dB(level_in_decibel);
        

        // 2. Pre-amp, PWM modulating and Post-amp
        // Cast level to pwm frequency
        pwmFrequency = pow_cast(
            level_in_decibel,
            MIN_PWM_TRIGGER_LEVEL_IN_DB * pow_cast(sensitivity),
            0,
            (1 - speed) < 0.01 ? 0.01 : (1 - speed)) * MAX_PWM_FREQ;

        for (size_t i = 0; i < subblock[block_index].getNumSamples(); ++i) {
            for (size_t j = 0; j < subblock[block_index].getNumChannels(); ++j) {
                // Preamp
                auto var = subblock[block_index].getSample(j, i) * dB_to_unity(gain);
                subblock[block_index].setSample(j, i, var);
                PushToHistory(var);
            }

            // Modulate
            pwm(pwmFrequency, subblock[block_index], i);
        }

        // 3. Volume-controlled auto-wah
        // Cast level to cutoff frequency
        double splitFreq = MIN_CUTOFF_FREQ + (1 - darkness) * (20000 - MIN_CUTOFF_FREQ);
        cutoffFrequency = splitFreq + pow_cast(
            level_in_decibel,
            MIN_PWM_TRIGGER_LEVEL_IN_DB * pow_cast(sensitivity),
            0,
            1 + darkness)
            * (20000 - splitFreq);
        lpf_shift.lowPassFrequency = cutoffFrequency;
        lpf_shift.process(subblock[block_index]);
    }
    
}