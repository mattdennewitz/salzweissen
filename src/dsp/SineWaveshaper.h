#pragma once
#include <cmath>
#include <algorithm>

class SineWaveshaper
{
public:
    void setAir(float a) { air = a; }

    float process(float input)
    {
        if (air < 0.001f) return 0.0f;

        float vcaGain;
        if (air <= 0.5f)
        {
            float t = air * 2.0f;
            float dB = -60.0f + t * 60.0f;
            vcaGain = std::pow(10.0f, dB / 20.0f);
        }
        else
        {
            vcaGain = 1.0f;
        }

        float signal = input * vcaGain;

        if (air > 0.5f)
        {
            float drive = 1.0f + (air - 0.5f) * 2.0f * 4.0f;
            signal = std::tanh(drive * std::sin(3.14159265359f * signal * 0.5f));
        }

        return signal;
    }

private:
    float air = 0.5f;
};
