/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomStyle.h"

//==============================================================================
/**
*/
class MohaAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MohaAudioProcessorEditor (MohaAudioProcessor&);
    ~MohaAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MohaAudioProcessor& audioProcessor;
    juce::CustomStyle customStyle;

    using APVTS = juce::AudioProcessorValueTreeState;

    // Display component handles

    juce::Slider gainSlider;
    juce::Label gainLabel;
    std::unique_ptr<APVTS::SliderAttachment> gainSliderAttachment;

    juce::Slider preHighPassFreqSlider;
    juce::Label preHighPassFreqLabel;
    std::unique_ptr<APVTS::SliderAttachment> preHighPassFreqSliderAttachment;

    juce::Slider preLowPassFreqSlider;
    juce::Label preLowPassFreqLabel;
    std::unique_ptr<APVTS::SliderAttachment> preLowPassFreqSliderAttachment;

    juce::Slider toneFreqSlider;
    juce::Label toneFreqLabel;
    std::unique_ptr<APVTS::SliderAttachment> toneFreqSliderAttachment;

    juce::Slider sensitivitySlider;
    juce::Label sensitivityLabel;
    std::unique_ptr<APVTS::SliderAttachment> sensitivitySliderAttachment;

    juce::Slider speedSlider;
    juce::Label speedLabel;
    std::unique_ptr<APVTS::SliderAttachment> speedSliderAttachment;

    juce::Slider darknessSlider;
    juce::Label darknessLabel;
    std::unique_ptr<APVTS::SliderAttachment> darknessSliderAttachment;

    juce::Slider volumeSlider;
    juce::Label volumeLabel;
    std::unique_ptr<APVTS::SliderAttachment> volumeSliderAttachment;

    void createSlider(juce::Slider& slider, juce::String textValueSuffix);
    void createLabel(juce::Label& label, juce::String text,
        juce::Component* slider);

    melatonin::DropShadow shadow = { { juce::Colours::black, 8, { -2, 0 } } };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MohaAudioProcessorEditor)
};
