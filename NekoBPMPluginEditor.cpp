#include "NekoBPMPluginEditor.h"
#include "NekoBPMPluginProcessor.h"
#include "BinaryData.h" 

void NekoBPMAudioProcessorEditor::loadNekoFrames()
{
    nekoFrames.clear();
    const int numFrames = 158; 
    
    for (int i = 1; i <= numFrames; ++i)
    {
        juce::String resourceName = juce::String::formatted ("neko_%03d_png", i);
        int dataSize;
        const char* data = BinaryData::getNamedResource (resourceName.toRawUTF8(), dataSize);
        
        if (data != nullptr)
        {
            juce::MemoryInputStream is (data, dataSize, false);
            juce::Image image = juce::ImageFileFormat::loadFrom (is);
            
            if (!image.isNull())
                nekoFrames.push_back(std::move(image));
            else
                break; 
        }
        else
        {
            break;
        }
    }
}

NekoBPMAudioProcessorEditor::NekoBPMAudioProcessorEditor (NekoBPMAudioProcessor& owner)
    : AudioProcessorEditor (&owner),
      processor (owner)
{
    loadNekoFrames(); 

    startTimer (16); 

    setSize (250, 350); 
}

NekoBPMAudioProcessorEditor::~NekoBPMAudioProcessorEditor()
{
    stopTimer();
}

void NekoBPMAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB (40, 40, 45)); 

    g.setColour (juce::Colours::white.withAlpha (0.9f));
    g.setFont (juce::Font ("Helvetica", 32.0f, juce::Font::bold));
    g.drawFittedText ("Neko BPM", getLocalBounds().removeFromTop (50).reduced (10, 0), juce::Justification::centred, 2);

    auto bounds = getLocalBounds();
    
    auto imageArea = bounds.withSizeKeepingCentre(220, 220).withY(50);
    
    if (!nekoFrames.empty())
    {
        g.drawImage (nekoFrames[currentFrameIndex], 
                     imageArea.toFloat()); 
    }
    else
    {
        g.setColour (juce::Colours::red);
        g.drawFittedText("Error: Neko GIF not loaded", imageArea, juce::Justification::centred, 1);
    }
    
    auto producerArea = bounds.removeFromBottom(20); 
    g.setColour (juce::Colours::white.withAlpha (0.4f));
    g.setFont (12.0f);
    g.drawFittedText ("Producer : suzuya", producerArea.reduced(10, 0), juce::Justification::centredRight, 1);

    auto bpmArea = getLocalBounds().removeFromBottom(70); 
    bpmArea.removeFromBottom(20); 
    
    juce::String bpmText = "DAW BPM: " + juce::String (processor.currentDAWBPM, 1);
    g.setColour (juce::Colours::cyan);
    g.setFont (juce::Font ("Helvetica", 20.0f, juce::Font::bold));
    g.drawFittedText (bpmText, bpmArea.reduced(10, 0), juce::Justification::centred, 1);
    
    auto playStateArea = getLocalBounds().removeFromBottom(55); 
    juce::String playStateText = processor.isPlaying ? "PLAYING" : "STOPPED";
    g.setColour (processor.isPlaying ? juce::Colours::lime : juce::Colours::orange);
    g.setFont (juce::Font ("Helvetica", 10.0f, juce::Font::bold));
    g.drawFittedText ("State: " + playStateText, playStateArea.reduced(10, 0), juce::Justification::centred, 1);
    
}

void NekoBPMAudioProcessorEditor::resized()
{
}

void NekoBPMAudioProcessorEditor::timerCallback()
{
    if (! processor.isPlaying) 
    {
        repaint(); 
        return; 
    }
    
    float dawBPM = processor.currentDAWBPM;
    
    const float baseBPM = 120.0f;
    
    float speedRatio = juce::jlimit (0.25f, 4.0f, dawBPM / baseBPM); 
    
    float targetFPS = originalNekoFPS * speedRatio;
    
    float targetIntervalMs = 1000.0f / targetFPS;
    targetIntervalMs = juce::jmax (10.0f, targetIntervalMs); 
    
    startTimer ((int)targetIntervalMs); 

    if (!nekoFrames.empty())
    {
        currentFrameIndex = (currentFrameIndex + 1) % nekoFrames.size();
        repaint();
    }
}