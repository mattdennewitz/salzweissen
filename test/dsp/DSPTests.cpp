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
