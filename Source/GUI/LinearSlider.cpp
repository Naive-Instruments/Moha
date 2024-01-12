/*
  ==============================================================================

    LinearSlider.cpp
    Created: 12 Jan 2024 2:20:29pm
    Author:  sunwei06

  ==============================================================================
*/

#include <JuceHeader.h>
#include "LinearSlider.h"

//==============================================================================
LinearSlider::LinearSlider()
{
    setLookAndFeel(&customLookAndFeel);
    setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    setColour(juce::Slider::thumbColourId, juce::Colours::white);
    //setRange(0.0, 100.0, 0.01);
}

LinearSlider::~LinearSlider()
{
}

void LinearSlider::paint (juce::Graphics& g)
{
    juce::Slider::paint(g);
}
