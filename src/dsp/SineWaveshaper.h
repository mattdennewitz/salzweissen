#pragma once
#include <cmath>
#include <algorithm>

class SineWaveshaper
{
public:
    void setAir(float a) { air = a; }

    float process(float input)
    {
        if (air < 0.001f)
        {
            prevInput = input;
            return 0.0f;
        }

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

        // 2x oversampling: interpolate, shape both, average
        float mid = (prevInput + input) * 0.5f;
        prevInput = input;

        float out0 = shape(mid * vcaGain);
        float out1 = shape(input * vcaGain);

        return (out0 + out1) * 0.5f;
    }

private:
    float shape(float signal) const
    {
        if (air > 0.5f)
        {
            float drive = 1.0f + (air - 0.5f) * 2.0f * 4.0f;
            return std::tanh(drive * std::sin(3.14159265359f * signal * 0.5f));
        }
        return signal;
    }

    float air = 0.5f;
    float prevInput = 0.0f;
};
