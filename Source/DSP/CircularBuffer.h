/*
  ==============================================================================

    CircularBuffer.h
    Created: 7 Nov 2023 5:37:10pm
    Author:  TaroPie

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class CircularBuffer
{
public:
    CircularBuffer(int numChannels, int bufferSize);

    void fillBuffer(juce::AudioBuffer<float>& buffer);
    juce::AudioBuffer<float> readFromBuffer(int numSamples);

    int getnumChannels() { return numChannels; }
    int getBufferSize() { return bufferSize; }

private:
    juce::AudioBuffer<float> circularBuffer;
    int writePosition{ 0 };
    int numChannels;
    int bufferSize;
};