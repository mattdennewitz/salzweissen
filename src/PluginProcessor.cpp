#include "PluginProcessor.h"

MangroveProcessor::MangroveProcessor()
    : AudioProcessor(BusesProperties()
        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    pitchParam   = apvts.getRawParameterValue("PITCH");
    fineParam    = apvts.getRawParameterValue("FINE");
    formantParam = apvts.getRawParameterValue("FORMANT");
    barrelParam  = apvts.getRawParameterValue("BARREL");
    airParam     = apvts.getRawParameterValue("AIR");
    fmIndexParam = apvts.getRawParameterValue("FM_INDEX");
    modeParam    = apvts.getRawParameterValue("MODE");
    mixParam     = apvts.getRawParameterValue("MIX");
    spreadParam  = apvts.getRawParameterValue("SPREAD");
    masterParam  = apvts.getRawParameterValue("MASTER");
}

MangroveProcessor::~MangroveProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout
MangroveProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"PITCH", 1}, "Pitch",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"FINE", 1}, "Fine",
        juce::NormalisableRange<float>(-0.5f, 0.5f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"FORMANT", 1}, "Formant",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"BARREL", 1}, "Barrel",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"AIR", 1}, "Air",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"FM_INDEX", 1}, "FM Index",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{"MODE", 1}, "Mode",
        juce::StringArray{"Wave", "Formant"}, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"MIX", 1}, "Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f), 1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"SPREAD", 1}, "Spread",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"MASTER", 1}, "Master",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f));

    return layout;
}

void MangroveProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    voice.prepare(sampleRate);
    oversampler.initProcessing(static_cast<size_t>(samplesPerBlock));
}

void MangroveProcessor::releaseResources() {}

void MangroveProcessor::processBlock(juce::AudioBuffer<float>& buffer,
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
    voice.setMode(modeParam->load() > 0.5f);
    voice.setMix(mixParam->load());
    voice.setSpread(spreadParam->load());
    voice.setMaster(masterParam->load());

    for (const auto metadata : midiMessages)
    {
        auto msg = metadata.getMessage();
        if (msg.isNoteOn())
        {
            currentNote = msg.getNoteNumber();
            voice.noteOn(currentNote);
        }
        else if (msg.isNoteOff() && msg.getNoteNumber() == currentNote)
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

bool MangroveProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono())
        return false;
    return true;
}

juce::AudioProcessorEditor* MangroveProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

bool MangroveProcessor::hasEditor() const { return true; }
const juce::String MangroveProcessor::getName() const { return "Mangrove"; }
bool MangroveProcessor::acceptsMidi() const { return true; }
bool MangroveProcessor::producesMidi() const { return false; }
bool MangroveProcessor::isMidiEffect() const { return false; }
double MangroveProcessor::getTailLengthSeconds() const { return 0.0; }
int MangroveProcessor::getNumPrograms() { return 1; }
int MangroveProcessor::getCurrentProgram() { return 0; }
void MangroveProcessor::setCurrentProgram(int) {}
const juce::String MangroveProcessor::getProgramName(int) { return {}; }
void MangroveProcessor::changeProgramName(int, const juce::String&) {}

void MangroveProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    if (xml == nullptr) { jassertfalse; return; }
    xml->setAttribute("pluginVersion", 1);
    copyXmlToBinary(*xml, destData);
}

void MangroveProcessor::setStateInformation(const void* data, int sizeInBytes)
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
    return new MangroveProcessor();
}
