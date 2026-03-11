#pragma once
#include <cmath>
#include <algorithm>

class ImpulseGenerator
{
public:
    void prepare(double sampleRate)
    {
        this->sampleRate = static_cast<float>(sampleRate);
        phase = 0.0f;
        active = false;
        output = -1.0f;
    }

    void setDuration(float d) { duration = d; }
    void setBarrel(float b) { barrel = b; }
    void setOscillatorPeriod(float periodSamples) { oscPeriod = periodSamples; }
    void setConstantFormant(bool cf) { constantFormant = cf; }

    bool trigger()
    {
        if (active) return false;
        phase = 0.0f;
        active = true;
        return true;
    }

    float process(bool risingEdge)
    {
        if (risingEdge && active)
        {
            // Skip retrigger during active impulse (pitch division)
        }
        else if (risingEdge && !active)
        {
            trigger();
        }

        if (!active)
        {
            output = -1.0f;
            return output;
        }

        float basePeriod = constantFormant
            ? sampleRate / 100.0f
            : oscPeriod;

        float minLen = basePeriod * 0.05f;
        float maxLen = basePeriod * 3.0f;
        float totalLen = maxLen - duration * (maxLen - minLen);
        totalLen = std::max(totalLen, 4.0f);

        float riseRatio = 1.0f - barrel;
        float riseSamples = totalLen * std::max(riseRatio, 0.01f);
        float fallSamples = totalLen * std::max(1.0f - riseRatio, 0.01f);

        if (phase < riseSamples)
        {
            float t = phase / riseSamples;
            output = -1.0f + 2.0f * t;
        }
        else if (phase < riseSamples + fallSamples)
        {
            float t = (phase - riseSamples) / fallSamples;
            output = 1.0f - 2.0f * t;
        }
        else
        {
            output = -1.0f;
            active = false;
        }

        phase += 1.0f;
        return output;
    }

    bool isActive() const { return active; }

private:
    float sampleRate = 44100.0f;
    float duration = 0.5f;
    float barrel = 0.5f;
    float oscPeriod = 100.0f;
    float phase = 0.0f;
    float output = -1.0f;
    bool active = false;
    bool constantFormant = false;
};
