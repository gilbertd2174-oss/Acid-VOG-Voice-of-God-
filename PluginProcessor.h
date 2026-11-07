#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "AcidEngine.h"
class AcidProcessor final:public juce::AudioProcessor{
public:
 AcidProcessor();void prepareToPlay(double,int)override;void releaseResources()override{};bool isBusesLayoutSupported(const BusesLayout&)const override;
 void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&)override;juce::AudioProcessorEditor* createEditor()override;bool hasEditor()const override{return true;}
 const juce::String getName()const override{return "Acid VOG";}bool acceptsMidi()const override{return true;}bool producesMidi()const override{return false;}double getTailLengthSeconds()const override{return 20;}
 int getNumPrograms()override{return 256;}int getCurrentProgram()override{return program.load();}void setCurrentProgram(int)override;const juce::String getProgramName(int i)override{return i>=0&&i<256?juce::String::fromUTF8(acid::scenes[(size_t)i].name):juce::String();}void changeProgramName(int,const juce::String&)override{}
 void getStateInformation(juce::MemoryBlock&)override;void setStateInformation(const void*,int)override;
 juce::AudioProcessorValueTreeState state;std::atomic<bool> running{false},panic{false};std::atomic<int> playStep{-1};std::atomic<float> peak{0},bpmDisplay{128};std::atomic<uint64_t> protectionTrips{0};
 acid::PatternData getPattern(int)const;void putPattern(int,const acid::PatternData&);int selectedPattern()const;void setStep(int,int,const acid::Step&);
private:
 std::array<std::atomic<float>*,acid::Count> raw{};std::array<std::array<std::atomic<uint32_t>,16>,16> patterns{};
 acid::Engine engine;double sr=48000,localPpq=0;int64_t lastSerial=INT64_MIN;bool wasPlaying=false;int oldMode=0,oldPattern=-1;std::array<float,128> keys{};std::atomic<int> program{0};
 acid::Values values()const;static juce::AudioProcessorValueTreeState::ParameterLayout layout();
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AcidProcessor)
};
