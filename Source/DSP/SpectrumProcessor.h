#include <JuceHeader.h>
#pragma once

class SpectrumProcessor
{
public:
    SpectrumProcessor() : forwardFFT (fftOrder), window (fftSize, juce::dsp::WindowingFunction<float>::hamming)
    {
        window.fillWindowingTables (fftSize, juce::dsp::WindowingFunction<float>::blackman);
    }

    enum
    {
        fftOrder = 11,
        fftSize = 1 << fftOrder, // 2048£, frequency resolution is 48kHz / 2048 = 23.44Hz
        numBins = fftSize / 2 // 1024
    };

    float fftData[2 * fftSize] = { 0 };
    bool nextFFTBlockReady = false;

    // Push each sample into fifo, if the size of fifo reaches fftSize and nextFFTBlockReady is false, copy the data from fifo to fftData
    void pushNextSampleIntoFifo (float sample) noexcept
    {
        if (fifoIndex == fftSize)
        {
            if (!nextFFTBlockReady)
            {
                juce::zeromem(fftData, sizeof(fftData));
                memmove(fftData, fifo, sizeof (fifo)); // memmove is safer, but memcpy is faster
                nextFFTBlockReady = true;
            }

            fifoIndex = 0;
        }

        fifo[fifoIndex++] = sample;
    }

    // Get current buffer from processBlock
    void pushDataToFFT(juce::AudioBuffer<float>& audioBuffer)
    {
        if (audioBuffer.getNumChannels() > 0)
        {
            auto* channelData = audioBuffer.getReadPointer(0);

            for (auto i = 0; i < audioBuffer.getNumSamples(); ++i)
                pushNextSampleIntoFifo(channelData[i]);
        }
    }

    void processFFT(float* tempFFTData)
    {
        // Add windows before processing to prevent spectrum leakage
        window.multiplyWithWindowingTable(tempFFTData, fftSize);
        forwardFFT.performFrequencyOnlyForwardTransform(tempFFTData);

        nextFFTBlockReady = false;
    }

    float* getFFTData() { return fftData; }
    int getNumBins() { return numBins; }
    int getFFTSize() { return fftSize; }

private:
    float fifo[fftSize];
    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;
    int fifoIndex = 0;
};
