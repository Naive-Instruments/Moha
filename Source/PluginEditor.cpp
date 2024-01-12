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

    //juce::LookAndFeel::setDefaultLookAndFeel(&customLookAndFeel);
    //juce::LookAndFeel::setDefaultLookAndFeel(&customStyle);

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
    createLabel(sensitivityLabel, "Speed", &sensitivitySlider);
    sensitivitySliderAttachment = std::make_unique<APVTS::SliderAttachment>(audioProcessor.apvts, "Speed", sensitivitySlider);
    
    createSlider(speedSlider, " %");
    toneFreqSlider.setValueArcColour(juce::Colour::fromFloatRGBA(0.43f, 0.83f, 1.0f, 1.0f));
    createLabel(speedLabel, "Slew", &speedSlider);
    speedSliderAttachment = std::make_unique<APVTS::SliderAttachment>(audioProcessor.apvts, "Slew", speedSlider);
    
    createSlider(darknessSlider, " %");
    darknessSlider.setValueArcColour(juce::Colour::fromFloatRGBA(0.34f, 0.74f, 0.66f, 1.0f));
    createLabel(darknessLabel, "Darkness", &darknessSlider);
    darknessSliderAttachment = std::make_unique<APVTS::SliderAttachment>(audioProcessor.apvts, "Darkness", darknessSlider);

    createSlider(volumeSlider, " dB");
    volumeSlider.setValueArcColour(juce::Colour::fromFloatRGBA(1.0f, 0.71f, 0.2f, 1.0f));
    createLabel(volumeLabel, "Volume", &volumeSlider);
    volumeSliderAttachment = std::make_unique<APVTS::SliderAttachment>(audioProcessor.apvts, "Volume", volumeSlider);

    addAndMakeVisible(linearSlider);
}

MohaAudioProcessorEditor::~MohaAudioProcessorEditor()
{
}

//==============================================================================
void MohaAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromFloatRGBA(0.f, 0.f, 0.f, 0.65f));
    //g.fillAll(juce::Colours::white);

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

    const int dialWidth = 90;
    const int dialHeight = 90;

    linearSlider.setBounds(leftRightMargin, topBottomMargin, 200, 30);

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
    addAndMakeVisible(slider);/*
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextValueSuffix(textValueSuffix);
    slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 60, 15);*/
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
