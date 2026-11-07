#pragma once
#include "PluginProcessor.h"
class AcidTheme:public juce::LookAndFeel_V4{public:AcidTheme();void drawRotarySlider(juce::Graphics&,int,int,int,int,float,float,float,juce::Slider&)override;};
struct AcidControl:juce::Component{juce::Label label;juce::Slider knob;juce::ComboBox combo;std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sa;std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>ca;AcidControl(juce::AudioProcessorValueTreeState&,acid::Param);void resized()override;};
struct StepGrid:juce::Component{AcidProcessor&processor;explicit StepGrid(AcidProcessor&p):processor(p){}void paint(juce::Graphics&)override;void mouseDown(const juce::MouseEvent&)override;};
struct AcidSurface:juce::Component{void paint(juce::Graphics&)override;private:juce::Image woodTexture;};
class AcidEditor final:public juce::AudioProcessorEditor,private juce::Timer{
public:explicit AcidEditor(AcidProcessor&);~AcidEditor()override;void paint(juce::Graphics&)override;void resized()override;
private:AcidProcessor&processor;AcidTheme theme;juce::Viewport viewport;AcidSurface body;std::vector<std::unique_ptr<AcidControl>> controls;
 StepGrid grid;juce::ComboBox scenes;juce::TextButton previousPatch{"<"},nextPatch{">"},run{"START"},panic{"PANIC"},save{"SAVE PATCH"},load{"LOAD PATCH"},exportMidi{"EXPORT MIDI"},copy{"COPY"},paste{"PASTE"},clear{"CLEAR"},random{"RANDOM"},left{"SHIFT <"},right{"SHIFT >"};
 juce::Label status;juce::TooltipWindow tips{this,600};acid::PatternData clipboard{};bool copied=false;std::unique_ptr<juce::FileChooser>chooser;void timerCallback()override;void exportPattern(const juce::File&);int selectedScene=-1;
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AcidEditor)
};
