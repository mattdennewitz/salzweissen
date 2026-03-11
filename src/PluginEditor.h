#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "ui/MangroveLookAndFeel.h"

class MangroveEditor : public juce::AudioProcessorEditor
{
public:
    explicit MangroveEditor(MangroveProcessor&);
    ~MangroveEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    MangroveProcessor& processorRef;
    MangroveLookAndFeel lookAndFeel;

    juce::Slider pitchKnob, fineKnob, formantKnob, barrelKnob, airKnob;
    juce::Label  pitchLabel, fineLabel, formantLabel, barrelLabel, airLabel;

    juce::Slider fmKnob, mixKnob, spreadKnob, masterKnob;
    juce::Label  fmLabel, mixLabel, spreadLabel, masterLabel;

    juce::TextButton waveButton{"WAVE"};
    juce::TextButton formantButton{"FORM"};

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> pitchAttach, fineAttach, formantAttach,
                                       barrelAttach, airAttach, fmAttach,
                                       mixAttach, spreadAttach, masterAttach;

    void setupKnob(juce::Slider& knob, juce::Label& label, const juce::String& text);
    void updateModeButtons();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MangroveEditor)
};
