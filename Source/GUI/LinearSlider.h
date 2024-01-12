/*
  ==============================================================================

    LinearSlider.h
    Created: 12 Jan 2024 2:20:29pm
    Author:  sunwei06

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CustomLookAndFeel.h"

//==============================================================================
/*
*/
class LinearSlider  : public juce::Slider
{
public:
    LinearSlider();
    ~LinearSlider() override;

    void paint (juce::Graphics&) override;

private:
    CustomLookAndFeel customLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LinearSlider)
};
