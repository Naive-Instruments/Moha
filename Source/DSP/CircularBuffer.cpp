/*
  ==============================================================================

    CircularBuffer.cpp
    Created: 7 Nov 2023 5:37:10pm
    Author:  TaroPie

  ==============================================================================
*/

#include "CircularBuffer.h"

CircularBuffer::CircularBuffer(const int numChannels, const int bufferSize) : CBNumChannels(numChannels), CBBufferSize(bufferSize)
{
	// 初始化CircularBuffer，需要给定通道数和buffer大小
    circularBuffer.setSize(numChannels, bufferSize);
    circularBuffer.clear();
}

void CircularBuffer::fillBuffer(juce::AudioBuffer<float>& buffer)
{
    // 首先判断circular buffer里面剩余的空间够不够放下这次的buffer
    if (CBBufferSize >= buffer.getNumSamples() + fillWritePosition)
    {
        // 直接在当前的writePositon往后覆盖写入当前的buffer
        copyFromEveryChannel(circularBuffer, fillWritePosition, buffer, 0, buffer.getNumSamples());

        // 更新writePosition
        fillWritePosition += buffer.getNumSamples();
    }
    // 如果不够的话
    else
    {
        // 先看看最后还剩多少空间，全部都填上
        auto numSamplesToEnd = CBBufferSize - fillWritePosition;
        copyFromEveryChannel(circularBuffer, fillWritePosition, buffer, 0, numSamplesToEnd);

        // 减去刚刚填在最后的部分还剩下多少
        auto numSamplesAtStart = buffer.getNumSamples() - numSamplesToEnd;
        // 从circular buffer的头部覆盖写入剩下的内容
        copyFromEveryChannel(circularBuffer, 0, buffer, numSamplesToEnd, numSamplesAtStart);
        fillWritePosition = numSamplesAtStart;
    }
}

juce::AudioBuffer<float> CircularBuffer::readFromBuffer(const int numSamples)
{
    juce::AudioBuffer<float> returnBuffer(CBNumChannels, numSamples);
    returnBuffer.clear();

    // 获取numSamples前的audio
    auto readPosition = fillWritePosition - numSamples;
    // 当writePosition位于circular buffer的开头位置时，这里的readPositon可能为负数，这是不允许出现的
    // 把它指向应该指向的位置，即circular buffer的尾部区域
    if (readPosition < 0)
        readPosition += CBBufferSize;
    
    for (int channel = 0; channel < CBNumChannels; ++channel)
    {
        if (readPosition + numSamples < CBBufferSize)
        {
            returnBuffer.addFrom(channel, 0, circularBuffer.getReadPointer(channel, readPosition), numSamples);
        }
        else
        {
            auto numSamplesToEnd = CBBufferSize - readPosition;
            returnBuffer.addFrom(channel, 0, circularBuffer.getReadPointer(channel, readPosition), numSamplesToEnd);

            auto numSamplesAtStart = numSamples - numSamplesToEnd;
            returnBuffer.addFrom(channel, numSamplesToEnd, circularBuffer.getReadPointer(channel, 0), numSamplesAtStart);
        }
    }

    return returnBuffer;
}

void CircularBuffer::resetBufferFromCircularBuffer(juce::AudioBuffer<float>& buffer, const int numSamples)
{
    buffer.clear();

    if (isLooping == false)
    {
        loopBuffer.makeCopyOf(readFromBuffer(numSamples));
        isLooping = true;
    }

    // 如果需要被loop的buffer小于输入进来的需要被替换的buffer
    // readPositon: 被loop的buffer读取到的位置
    // writePosition: 被替换的buffer写入到的位置
    if (numSamples < buffer.getNumSamples())
    {
        // 猛猛塞
        while (resetWritePosition + numSamples < buffer.getNumSamples())
        {
            copyFromEveryChannel(buffer, resetWritePosition, loopBuffer, resetReadPosition, loopBuffer.getNumSamples() - resetReadPosition);
            resetWritePosition += loopBuffer.getNumSamples() - resetReadPosition;
            resetReadPosition = 0;
        }
        int oriNumSamplesToEnd = buffer.getNumSamples() - resetWritePosition;
        copyFromEveryChannel(buffer, resetWritePosition, loopBuffer, 0, oriNumSamplesToEnd);
        resetReadPosition = oriNumSamplesToEnd;
        resetWritePosition = 0;
    }
    // 如果需要被loop的buffer大于输入进来的需要被替换的buffer（大概率是进这）
    else
    {
        if (resetReadPosition + buffer.getNumSamples() < numSamples)
        {
            copyFromEveryChannel(buffer, 0, loopBuffer, resetReadPosition, buffer.getNumSamples());
            resetReadPosition += buffer.getNumSamples();
        }
        else
        {
            int loopNumSamplesToEnd = numSamples - resetReadPosition;
            copyFromEveryChannel(buffer, 0, loopBuffer, resetReadPosition, loopNumSamplesToEnd);
            resetReadPosition = buffer.getNumSamples() - loopNumSamplesToEnd;
            copyFromEveryChannel(buffer, loopNumSamplesToEnd, loopBuffer, 0, resetReadPosition);
        }
    }
}
                                 
void CircularBuffer::copyFromEveryChannel(juce::AudioBuffer<float>& dest, const int destStartSample,
                                          const juce::AudioBuffer<float>& source, const int sourceStartSample,
                                          const int numSamples)
{
    if (dest.getNumChannels() != source.getNumChannels())
    {
        DBG("Number of channels does not match!");
        jassertfalse;
    }

    for (int channel = 0; channel < dest.getNumChannels(); ++channel)
		dest.copyFrom(channel, destStartSample, source, channel, sourceStartSample, numSamples);
}