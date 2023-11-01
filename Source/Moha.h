/*
  ==============================================================================

    Moha.h
    Created: 23 Oct 2023 5:18:36pm
    Author:  linyudong

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include <math.h>
#include <limits>
#include "DSP/HighPassFilter.h"
#include "DSP/LowPassFilter.h"

#define MAX_HISTORY_SAMPLES (144)
#define MIN_CUTOFF_FREQ (200)
#define MAX_PWM_FREQ (120)
#define MIN_PWM_TRIGGER_LEVEL_IN_DB (-45)
#define MAX_GAIN_RANGE (24)


class Moha {
private:
    HighPassFilter hpf_pre;
    LowPassFilter lpf_pre, lpf_shift;
    
    size_t sampleRate = 48000;
    size_t channel = 2;

    std::vector<float>* secondary_buffer = nullptr;

    // Middle parameters
    double phase = 0;               // Phase of PWM signal
    double rise_n_fall_time = 10;   // Rise and Fall time of pulse modulator in milliseconds
    double level_in_decibel = -120; // Audio input level
    double cutoffFrequency = 20000;  // Shifting lowpass filter frequency
    double Q = sqrt(2);                   // Shifting lowpass filter quality factor
    
    // Adjustble values
    double gain = 0;            // Preamp gain in dB
    double sensitivity = 0.5;   // Range [0,1]
    double speed = 0.5;         // Response speed, range [0,1]
    double darkness = 0.5;      // Set cutoff requency moving speed
    double volume = 0;

public:
    Moha() {};
    ~Moha() {
        if (secondary_buffer != nullptr) 
            delete secondary_buffer;
    };

    // Tools
    double limit(double& _in, double _min, double _max) {
        if (_in > _max) return _max;
        else if (_in < _min) return _min;
        else return _in;
    }

    double unity_to_dB(double _in) {
        _in = (_in <= 0) ? 0.000001 : _in;
        return 20 * log10(_in);
    }

    double dB_to_unity(double _in) {
        return pow(10, _in / 20);
    }

    double pow_cast(double _in, double _min = 0, double _max = 1, double times = 0.5) {
        if (_min >= _max) return -1;
        if (_in < _min) return 0;
        else if (_in > _max) return 1;
        else {
            return pow((_in - _min) / (_max - _min), times);
        }
    }

    double lin_cast(double _in, double _min, double _max, double lower_boundary = 0, double upper_boundary = 1) {
        if ((_min >= _max) || (_in < _min) || (_in > _max) || (lower_boundary >= upper_boundary)) return -std::numeric_limits<double>::infinity();
        else {
            return  lower_boundary + (_in - _min) / (_max - _min) * (upper_boundary - lower_boundary);
        }
    }

    void pwm(double freq, juce::dsp::AudioBlock<float>& block, size_t& frameIndex);

    void GetBufferAvgLevel(juce::dsp::AudioBlock<float>& in_audioBlock, double& _out);
    void GetHistoryAvgLevel(double& _out);
    void PushToHistory(float _in);

    // Pre-stage
    void SetLPFPreFreq(double _in) { lpf_pre.lowPassFrequency = (_in < 20000 && _in > 20) ? _in : lpf_pre.lowPassFrequency; }
    void SetHPFPreFreq(double _in) { hpf_pre.highPassFrequency = (_in < 20000 && _in > 20) ? _in : hpf_pre.highPassFrequency; }

    // Core process
    double GetGain() { return gain; }
    void SetGain(double _gain) { gain = limit(_gain, -MAX_GAIN_RANGE, MAX_GAIN_RANGE); }
    double GetSensitivity() { return sensitivity; }
    void SetSensitivity(double _sensitivity) { sensitivity = limit(_sensitivity, 0.01, 100) / 100; }
    double GetSpeed() { return speed; }
    void SetSpeed(double _speed) { speed = limit(_speed, 0.01, 100) / 100; }
    double GetDarkness() { return darkness; }
    void SetDarkness(double _darkness) { darkness = limit(_darkness, 0.01, 100) / 100; }
    double GetVolume() { return volume; }
    void SetVolume(double _volume) { volume = limit(_volume, -144, 6); }

    void prepare(juce::dsp::ProcessSpec& in_spec);
    void process(juce::dsp::AudioBlock<float>& in_audioBlock);

};