#include "MangroveLookAndFeel.h"

MangroveLookAndFeel::MangroveLookAndFeel()
{
    setColour(juce::Slider::textBoxTextColourId, juce::Colour(textColor));
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}

void MangroveLookAndFeel::drawRotarySlider(juce::Graphics& g,
                                            int x, int y, int width, int height,
                                            float sliderPos,
                                            float rotaryStartAngle,
                                            float rotaryEndAngle,
                                            juce::Slider&)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(8.0f);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto centre = bounds.getCentre();
    auto arcRadius = radius - 4.0f;
    float lineWidth = 3.0f;

    // Background track arc
    juce::Path bgArc;
    bgArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius,
                         0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(juce::Colour(trackColor));
    g.strokePath(bgArc, juce::PathStrokeType(lineWidth, juce::PathStrokeType::curved,
                                              juce::PathStrokeType::rounded));

    // Value arc
    float currentAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    juce::Path valueArc;
    valueArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius,
                            0.0f, rotaryStartAngle, currentAngle, true);
    g.setColour(juce::Colour(accentColor));
    g.strokePath(valueArc, juce::PathStrokeType(lineWidth, juce::PathStrokeType::curved,
                                                 juce::PathStrokeType::rounded));

    // Dot indicator at current position
    float dotRadius = 4.0f;
    float dotX = centre.x + (arcRadius) * std::cos(currentAngle - juce::MathConstants<float>::halfPi);
    float dotY = centre.y + (arcRadius) * std::sin(currentAngle - juce::MathConstants<float>::halfPi);
    g.setColour(juce::Colour(accentColor));
    g.fillEllipse(dotX - dotRadius, dotY - dotRadius, dotRadius * 2.0f, dotRadius * 2.0f);

    // Center dot
    g.setColour(juce::Colour(surfaceColor));
    g.fillEllipse(centre.x - 3.0f, centre.y - 3.0f, 6.0f, 6.0f);
}

void MangroveLookAndFeel::drawButtonBackground(juce::Graphics& g,
                                                juce::Button& button,
                                                const juce::Colour&,
                                                bool isHighlighted,
                                                bool isDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
    float cornerSize = bounds.getHeight() * 0.5f;

    if (button.getToggleState())
        g.setColour(juce::Colour(accentColor));
    else if (isHighlighted || isDown)
        g.setColour(juce::Colour(surfaceColor).brighter(0.1f));
    else
        g.setColour(juce::Colour(surfaceColor));

    g.fillRoundedRectangle(bounds, cornerSize);
}

void MangroveLookAndFeel::drawButtonText(juce::Graphics& g,
                                          juce::TextButton& button,
                                          bool,
                                          bool)
{
    auto colour = button.getToggleState()
        ? juce::Colour(bgColor)
        : juce::Colour(textColor);
    g.setColour(colour);
    g.setFont(juce::Font(12.0f));
    g.drawText(button.getButtonText(), button.getLocalBounds(),
               juce::Justification::centred);
}
