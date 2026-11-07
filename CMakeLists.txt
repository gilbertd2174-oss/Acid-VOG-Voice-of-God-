cmake_minimum_required(VERSION 3.24)
project(AcidVOG VERSION 1.0.8 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(CTest)
if(MSVC)
 add_compile_options(/bigobj /utf-8)
endif()
if(BUILD_TESTING)
 add_executable(DavoeEngineTests Tests/EngineTests.cpp)
 target_include_directories(DavoeEngineTests PRIVATE Source)
 add_test(NAME SynthEngine COMMAND DavoeEngineTests)
 add_executable(AcidStabilityTests Tests/StabilityTests.cpp)
 target_include_directories(AcidStabilityTests PRIVATE Source)
 add_test(NAME AcidLongRun COMMAND AcidStabilityTests)
 set_tests_properties(AcidLongRun PROPERTIES TIMEOUT 900)
endif()
option(DAVOE_ENGINE_ONLY "Build engine tests without JUCE" OFF)
if(DAVOE_ENGINE_ONLY)
 return()
endif()
include(FetchContent)
FetchContent_Declare(JUCE GIT_REPOSITORY https://github.com/juce-framework/JUCE.git GIT_TAG 8.0.6 GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)
juce_add_plugin(AcidVOG
 COMPANY_NAME "Davoe Audio"
 BUNDLE_ID "audio.davoe.acid-vog"
 PLUGIN_MANUFACTURER_CODE Dvoe
 PLUGIN_CODE AcV1
 FORMATS VST3 Standalone
 PRODUCT_NAME "Acid VOG"
 DESCRIPTION "Acid bass synthesizer and step sequencer"
 IS_SYNTH TRUE NEEDS_MIDI_INPUT TRUE NEEDS_MIDI_OUTPUT FALSE
 IS_MIDI_EFFECT FALSE EDITOR_WANTS_KEYBOARD_FOCUS TRUE
 COPY_PLUGIN_AFTER_BUILD FALSE
 VST3_CATEGORIES Instrument Synth)
target_sources(AcidVOG PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp)
target_compile_definitions(AcidVOG PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0 JUCE_DISPLAY_SPLASH_SCREEN=0)
target_link_libraries(AcidVOG PRIVATE juce::juce_audio_utils PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_warning_flags)
