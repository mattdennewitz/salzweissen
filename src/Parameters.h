#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

namespace ParamIDs
{
    inline const juce::ParameterID pitch   { "PITCH", 1 };
    inline const juce::ParameterID fine    { "FINE", 1 };
    inline const juce::ParameterID formant { "FORMANT", 1 };
    inline const juce::ParameterID barrel  { "BARREL", 1 };
    inline const juce::ParameterID air     { "AIR", 1 };
    inline const juce::ParameterID fmIndex { "FM_INDEX", 1 };
    inline const juce::ParameterID mode    { "MODE", 1 };
    inline const juce::ParameterID mix     { "MIX", 1 };
    inline const juce::ParameterID spread  { "SPREAD", 1 };
    inline const juce::ParameterID master  { "MASTER", 1 };
}
