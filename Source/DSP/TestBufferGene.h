/*
  ==============================================================================

    TestBufferGene.h
    Created: 8 Jan 2024 10:25:28am
    Author:  TaroPie

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class TestBufferGene
{
public:
    TestBufferGene(int numChannels, int bufferSize)
    {
		testBuffer.setSize(numChannels, bufferSize);
        testBuffer.clear();
    }

    void generate()
    {
        for (size_t channel = 0; channel < testBuffer.getNumChannels(); ++channel)
		{
			for (size_t i = 0; i < testBuffer.getNumSamples(); ++i)
			{
				testBuffer.setSample(channel, i, (i % 10) / 10.0f);
			}
		}
    }

    void replace(juce::AudioBuffer<float>& buffer)
	{
        for (size_t channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            if (readPos + buffer.getNumSamples() > testBuffer.getNumSamples())
            {
                buffer.copyFrom(channel, 0, testBuffer, channel, readPos, testBuffer.getNumSamples() - readPos);
                buffer.copyFrom(channel, testBuffer.getNumSamples() - readPos, testBuffer, channel, 0, buffer.getNumSamples() - (testBuffer.getNumSamples() - readPos));
            }
            else
            {
                buffer.copyFrom(0, 0, testBuffer, 0, readPos, testBuffer.getNumSamples());
            }
        }
        readPos += buffer.getNumSamples();
	}

    juce::AudioBuffer<float> getTestBuffer()
    {
        return testBuffer;
    }

private:
	juce::AudioBuffer<float> testBuffer;

    int readPos = 0;
};