#include "PluginEditor.h"

MangroveEditor::MangroveEditor(MangroveProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setLookAndFeel(&lookAndFeel);
    setSize(600, 450);

    setupKnob(pitchKnob, pitchLabel, "PITCH");
    setupKnob(fineKnob, fineLabel, "FINE");
    setupKnob(formantKnob, formantLabel, "FORMANT");
    setupKnob(barrelKnob, barrelLabel, "BARREL");
    setupKnob(airKnob, airLabel, "AIR");
    setupKnob(fmKnob, fmLabel, "FM");
    setupKnob(mixKnob, mixLabel, "MIX");
    setupKnob(spreadKnob, spreadLabel, "SPREAD");
    setupKnob(masterKnob, masterLabel, "MASTER");

    pitchAttach   = std::make_unique<SliderAttachment>(processorRef.apvts, "PITCH", pitchKnob);
    fineAttach    = std::make_unique<SliderAttachment>(processorRef.apvts, "FINE", fineKnob);
    formantAttach = std::make_unique<SliderAttachment>(processorRef.apvts, "FORMANT", formantKnob);
    barrelAttach  = std::make_unique<SliderAttachment>(processorRef.apvts, "BARREL", barrelKnob);
    airAttach     = std::make_unique<SliderAttachment>(processorRef.apvts, "AIR", airKnob);
    fmAttach      = std::make_unique<SliderAttachment>(processorRef.apvts, "FM_INDEX", fmKnob);
    mixAttach     = std::make_unique<SliderAttachment>(processorRef.apvts, "MIX", mixKnob);
    spreadAttach  = std::make_unique<SliderAttachment>(processorRef.apvts, "SPREAD", spreadKnob);
    masterAttach  = std::make_unique<SliderAttachment>(processorRef.apvts, "MASTER", masterKnob);

    waveButton.setRadioGroupId(1001);
    formantButton.setRadioGroupId(1001);
    waveButton.setClickingTogglesState(true);
    formantButton.setClickingTogglesState(true);

    waveButton.onClick = [this]() {
        auto* param = processorRef.apvts.getParameter("MODE");
        param->setValueNotifyingHost(param->convertTo0to1(0.0f));
        updateModeButtons();
    };
    formantButton.onClick = [this]() {
        auto* param = processorRef.apvts.getParameter("MODE");
        param->setValueNotifyingHost(param->convertTo0to1(1.0f));
        updateModeButtons();
    };

    addAndMakeVisible(waveButton);
    addAndMakeVisible(formantButton);
    updateModeButtons();
}

MangroveEditor::~MangroveEditor()
{
    setLookAndFeel(nullptr);
}

void MangroveEditor::setupKnob(juce::Slider& knob, juce::Label& label,
                                 const juce::String& text)
{
    knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(knob);

    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId,
                    juce::Colour(MangroveLookAndFeel::textDimColor));
    label.setFont(juce::Font(11.0f));
    addAndMakeVisible(label);
}

void MangroveEditor::updateModeButtons()
{
    float modeVal = processorRef.apvts.getParameter("MODE")->getValue();
    bool isFormant = modeVal > 0.5f;
    waveButton.setToggleState(!isFormant, juce::dontSendNotification);
    formantButton.setToggleState(isFormant, juce::dontSendNotification);
}

void MangroveEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(MangroveLookAndFeel::bgColor));

    g.setColour(juce::Colour(MangroveLookAndFeel::textColor));
    g.setFont(juce::Font(20.0f).boldened());
    g.drawText("MANGROVE", 20, 15, 200, 30, juce::Justification::centredLeft);

    g.setColour(juce::Colour(MangroveLookAndFeel::trackColor));
    g.drawHorizontalLine(250, 40.0f, 560.0f);
}

void MangroveEditor::resized()
{
    int knobSize = 80;
    int labelHeight = 20;

    int toggleW = 60, toggleH = 26;
    waveButton.setBounds(getWidth() - 140, 18, toggleW, toggleH);
    formantButton.setBounds(getWidth() - 75, 18, toggleW, toggleH);

    int topY = 70;
    int topRowWidth = 5 * knobSize + 4 * 20;
    int topStartX = (getWidth() - topRowWidth) / 2;

    auto placeKnob = [&](juce::Slider& knob, juce::Label& label, int x, int y) {
        knob.setBounds(x, y, knobSize, knobSize);
        label.setBounds(x, y + knobSize - 5, knobSize, labelHeight);
    };

    int spacing = knobSize + 20;
    placeKnob(pitchKnob,   pitchLabel,   topStartX,                topY);
    placeKnob(fineKnob,    fineLabel,    topStartX + spacing,      topY);
    placeKnob(formantKnob, formantLabel, topStartX + spacing * 2,  topY);
    placeKnob(barrelKnob,  barrelLabel,  topStartX + spacing * 3,  topY);
    placeKnob(airKnob,     airLabel,     topStartX + spacing * 4,  topY);

    int botY = 280;
    int botRowWidth = 4 * knobSize + 3 * 20;
    int botStartX = (getWidth() - botRowWidth) / 2;

    placeKnob(fmKnob,     fmLabel,     botStartX,                botY);
    placeKnob(mixKnob,    mixLabel,    botStartX + spacing,      botY);
    placeKnob(spreadKnob, spreadLabel, botStartX + spacing * 2,  botY);
    placeKnob(masterKnob, masterLabel, botStartX + spacing * 3,  botY);
}
