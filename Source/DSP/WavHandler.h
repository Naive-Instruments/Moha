/*
  ==============================================================================

    WavReader.h
    Created: 19 Oct 2023 6:19:34pm
    Author:  TaroPie

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class WavHandler
{
public:
    WavHandler()
    {
		formatManager.registerBasicFormats();
	}

    void readWav(const char* wavName, const int wavSize)
    {
        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(std::make_unique<juce::MemoryInputStream>(wavName, wavSize, false)));
        if (reader != nullptr)
        {
            sampleRate = reader->sampleRate;
            audioBuffer.setSize(static_cast<int>(reader->numChannels), static_cast<int>(reader->lengthInSamples), false, false, false);
            reader->read(&audioBuffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
        }
    }

    void writeWav(juce::AudioBuffer<float> buffer, juce::String filePath, int wavSampleRate = 48000, int wavBitDepth = 32)
    {
        juce::File audioFile(filePath);
        if (audioFile.existsAsFile())
        {
            audioFile.replaceWithText("");
        }
        juce::WavAudioFormat wavFormat;
        std::unique_ptr<juce::AudioFormatWriter> writer;
        writer.reset(wavFormat.createWriterFor(new juce::FileOutputStream(juce::File(filePath)),
            wavSampleRate,
            buffer.getNumChannels(),
            wavBitDepth,
            {},
            0));
        if (writer != nullptr)
            writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());

        DBG("IR saved");
    }

    int getWavSampleRate()
    {
        return sampleRate;
    }

    juce::AudioBuffer<float> getWavAudioBuffer()
    {
        return audioBuffer;
    }

private:
    juce::AudioBuffer<float> audioBuffer;
    int sampleRate = 0;

    juce::AudioFormatManager formatManager;
};