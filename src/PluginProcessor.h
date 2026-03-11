#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "dsp/MangroveVoice.h"

class MangroveProcessor : public juce::AudioProcessor
{
public:
    MangroveProcessor();
    ~MangroveProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int) override;
    const juce::String getProgramName(int) override;
    void changeProgramName(int, const juce::String&) override;

    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    bool isBusesLayoutSupported(const BusesLayout&) const override;

    juce::AudioProcessorValueTreeState apvts;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    MangroveVoice voice;

    std::atomic<float>* pitchParam = nullptr;
    std::atomic<float>* fineParam = nullptr;
    std::atomic<float>* formantParam = nullptr;
    std::atomic<float>* barrelParam = nullptr;
    std::atomic<float>* airParam = nullptr;
    std::atomic<float>* fmIndexParam = nullptr;
    std::atomic<float>* modeParam = nullptr;
    std::atomic<float>* mixParam = nullptr;
    std::atomic<float>* spreadParam = nullptr;
    std::atomic<float>* masterParam = nullptr;

    juce::dsp::Oversampling<float> oversampler{2, 1,
        juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, false};

    int currentNote = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MangroveProcessor)
};
