/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MohaAudioProcessorEditor::MohaAudioProcessorEditor (MohaAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 400);

    // Knobs
    createSlider(gainSlider, "");
    createLabel(gainLabel, "Gain", &gainSlider);
    gainSliderAttachment = std::make_unique<APVTS::SliderAttachment>(audioProcessor.apvts, "Gain", gainSlider);

    createSlider(preHighPassFreqSlider, " Hz");
    createLabel(preHighPassFreqLabel, "High Pass", &preHighPassFreqSlider);
    preHighPassFreqSliderAttachment = std::make_unique<APVTS::SliderAttachment>(audioProcessor.apvts, "HighPassFreq", preHighPassFreqSlider);
    
    createSlider(preLowPassFreqSlider, " Hz");
    createLabel(preLowPassFreqLabel, "Low Pass", &preLowPassFreqSlider);
    preLowPassFreqSliderAttachment = std::make_unique<APVTS::SliderAttachment>(audioProcessor.apvts, "LowPassFreq", preLowPassFreqSlider);

    createSlider(toneFreqSlider, " Hz");
    createLabel(toneFreqLabel, "Tone", &toneFreqSlider);
    toneFreqSliderAttachment = std::make_unique<APVTS::SliderAttachment>(audioProcessor.apvts, "ToneFreq", toneFreqSlider);
    
    createSlider(sensitivitySlider, " %");
    createLabel(sensitivityLabel, "Sensitivity", &sensitivitySlider);
    sensitivitySliderAttachment = std::make_unique<APVTS::SliderAttachment>(audioProcessor.apvts, "Sensitivity", sensitivitySlider);
    
    createSlider(speedSlider, " %");
    createLabel(speedLabel, "Speed", &speedSlider);
    speedSliderAttachment = std::make_unique<APVTS::SliderAttachment>(audioProcessor.apvts, "Speed", speedSlider);
    
    createSlider(darknessSlider, " %");
    createLabel(darknessLabel, "Darkness", &darknessSlider);
    darknessSliderAttachment = std::make_unique<APVTS::SliderAttachment>(audioProcessor.apvts, "Darkness", darknessSlider);

    createSlider(volumeSlider, " dB");
    createLabel(volumeLabel, "Volume", &volumeSlider);
    volumeSliderAttachment = std::make_unique<APVTS::SliderAttachment>(audioProcessor.apvts, "Volume", volumeSlider);
}

MohaAudioProcessorEditor::~MohaAudioProcessorEditor()
{
}

//==============================================================================
void MohaAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (32.0f);
    g.drawFittedText ("MOHA FROG PEDAL FX", getLocalBounds(), juce::Justification::centred, 1);
}

void MohaAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    const int topBottomMargin = 15;
    const int leftRightMargin = 15;

    const int dialWidth = 80;
    const int dialHeight = 90;

    gainSlider.setBounds(leftRightMargin + dialWidth - 6, getHeight() - topBottomMargin - dialHeight, dialWidth, dialHeight);

    preHighPassFreqSlider.setBounds(getWidth() - leftRightMargin - dialWidth * 3, getHeight() - 3 * topBottomMargin - 2 * dialHeight, dialWidth, dialHeight);
    
    preLowPassFreqSlider.setBounds(getWidth() - leftRightMargin - dialWidth * 2, getHeight() - 3 * topBottomMargin - 2 * dialHeight, dialWidth, dialHeight);

    toneFreqSlider.setBounds(getWidth() - leftRightMargin - dialWidth * 1, getHeight() - 3 * topBottomMargin - 2 * dialHeight, dialWidth, dialHeight);
    
    sensitivitySlider.setBounds(2 * leftRightMargin + 5, getHeight() - 3 * topBottomMargin - 3 * dialHeight, 2 * dialWidth, 2 * dialHeight);
    
    speedSlider.setBounds(getWidth() - leftRightMargin - dialWidth * 3, getHeight() - topBottomMargin - dialHeight, dialWidth, dialHeight);
    
    darknessSlider.setBounds(getWidth() - leftRightMargin - dialWidth * 2, getHeight() - topBottomMargin - dialHeight, dialWidth, dialHeight);
    
    volumeSlider.setBounds(getWidth() - leftRightMargin - dialWidth * 1, getHeight() - topBottomMargin - dialHeight, dialWidth, dialHeight);
}

void MohaAudioProcessorEditor::createSlider(juce::Slider& slider, juce::String textValueSuffix)
{
    addAndMakeVisible(slider);
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextValueSuffix(textValueSuffix);
    slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 60, 15);
    // slider.setPopupDisplayEnabled(true, true, this);
}

void MohaAudioProcessorEditor::createLabel(juce::Label& label, juce::String text, juce::Component* slider)
{
    addAndMakeVisible(label);
    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setBorderSize(juce::BorderSize<int>(0));
    label.attachToComponent(slider, false);
}
