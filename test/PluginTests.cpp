#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "PluginProcessor.h"

TEST_CASE("Processor creates without crashing", "[processor]")
{
    SalzwiesenProcessor proc;
    REQUIRE(proc.getName() == "Salzwiesen");
}

TEST_CASE("Processor has correct parameter count", "[processor]")
{
    SalzwiesenProcessor proc;
    REQUIRE(proc.apvts.getParameter("PITCH") != nullptr);
    REQUIRE(proc.apvts.getParameter("FINE") != nullptr);
    REQUIRE(proc.apvts.getParameter("FORMANT") != nullptr);
    REQUIRE(proc.apvts.getParameter("BARREL") != nullptr);
    REQUIRE(proc.apvts.getParameter("AIR") != nullptr);
    REQUIRE(proc.apvts.getParameter("FM_INDEX") != nullptr);
    REQUIRE(proc.apvts.getParameter("MODE") != nullptr);
    REQUIRE(proc.apvts.getParameter("MIX") != nullptr);
    REQUIRE(proc.apvts.getParameter("SPREAD") != nullptr);
    REQUIRE(proc.apvts.getParameter("MASTER") != nullptr);
}

TEST_CASE("Processor produces output with MIDI", "[processor]")
{
    SalzwiesenProcessor proc;
    proc.prepareToPlay(44100.0, 512);

    juce::AudioBuffer<float> buffer(2, 512);
    buffer.clear();

    juce::MidiBuffer midi;
    midi.addEvent(juce::MidiMessage::noteOn(1, 60, (juce::uint8)100), 0);

    proc.processBlock(buffer, midi);

    float maxAbs = 0.0f;
    for (int ch = 0; ch < 2; ++ch)
        for (int i = 0; i < 512; ++i)
            maxAbs = std::max(maxAbs, std::abs(buffer.getSample(ch, i)));

    REQUIRE(maxAbs > 0.001f);
}

TEST_CASE("State persistence round-trip", "[processor]")
{
    SalzwiesenProcessor proc;
    proc.prepareToPlay(44100.0, 512);

    auto* param = proc.apvts.getParameter("FORMANT");
    param->setValueNotifyingHost(0.75f);

    juce::MemoryBlock stateData;
    proc.getStateInformation(stateData);

    SalzwiesenProcessor proc2;
    proc2.setStateInformation(stateData.getData(), static_cast<int>(stateData.getSize()));

    float restored = proc2.apvts.getParameter("FORMANT")->getValue();
    REQUIRE_THAT(static_cast<double>(restored), Catch::Matchers::WithinAbs(0.75, 0.01));
}
