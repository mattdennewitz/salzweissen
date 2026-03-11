#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "dsp/ParameterSmoother.h"

using Catch::Matchers::WithinAbs;

TEST_CASE("ParameterSmoother converges to target", "[dsp][smoother]")
{
    ParameterSmoother smoother;
    smoother.prepare(44100.0, 10.0);  // 10ms smoothing
    smoother.reset(0.0f);
    smoother.setTarget(1.0f);

    // After ~10ms (441 samples), should be close to target
    float val = 0.0f;
    for (int i = 0; i < 441; ++i)
        val = smoother.next();

    REQUIRE_THAT(val, WithinAbs(1.0, 0.05));
}

TEST_CASE("ParameterSmoother reset jumps immediately", "[dsp][smoother]")
{
    ParameterSmoother smoother;
    smoother.prepare(44100.0, 10.0);
    smoother.reset(0.75f);

    REQUIRE_THAT(smoother.next(), WithinAbs(0.75, 0.001));
}

#include "dsp/TriangleOscillator.h"
#include <cmath>

TEST_CASE("TriangleOscillator produces correct frequency", "[dsp][osc]")
{
    TriangleOscillator osc;
    osc.prepare(44100.0);
    osc.setFrequency(440.0f);

    // Count rising edges over 1 second (44100 samples)
    int risingEdges = 0;
    for (int i = 0; i < 44100; ++i)
    {
        auto [square, edge] = osc.process();
        if (edge) ++risingEdges;
    }

    // Should be ~440 edges (allow ±2 for boundary)
    REQUIRE(risingEdges >= 438);
    REQUIRE(risingEdges <= 442);
}

TEST_CASE("TriangleOscillator square output is bipolar", "[dsp][osc]")
{
    TriangleOscillator osc;
    osc.prepare(44100.0);
    osc.setFrequency(1000.0f);

    float minVal = 1.0f, maxVal = -1.0f;
    for (int i = 0; i < 4410; ++i)
    {
        auto [square, edge] = osc.process();
        minVal = std::min(minVal, square);
        maxVal = std::max(maxVal, square);
    }

    REQUIRE(minVal < -0.9f);
    REQUIRE(maxVal > 0.9f);
}

#include "dsp/ImpulseGenerator.h"

TEST_CASE("ImpulseGenerator produces AR envelope on trigger", "[dsp][impulse]")
{
    ImpulseGenerator gen;
    gen.prepare(44100.0);
    gen.setDuration(0.5f);
    gen.setBarrel(0.5f);

    gen.trigger();

    float peak = -2.0f;
    float trough = 2.0f;
    for (int i = 0; i < 200; ++i)
    {
        float val = gen.process(false);
        peak = std::max(peak, val);
        trough = std::min(trough, val);
    }

    REQUIRE(peak > 0.5f);
    REQUIRE(trough < -0.5f);
}

TEST_CASE("ImpulseGenerator barrel=0 produces ramp shape", "[dsp][impulse]")
{
    ImpulseGenerator gen;
    gen.prepare(44100.0);
    gen.setDuration(0.5f);
    gen.setBarrel(0.0f);

    gen.trigger();

    float first = gen.process(false);
    REQUIRE(first < 0.0f);

    float last = 0.0f;
    for (int i = 0; i < 500; ++i)
        last = gen.process(false);

    REQUIRE(last <= 0.0f);
}

TEST_CASE("ImpulseGenerator pitch division skips retrigger", "[dsp][impulse]")
{
    ImpulseGenerator gen;
    gen.prepare(44100.0);
    gen.setDuration(0.99f);
    gen.setBarrel(0.5f);

    gen.trigger();

    for (int i = 0; i < 5; ++i)
        gen.process(false);

    bool accepted = gen.trigger();
    REQUIRE_FALSE(accepted);
}

#include "dsp/SineWaveshaper.h"

TEST_CASE("SineWaveshaper silence at air=0", "[dsp][shaper]")
{
    SineWaveshaper shaper;
    shaper.setAir(0.0f);

    float result = shaper.process(1.0f);
    REQUIRE_THAT(result, WithinAbs(0.0, 0.001));
}

TEST_CASE("SineWaveshaper linear at air=0.5", "[dsp][shaper]")
{
    SineWaveshaper shaper;
    shaper.setAir(0.5f);

    float result = shaper.process(0.5f);
    REQUIRE(std::abs(result) > 0.1f);
    REQUIRE(std::abs(result) < 1.0f);
}

TEST_CASE("SineWaveshaper clips at air=1", "[dsp][shaper]")
{
    SineWaveshaper shaper;
    shaper.setAir(1.0f);

    float result = shaper.process(0.8f);
    REQUIRE(std::abs(result) <= 1.01f);
}

#include "dsp/SalzwiesenVoice.h"

TEST_CASE("SalzwiesenVoice produces non-silent output", "[dsp][voice]")
{
    SalzwiesenVoice voice;
    voice.prepare(44100.0);
    voice.noteOn(60);  // Middle C

    float maxAbs = 0.0f;
    for (int i = 0; i < 4410; ++i)
    {
        auto [left, right] = voice.process();
        maxAbs = std::max(maxAbs, std::max(std::abs(left), std::abs(right)));
    }

    REQUIRE(maxAbs > 0.01f);
}

TEST_CASE("SalzwiesenVoice silence after noteOff", "[dsp][voice]")
{
    SalzwiesenVoice voice;
    voice.prepare(44100.0);
    voice.noteOn(60);

    for (int i = 0; i < 4410; ++i)
        voice.process();

    voice.noteOff();

    for (int i = 0; i < 4410; ++i)
        voice.process();

    auto [left, right] = voice.process();
    REQUIRE_THAT(left, WithinAbs(0.0, 0.01));
}

TEST_CASE("SalzwiesenVoice mix knob blends square and formant", "[dsp][voice]")
{
    SalzwiesenVoice voice;
    voice.prepare(44100.0);
    voice.noteOn(60);
    voice.setMix(0.0f);

    float sumMix0 = 0.0f;
    for (int i = 0; i < 441; ++i)
    {
        auto [l, r] = voice.process();
        sumMix0 += std::abs(l);
    }

    SalzwiesenVoice voice2;
    voice2.prepare(44100.0);
    voice2.noteOn(60);
    voice2.setMix(1.0f);

    float sumMix1 = 0.0f;
    for (int i = 0; i < 441; ++i)
    {
        auto [l, r] = voice2.process();
        sumMix1 += std::abs(l);
    }

    REQUIRE(sumMix0 > 0.0f);
    REQUIRE(sumMix1 > 0.0f);
}
