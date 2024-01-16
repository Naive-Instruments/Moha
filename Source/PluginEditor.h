/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/CustomStyle.h"
#include "GUI/CustomLookAndFeel.h"
#include "GUI/RotarySlider.h"
#include "GUI/LinearSlider.h"
#include "GUI/SpectrumComponent.h"

//==============================================================================
/**
*/
class MohaAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    MohaAudioProcessorEditor (MohaAudioProcessor&);
    ~MohaAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MohaAudioProcessor& audioProcessor;

    CustomLookAndFeel customLookAndFeel;
    //juce::CustomStyle customStyle;

    using APVTS = juce::AudioProcessorValueTreeState;

    // Display component handles

    RotarySlider gainSlider;
    juce::Label gainLabel;
    std::unique_ptr<APVTS::SliderAttachment> gainSliderAttachment;

    RotarySlider preHighPassFreqSlider;
    juce::Label preHighPassFreqLabel;
    std::unique_ptr<APVTS::SliderAttachment> preHighPassFreqSliderAttachment;

    RotarySlider preLowPassFreqSlider;
    juce::Label preLowPassFreqLabel;
    std::unique_ptr<APVTS::SliderAttachment> preLowPassFreqSliderAttachment;

    RotarySlider toneFreqSlider;
    juce::Label toneFreqLabel;
    std::unique_ptr<APVTS::SliderAttachment> toneFreqSliderAttachment;

    RotarySlider sensitivitySlider;
    juce::Label sensitivityLabel;
    std::unique_ptr<APVTS::SliderAttachment> sensitivitySliderAttachment;

    RotarySlider speedSlider;
    juce::Label speedLabel;
    std::unique_ptr<APVTS::SliderAttachment> speedSliderAttachment;

    RotarySlider darknessSlider;
    juce::Label darknessLabel;
    std::unique_ptr<APVTS::SliderAttachment> darknessSliderAttachment;

    RotarySlider volumeSlider;
    juce::Label volumeLabel;
    std::unique_ptr<APVTS::SliderAttachment> volumeSliderAttachment;

    void createSlider(juce::Slider& slider, juce::String textValueSuffix);
    void createLabel(juce::Label& label, juce::String text,
        juce::Component* slider);

    RotarySlider rotarySlider;
    LinearSlider linearSlider;

    SpectrumComponent spectrum;

    juce::String compileTime;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MohaAudioProcessorEditor)
};
