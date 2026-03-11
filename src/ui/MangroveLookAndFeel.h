#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class MangroveLookAndFeel : public juce::LookAndFeel_V4
{
public:
    MangroveLookAndFeel();

    void drawRotarySlider(juce::Graphics& g,
                          int x, int y, int width, int height,
                          float sliderPos,
                          float rotaryStartAngle,
                          float rotaryEndAngle,
                          juce::Slider&) override;

    void drawButtonBackground(juce::Graphics& g,
                              juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool isHighlighted,
                              bool isDown) override;

    void drawButtonText(juce::Graphics& g,
                        juce::TextButton& button,
                        bool isHighlighted,
                        bool isDown) override;

    static constexpr juce::uint32 bgColor       = 0xff1a1a1a;
    static constexpr juce::uint32 surfaceColor   = 0xff252525;
    static constexpr juce::uint32 accentColor    = 0xff00bcd4;
    static constexpr juce::uint32 accentDim      = 0xff007888;
    static constexpr juce::uint32 textColor      = 0xffcccccc;
    static constexpr juce::uint32 textDimColor   = 0xff888888;
    static constexpr juce::uint32 trackColor     = 0xff333333;
};
