#pragma once
#include <cmath>
#include <utility>

class TriangleOscillator
{
public:
    struct Output
    {
        float square;
        bool risingEdge;
    };

    void prepare(double sampleRate)
    {
        this->sampleRate = static_cast<float>(sampleRate);
        phase = 0.0f;
        prevSquare = -1.0f;
    }

    void setFrequency(float hz)
    {
        freq = hz;
        phaseInc = freq / sampleRate;
    }

    void reset()
    {
        phase = 0.0f;
        prevSquare = -1.0f;
    }

    // Returns anti-aliased square wave + rising edge flag
    Output process()
    {
        // Advance phase [0, 1)
        phase += phaseInc;
        if (phase >= 1.0f) phase -= 1.0f;

        // Naive square
        float square = phase < 0.5f ? 1.0f : -1.0f;

        // PolyBLEP anti-aliasing
        square += polyBlep(phase, phaseInc);
        square -= polyBlep(fmod(phase + 0.5f, 1.0f), phaseInc);

        // Detect rising edge (transition from negative to positive)
        bool edge = (prevSquare <= 0.0f && square > 0.0f);
        prevSquare = square;

        return {square, edge};
    }

    float getPhase() const { return phase; }
    float getFrequency() const { return freq; }

private:
    static float polyBlep(float t, float dt)
    {
        if (t < dt)
        {
            t /= dt;
            return t + t - t * t - 1.0f;
        }
        if (t > 1.0f - dt)
        {
            t = (t - 1.0f) / dt;
            return t * t + t + t + 1.0f;
        }
        return 0.0f;
    }

    float sampleRate = 44100.0f;
    float freq = 440.0f;
    float phase = 0.0f;
    float phaseInc = 0.0f;
    float prevSquare = -1.0f;
};
