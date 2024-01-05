/*
  ==============================================================================

    CircularBuffer.cpp
    Created: 7 Nov 2023 5:37:10pm
    Author:  TaroPie

  ==============================================================================
*/

#include "CircularBuffer.h"

CircularBuffer::CircularBuffer(int numChannels, int bufferSize) : numChannels(numChannels), bufferSize(bufferSize)
{
	// 初始化CircularBuffer，需要给定通道数和buffer大小
    circularBuffer.setSize(numChannels, bufferSize);
    circularBuffer.clear();
}

void CircularBuffer::fillBuffer(juce::AudioBuffer<float>& buffer)
{
    if (!isFillBuffer)
		return;

    for (int channel = 0; channel < numChannels; ++channel)
    {
        // 首先判断circular buffer里面剩余的空间够不够放下这次的buffer
        if (bufferSize >= buffer.getNumSamples() + writePosition)
        {
            // 直接在当前的writePositon往后覆盖写入当前的buffer
            circularBuffer.copyFrom(channel, writePosition, buffer.getReadPointer(channel), buffer.getNumSamples());

            // 更新writePosition
            writePosition += buffer.getNumSamples();
        }
        // 如果不够的话
        else
        {
            // 先看看最后还剩多少空间，全部都填上
            auto numSamplesToEnd = bufferSize - writePosition;
            circularBuffer.copyFrom(channel, writePosition, buffer.getReadPointer(channel), numSamplesToEnd);

            // 减去刚刚填在最后的部分还剩下多少
            auto numSamplesAtStart = buffer.getNumSamples() - numSamplesToEnd;
            // 从circular buffer的头部覆盖写入剩下的内容
            circularBuffer.copyFrom(channel, 0, buffer.getReadPointer(channel, numSamplesToEnd), numSamplesAtStart);

            writePosition = numSamplesAtStart;
        }
	}
}

juce::AudioBuffer<float> CircularBuffer::readFromBuffer(int numSamples)
{
    juce::AudioBuffer<float> returnBuffer(numChannels, numSamples);
    returnBuffer.clear();

    // 获取numSamples前的audio
    auto readPosition = writePosition - numSamples;
    // 当writePosition位于circular buffer的开头位置时，这里的readPositon可能为负数，这是不允许出现的
    // 把它指向应该指向的位置，即circular buffer的尾部区域
    if (readPosition < 0)
        readPosition += bufferSize;
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        if (readPosition + numSamples < bufferSize)
        {
            returnBuffer.addFrom(channel, 0, circularBuffer.getReadPointer(channel, readPosition), numSamples);
        }
        else
        {
            auto numSamplesToEnd = bufferSize - readPosition;
            returnBuffer.addFrom(channel, 0, circularBuffer.getReadPointer(channel, readPosition), numSamplesToEnd);

            auto numSamplesAtStart = numSamples - numSamplesToEnd;
            returnBuffer.addFrom(channel, numSamplesToEnd, circularBuffer.getReadPointer(channel, 0), numSamplesAtStart);
        }
    }

    return returnBuffer;
}

void CircularBuffer::resetBufferFromCircularBuffer(juce::AudioBuffer<float>& buffer, int numSamples)
{
    pauseFillBuffer();

    buffer.clear();

    juce::AudioBuffer<float> loopBuffer(readFromBuffer(numSamples));

    for (int channel = 0; channel < numChannels; ++channel)
    {
        // 如果需要被loop的buffer小于输入进来的需要被替换的buffer
        // readPositon: 被loop的buffer读取到的位置
        // writePosition: 被替换的buffer写入到的位置
        if (numSamples < buffer.getNumSamples())
        {
            // 猛猛塞
            while (writePosition + numSamples < buffer.getNumSamples())
            {
                buffer.copyFrom(channel, writePosition, loopBuffer, 0, readPosition, loopBuffer.getNumSamples() - readPosition);
                writePosition += loopBuffer.getNumSamples();
                readPosition = 0;
            }
            int oriNumSamplesToEnd = buffer.getNumSamples() - writePosition;
            buffer.copyFrom(channel, writePosition, loopBuffer, 0, 0, oriNumSamplesToEnd);
            readPosition = oriNumSamplesToEnd;
            writePosition = 0;
        }
        // 如果需要被loop的buffer大于输入进来的需要被替换的buffer
        else
        {
            while (readPosition + buffer.getNumSamples() < numSamples)
            {
                buffer.copyFrom(channel, 0, loopBuffer, 0, readPosition, buffer.getNumSamples());
                readPosition += buffer.getNumSamples();
            }
            int loopNumSamplesToEnd = numSamples - readPosition;
            buffer.copyFrom(channel, 0, loopBuffer, 0, readPosition, loopNumSamplesToEnd);
            readPosition = buffer.getNumSamples() - loopNumSamplesToEnd;
            buffer.copyFrom(channel, loopNumSamplesToEnd, loopBuffer, 0, 0, readPosition);
        }
    }
}