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
