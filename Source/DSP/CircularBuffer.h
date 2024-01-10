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
    CircularBuffer(const int numChannels, const int bufferSize);

    void fillBuffer(juce::AudioBuffer<float>& buffer);
    juce::AudioBuffer<float> readFromBuffer(const int numSamples);

    void resetBufferFromCircularBuffer(juce::AudioBuffer<float>& buffer, const int loopNumSamples);

    int getnumChannels() { return CBNumChannels; }
    int getBufferSize() { return CBBufferSize; }

    bool isLooping{ false };

private:
    juce::AudioBuffer<float> circularBuffer;

    // ��ÿ�ο�ʼloop��ʱ��Ӧ�ö�Ҫ����һ�ݣ�������Ӱ��circularBuffer�ļ���fill
    juce::AudioBuffer<float> loopBuffer;

    int fillWritePosition{ 0 };
    int resetWritePosition{ 0 }, resetReadPosition{ 0 };
    int CBNumChannels{ 0 }, CBBufferSize{ 0 };

    void copyFromEveryChannel(juce::AudioBuffer<float>& dest, const int destStartSample, const juce::AudioBuffer<float>& source, const int sourceStartSample, const int numSamples);
};