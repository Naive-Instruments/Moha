
#include <JuceHeader.h>
#include "CustomLookAndFeel.h"

CustomLookAndFeel::CustomLookAndFeel(){};
CustomLookAndFeel::~CustomLookAndFeel(){};

juce::Slider::SliderLayout CustomLookAndFeel::getSliderLayout(juce::Slider& slider)
{
    auto localBounds = slider.getLocalBounds();
    
    juce::Slider::SliderLayout layout;

    //layout.textBoxBounds = localBounds;
    layout.sliderBounds = localBounds;

    return layout;
}

void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                         const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
{
    auto fill = slider.findColour(juce::Slider::rotarySliderFillColourId);
    //auto fill = juce::Colours::darkgrey;

    auto bounds = juce::Rectangle<float>(x, y, width, height).reduced(6.0f);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = radius * 0.085f;
    auto arcRadius = radius - lineW * 1.5f;
    
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(bounds.getCentreX(),
                                bounds.getCentreY(),
                                arcRadius,
                                arcRadius,
                                0.0f,
                                rotaryStartAngle,
                                rotaryEndAngle,
                                true);
    
    g.setColour(blackGrey);
    g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
    juce::Path valueArc;
    valueArc.addCentredArc(bounds.getCentreX(),
                           bounds.getCentreY(),
                           arcRadius,
                           arcRadius,
                           0.0f,
                           rotaryStartAngle,
                           toAngle,
                           true);
        
    //auto alpha = 0.1f + (float) slider.getValue() * 0.9f;
    //auto brightness = 0.4f + (float) slider.getValue() * 0.6f;

    //g.setColour (fill.withAlpha (alpha).brighter (brightness));
    g.setColour(fill);
    juce::PathStrokeType strokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    g.strokePath(valueArc, strokeType);

    auto thumbWidth = lineW * 2.0f;
 
    g.setColour(blackGrey);
    g.fillEllipse(bounds.reduced(8.0f));

    juce::Path thumb;
    //thumb.addRectangle(-thumbWidth / 2, -thumbWidth / 2, thumbWidth, radius + lineW);
    thumb.addEllipse(-thumbWidth, thumbWidth * 2.5, thumbWidth, thumbWidth);
    
    g.setColour(juce::Colours::black);
    g.fillPath(thumb, juce::AffineTransform::rotation(toAngle + 3.f).translated(bounds.getCentre()));

    juce::Path valueBlurPath;
    strokeType.createStrokedPath(valueBlurPath, valueArc);
    g.setColour(fill);
    melatonin::DropShadow valueBlurShadow = { fill, 12, { 0, 0 }, 0 };
    valueBlurShadow.render(g, valueBlurPath);

    juce::Path SliderBlurArc;
    juce::Path sliderBlurPath;
    SliderBlurArc.addCentredArc(bounds.getCentreX(),
                                bounds.getCentreY(),
                                arcRadius - 3.f,
                                arcRadius - 3.f,
                                0.0f,
                                juce::degreesToRadians(150.f),
                                juce::degreesToRadians(210.f),
                                true);

    strokeType.createStrokedPath(sliderBlurPath, SliderBlurArc);
    g.setColour(juce::Colours::black);
    melatonin::DropShadow sliderBlurShadow = { juce::Colours::black, 6, { 0, 0 }, 0 };
	sliderBlurShadow.render(g, sliderBlurPath);   
}

juce::Label* CustomLookAndFeel::createSliderTextBox (juce::Slider& slider)
{
    auto* l = new juce::Label();
    l->setJustificationType(juce::Justification::centred);
    l->setColour(juce::Label::textColourId, slider.findColour (juce::Slider::textBoxTextColourId));
    l->setColour(juce::Label::textWhenEditingColourId, slider.findColour (juce::Slider::textBoxTextColourId));
    l->setColour(juce::Label::outlineWhenEditingColourId, slider.findColour (juce::Slider::textBoxOutlineColourId));
    l->setInterceptsMouseClicks(false, false);
    l->setFont (15);
    
    return l;
}

void CustomLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPos,
                                         float minSliderPos,
                                         float maxSliderPos,
                                         juce::Slider::SliderStyle style, juce::Slider& slider)
{
    auto bounds = juce::Rectangle<float>(x, y, width, height);

    juce::Path baseLine;
    float baseLineHeight = 4.f;

    baseLine.addRoundedRectangle(bounds.getX() + 6.f, bounds.getHeight() * 0.5f + baseLineHeight * 0.5f, bounds.getWidth() - 12.f, baseLineHeight, 2.f);

    g.setColour(grey);
    g.fillPath(baseLine);
    //g.fillRect(bounds);

    auto sliderRadius = (float)6.5f;//(getSliderThumbRadius (slider) - 2);

    auto isDownOrDragging = slider.isEnabled() && (slider.isMouseOverOrDragging() || slider.isMouseButtonDown());

    auto knobColour = slider.findColour(juce::Slider::thumbColourId);
    //    .withMultipliedSaturation((slider.hasKeyboardFocus(false) || isDownOrDragging) ? 1.3f : 0.9f)
    //    .withMultipliedAlpha(slider.isEnabled() ? 1.0f : 0.7f);

    juce::Path thumb;
    thumb.addEllipse(juce::jmap(sliderPos, 0.f, 200.f, bounds.getX(), bounds.getWidth() - 12.f), bounds.getHeight() * 0.5f - 1.5f, 12.f, 12.f);
    //thumb.addEllipse(sliderPos, bounds.getHeight() * 0.5f - 1.f, 12.f, 12.f);

    g.setColour(knobColour);
    g.fillPath(thumb);

    g.setColour(knobColour.brighter());
    //g.strokePath(p, juce::PathStrokeType(outlineThickness));
}