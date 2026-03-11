#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters.h"

SalzwiesenProcessor::SalzwiesenProcessor()
    : AudioProcessor(BusesProperties()
        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    pitchParam   = apvts.getRawParameterValue(ParamIDs::pitch.getParamID());
    fineParam    = apvts.getRawParameterValue(ParamIDs::fine.getParamID());
    formantParam = apvts.getRawParameterValue(ParamIDs::formant.getParamID());
    barrelParam  = apvts.getRawParameterValue(ParamIDs::barrel.getParamID());
    airParam     = apvts.getRawParameterValue(ParamIDs::air.getParamID());
    fmIndexParam = apvts.getRawParameterValue(ParamIDs::fmIndex.getParamID());
    modeParam    = apvts.getRawParameterValue(ParamIDs::mode.getParamID());
    mixParam     = apvts.getRawParameterValue(ParamIDs::mix.getParamID());
    spreadParam  = apvts.getRawParameterValue(ParamIDs::spread.getParamID());
    masterParam  = apvts.getRawParameterValue(ParamIDs::master.getParamID());

    jassert(pitchParam && fineParam && formantParam && barrelParam && airParam);
    jassert(fmIndexParam && modeParam && mixParam && spreadParam && masterParam);
}

SalzwiesenProcessor::~SalzwiesenProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout
SalzwiesenProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::pitch, "Pitch",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::fine, "Fine",
        juce::NormalisableRange<float>(-0.5f, 0.5f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::formant, "Formant",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::barrel, "Barrel",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::air, "Air",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::fmIndex, "FM Index",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        ParamIDs::mode, "Mode",
        juce::StringArray{"Wave", "Formant"}, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::mix, "Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f), 1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::spread, "Spread",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::master, "Master",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f));

    return layout;
}

void SalzwiesenProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    voice.prepare(sampleRate);
}

void SalzwiesenProcessor::releaseResources() {}

void SalzwiesenProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                      juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    if (numSamples == 0) return;

    voice.setPitchAndFine(pitchParam->load(), fineParam->load());
    voice.setFormant(formantParam->load());
    voice.setBarrel(barrelParam->load());
    voice.setAir(airParam->load());
    voice.setFmIndex(fmIndexParam->load());
    voice.setMode(static_cast<int>(modeParam->load()) == 1);
    voice.setMix(mixParam->load());
    voice.setSpread(spreadParam->load());
    voice.setMaster(masterParam->load());

    for (const auto metadata : midiMessages)
    {
        auto msg = metadata.getMessage();
        if (msg.isNoteOn(true))
        {
            currentNote = msg.getNoteNumber();
            voice.noteOn(currentNote);
        }
        else if (msg.isNoteOff(true) && msg.getNoteNumber() == currentNote)
        {
            voice.noteOff();
            currentNote = -1;
        }
        else if (msg.isAllNotesOff() || msg.isAllSoundOff())
        {
            voice.noteOff();
            currentNote = -1;
        }
    }

    auto* leftChannel = buffer.getWritePointer(0);
    auto* rightChannel = buffer.getNumChannels() > 1
        ? buffer.getWritePointer(1) : nullptr;

    for (int i = 0; i < numSamples; ++i)
    {
        auto [left, right] = voice.process();
        leftChannel[i] = left;
        if (rightChannel)
            rightChannel[i] = right;
    }

    for (int ch = 2; ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear(ch, 0, numSamples);
}

bool SalzwiesenProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono())
        return false;
    return true;
}

juce::AudioProcessorEditor* SalzwiesenProcessor::createEditor()
{
    return new SalzwiesenEditor(*this);
}

bool SalzwiesenProcessor::hasEditor() const { return true; }
const juce::String SalzwiesenProcessor::getName() const { return "Salzwiesen"; }
bool SalzwiesenProcessor::acceptsMidi() const { return true; }
bool SalzwiesenProcessor::producesMidi() const { return false; }
bool SalzwiesenProcessor::isMidiEffect() const { return false; }
double SalzwiesenProcessor::getTailLengthSeconds() const { return 0.0; }
int SalzwiesenProcessor::getNumPrograms() { return 1; }
int SalzwiesenProcessor::getCurrentProgram() { return 0; }
void SalzwiesenProcessor::setCurrentProgram(int) {}
const juce::String SalzwiesenProcessor::getProgramName(int) { return {}; }
void SalzwiesenProcessor::changeProgramName(int, const juce::String&) {}

void SalzwiesenProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    if (xml == nullptr) { jassertfalse; return; }
    xml->setAttribute("pluginVersion", 1);
    copyXmlToBinary(*xml, destData);
}

void SalzwiesenProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (data == nullptr || sizeInBytes <= 0) return;
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml == nullptr) { jassertfalse; return; }
    if (!xml->hasTagName(apvts.state.getType())) { jassertfalse; return; }
    auto newState = juce::ValueTree::fromXml(*xml);
    if (!newState.isValid()) { jassertfalse; return; }
    apvts.replaceState(newState);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SalzwiesenProcessor();
}
