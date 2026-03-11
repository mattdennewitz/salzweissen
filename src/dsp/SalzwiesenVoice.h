#pragma once
#include "TriangleOscillator.h"
#include "ImpulseGenerator.h"
#include "SineWaveshaper.h"
#include "ParameterSmoother.h"
#include <cmath>
#include <utility>
#include <algorithm>

class SalzwiesenVoice
{
public:
    struct StereoSample
    {
        float left;
        float right;
    };

    void prepare(double sr)
    {
        if (sr <= 0.0) sr = 44100.0;
        this->sampleRate = sr;
        osc.prepare(sampleRate);
        impulse.prepare(sampleRate);

        pitchSmoother.prepare(sampleRate, 10.0);
        fineSmoother.prepare(sampleRate, 10.0);
        formantSmoother.prepare(sampleRate, 10.0);
        barrelSmoother.prepare(sampleRate, 10.0);
        airSmoother.prepare(sampleRate, 10.0);
        fmSmoother.prepare(sampleRate, 10.0);
        mixSmoother.prepare(sampleRate, 10.0);
        spreadSmoother.prepare(sampleRate, 10.0);
        masterSmoother.prepare(sampleRate, 10.0);

        mixSmoother.reset(1.0f);
        masterSmoother.reset(0.7f);
        airSmoother.reset(0.5f);
        formantSmoother.reset(0.5f);
        barrelSmoother.reset(0.5f);

        prevFormantOut = 0.0f;
        playing = false;

        // Anti-click ramp: ~5ms
        rampSamples = static_cast<int>(sampleRate * 0.005);
        rampInc = 1.0f / static_cast<float>(rampSamples);
        rampGain = 0.0f;
        rampActive = false;
    }

    void noteOn(int midiNote)
    {
        baseFreq = 440.0f * std::pow(2.0f, (midiNote - 69) / 12.0f);
        osc.reset();
        shaper.reset();
        prevFormantOut = 0.0f;
        if (!playing)
            rampGain = 0.0f;
        playing = true;
        rampActive = true;
        releasing = false;
    }

    void noteOff()
    {
        releasing = true;
        rampActive = true;
    }

    void setPitch(float p)    { pitchSmoother.setTarget(p); }
    void setFine(float f)     { fineSmoother.setTarget(f); }
    void setFormant(float f)  { formantSmoother.setTarget(f); }
    void setBarrel(float b)   { barrelSmoother.setTarget(b); }
    void setAir(float a)      { airSmoother.setTarget(a); }
    void setFmIndex(float f)  { fmSmoother.setTarget(f); }
    void setMix(float m)      { mixSmoother.setTarget(m); }
    void setSpread(float s)   { spreadSmoother.setTarget(s); }
    void setMaster(float m)   { masterSmoother.setTarget(m); }
    void setMode(bool constantFormant) { impulse.setConstantFormant(constantFormant); }

    void setPitchAndFine(float pitch, float fine)
    {
        pitchSmoother.setTarget(pitch);
        fineSmoother.setTarget(fine);
    }

    StereoSample process() noexcept
    {
        if (!playing)
            return {0.0f, 0.0f};

        // Anti-click amplitude ramp
        if (rampActive)
        {
            if (releasing)
            {
                rampGain -= rampInc;
                if (rampGain <= 0.0f)
                {
                    rampGain = 0.0f;
                    rampActive = false;
                    releasing = false;
                    playing = false;
                    return {0.0f, 0.0f};
                }
            }
            else
            {
                rampGain += rampInc;
                if (rampGain >= 1.0f)
                {
                    rampGain = 1.0f;
                    rampActive = false;
                }
            }
        }

        float pitch = pitchSmoother.next();
        float formant = formantSmoother.next();
        float barrel = barrelSmoother.next();
        float air = airSmoother.next();
        float fmIndex = fmSmoother.next();
        float mix = mixSmoother.next();
        float spread = spreadSmoother.next();
        float master = masterSmoother.next();

        float pitchOffset = (pitch - 0.5f) * 6.0f;
        float fine = fineSmoother.next();
        float fineOffset = fine * 2.0f;
        float freq = baseFreq * std::pow(2.0f, pitchOffset + fineOffset);

        float fmAmount = fmIndex * prevFormantOut * freq * 0.5f;
        float finalFreq = std::clamp(freq + fmAmount, 20.0f, static_cast<float>(sampleRate) * 0.45f);

        osc.setFrequency(finalFreq);

        auto [square, edge] = osc.process();

        float period = static_cast<float>(sampleRate) / finalFreq;
        impulse.setOscillatorPeriod(period);
        impulse.setDuration(formant);
        impulse.setBarrel(barrel);

        float impulseOut = impulse.process(edge);

        shaper.setAir(air);
        float formantOut = shaper.process(impulseOut);
        prevFormantOut = formantOut;

        float mixed = square * (1.0f - mix) + formantOut * mix;

        float gain = master * rampGain;

        float left, right;
        if (spread < 0.01f)
        {
            left = right = mixed * gain;
        }
        else
        {
            float monoMix = square * (1.0f - mix) + formantOut * mix;
            left = (monoMix * (1.0f - spread) + square * spread) * gain;
            right = (monoMix * (1.0f - spread) + formantOut * spread) * gain;
        }

        return {left, right};
    }

private:
    double sampleRate = 44100.0;
    float baseFreq = 440.0f;
    bool playing = false;
    bool releasing = false;
    float prevFormantOut = 0.0f;

    // Anti-click ramp
    int rampSamples = 220;
    float rampInc = 1.0f / 220.0f;
    float rampGain = 0.0f;
    bool rampActive = false;

    TriangleOscillator osc;
    ImpulseGenerator impulse;
    SineWaveshaper shaper;

    ParameterSmoother pitchSmoother;
    ParameterSmoother fineSmoother;
    ParameterSmoother formantSmoother;
    ParameterSmoother barrelSmoother;
    ParameterSmoother airSmoother;
    ParameterSmoother fmSmoother;
    ParameterSmoother mixSmoother;
    ParameterSmoother spreadSmoother;
    ParameterSmoother masterSmoother;
};
