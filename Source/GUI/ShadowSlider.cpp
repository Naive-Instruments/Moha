/*
  ==============================================================================

    ShadowSlider.cpp
    Created: 11 Jan 2024 3:19:18pm
    Author:  sunwei06

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ShadowSlider.h"

//==============================================================================
ShadowSlider::ShadowSlider()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

ShadowSlider::~ShadowSlider()
{
}

void ShadowSlider::paint (juce::Graphics& g)
{
    juce::Path valueTrack;
    valueTrack.addRoundedRectangle(10, 10, 100, 20, 2);

    shadow.render(g, valueTrack);

    g.setColour(juce::Colours::red);
    //path.fillPath(valueTrack);
}

void ShadowSlider::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
