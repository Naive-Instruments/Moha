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
#include "DSP/HighPassFilter.h"
#include "DSP/LowPassFilter.h"


class Moha {
private:
    HighPassFilter hpf_pre;
    LowPassFilter lpf_pre, lpf_shift;
    
    size_t sampleRate = 48000;
    size_t channel = 2;

    // Middle parameters
    double phase = 0;               // Phase of PWM signal
    double rise_n_fall_time = 10;   // Rise and Fall time of pulse modulator in milliseconds
    double level_in_decibel = -120; // Audio input level
    double cutoffFrequency = 4000;  // Shifting lowpass filter frequency
    double Q = 1;                   // Shifting lowpass filter quality factor
    
    // Adjustble values
    double sensitivity = 0.5;   // Range [0,1]
    double speed = 0.5;         // Response speed, range [0,1]
    double darkness = 0.5;      // Set cutoff requency moving speed

public:
    Moha(){}
    ~Moha();

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

    void pwm(double freq, float& _data);

    void GetLevel(juce::dsp::AudioBlock<float>& in_audioBlock, double& _out);
    double GetSensitivity() { return sensitivity; }
    void SetSensitivity(double _sensitivity) { sensitivity = limit(_sensitivity, 0, 1); }
    double GetSpeed() { return speed; }
    void SetSpeed(double _speed) { speed = limit(_speed, 0, 1); }
    double GetDarkener() { return darkness; }
    void SetDarkener(double _darkness) { darkness = limit(_darkness, 0, 1); }

    void prepare(juce::dsp::ProcessSpec& in_spec);
    void process(juce::dsp::AudioBlock<double>& in_audioBlock);

};