cmake_minimum_required(VERSION 3.22)
project(Salzwiesen VERSION 1.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(lib/JUCE)

juce_add_plugin(Salzwiesen
    COMPANY_NAME "Die stille Erde"
    PLUGIN_MANUFACTURER_CODE DsEr
    PLUGIN_CODE Slzw
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Salzwiesen"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    AU_MAIN_TYPE kAudioUnitType_MusicDevice
    VST3_CATEGORIES Instrument Synth
)

target_sources(Salzwiesen PRIVATE
    src/PluginProcessor.cpp
    src/PluginEditor.cpp
    src/ui/SalzwiesenLookAndFeel.cpp
)

target_include_directories(Salzwiesen PRIVATE src)

target_compile_definitions(Salzwiesen PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    DONT_SET_USING_JUCE_NAMESPACE=1
)

target_link_libraries(Salzwiesen
    PRIVATE
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_gui_basics
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# --- Tests ---
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.7.1
)
FetchContent_MakeAvailable(Catch2)

add_executable(SalzwiesenTests
    test/dsp/DSPTests.cpp
    test/PluginTests.cpp
    src/PluginProcessor.cpp
    src/PluginEditor.cpp
    src/ui/SalzwiesenLookAndFeel.cpp
)

target_include_directories(SalzwiesenTests PRIVATE src)

target_link_libraries(SalzwiesenTests PRIVATE
    Catch2::Catch2WithMain
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_gui_basics
)

target_compile_definitions(SalzwiesenTests PRIVATE
    $<TARGET_PROPERTY:Salzwiesen,COMPILE_DEFINITIONS>
    JUCE_STANDALONE_APPLICATION=0
    JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP=0
)

include(CTest)
add_test(NAME SalzwiesenTests COMMAND SalzwiesenTests)
