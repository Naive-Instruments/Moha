/*
  ==============================================================================

    ShadowSlider.h
    Created: 11 Jan 2024 3:19:18pm
    Author:  sunwei06

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class ShadowSlider  : public juce::Component
{
public:
    ShadowSlider();
    ~ShadowSlider() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    melatonin::DropShadow shadow = { { juce::Colours::black, 8, { -2, 0 } } };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShadowSlider)
};
