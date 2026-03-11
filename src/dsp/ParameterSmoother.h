#pragma once
#include <cmath>

class ParameterSmoother
{
public:
    void prepare(double sampleRate, double timeMs)
    {
        alpha = 1.0f - std::exp(-6.28318530718f
            / static_cast<float>(timeMs * 0.001 * sampleRate));
    }

    void setTarget(float target) { this->target = target; }

    void reset(float value)
    {
        current = value;
        target = value;
    }

    float next()
    {
        current += alpha * (target - current);
        return current;
    }

    float getCurrent() const { return current; }

private:
    float current = 0.0f;
    float target = 0.0f;
    float alpha = 1.0f;
};
