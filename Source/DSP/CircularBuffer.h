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

    void resetBufferFromCircularBuffer(juce::AudioBuffer<float>& buffer, int loopNumSamples);

    int getnumChannels() { return numChannels; }
    int getBufferSize() { return bufferSize; }

    void continueFillBuffer() { isFillBuffer = true; }
    void pauseFillBuffer() { isFillBuffer = false; }

private:
    juce::AudioBuffer<float> circularBuffer;
    int fillWritePosition{ 0 };
    int resetWritePosition{ 0 }, resetReadPosition{ 0 };
    int numChannels;
    int bufferSize;

    bool isFillBuffer = true;
};