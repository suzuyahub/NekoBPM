#pragma once

#include <JuceHeader.h>
#include "NekoBPMPluginProcessor.h" 

class NekoBPMAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                           public juce::Timer 
{
public:
    NekoBPMAudioProcessorEditor(NekoBPMAudioProcessor&);
    ~NekoBPMAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void timerCallback() override; 

private:
    void loadNekoFrames(); 
    
    std::vector<juce::Image> nekoFrames; 
    int currentFrameIndex = 0;           
    
    const float originalNekoFPS = 27.95f; 

    NekoBPMAudioProcessor& processor; 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NekoBPMAudioProcessorEditor)
};