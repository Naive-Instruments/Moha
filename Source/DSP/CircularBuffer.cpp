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
    for (int channel = 0; channel < numChannels; ++channel)
    {
        // 首先判断circular buffer里面剩余的空间够不够放下这次的buffer
        if (bufferSize >= buffer.getNumSamples() + writePosition)
        {
            // 直接在当前的writePositon往后覆盖写入当前的buffer
            circularBuffer.copyFrom(channel, writePosition, buffer.getWritePointer(channel), bufferSize);
        }
        // 如果不够的话
        else
        {
            // 先看看最后还剩多少空间，全部都填上
            auto numSamplesToEnd = buffer.getNumSamples() - writePosition;
            circularBuffer.copyFrom(channel, writePosition, buffer.getWritePointer(channel), numSamplesToEnd);

            // 再看看bufferSize减去刚刚填在最后的部分还剩下多少
            auto numSamplesAtStart = bufferSize - numSamplesToEnd;
            // 从circular buffer的头部覆盖写入剩下的内容
            circularBuffer.copyFrom(channel, 0, buffer.getWritePointer(channel, numSamplesToEnd), numSamplesAtStart);
        }
	}

    // 更新writePosition
    writePosition += bufferSize;
}

juce::AudioBuffer<float> CircularBuffer::readFromBuffer(int numSamples)
{
    juce::AudioBuffer<float> returnBuffer(numChannels, numSamples);
    returnBuffer.clear();

    // 获取numSamples前的audio
    auto readPosition = writePosition - numSamples;
    // 值得注意的是，当writePosition位于circular buffer的开头位置时，这里的readPositon可能为负数，但这是不允许出现的
    // 所以我们把它指向应该指向的位置，即circular buffer的尾部区域
    if (readPosition < 0)
        readPosition += bufferSize;
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        if (readPosition + numSamples < bufferSize)
        {
            returnBuffer.addFrom(channel, 0, circularBuffer.getReadPointer(channel, readPosition), bufferSize);
        }
        else
        {
            auto numSamplesToEnd = bufferSize - readPosition;
            returnBuffer.addFrom(channel, 0, circularBuffer.getReadPointer(channel, readPosition), numSamplesToEnd);

            auto numSamplesAtStart = bufferSize - numSamplesToEnd;
            returnBuffer.addFrom(channel, numSamplesToEnd, circularBuffer.getReadPointer(channel, 0), numSamplesAtStart);
        }
    }

    return returnBuffer;
}