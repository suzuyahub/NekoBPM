#include "NekoBPMPluginProcessor.h"
#include "NekoBPMPluginEditor.h"
#include <cmath> 

NekoBPMAudioProcessor::NekoBPMAudioProcessor()
    #if JucePlugin_Enable_ARA
    : AudioProcessor (BusesProperties().withAnyAudio (true)),
    #else
    : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                      ),
    #endif
    parameters (*this, nullptr, "NekoBPMParameters", createParameterLayout()) 
{
}

NekoBPMAudioProcessor::~NekoBPMAudioProcessor() {}

const juce::String NekoBPMAudioProcessor::getName() const { return JucePlugin_Name; }
bool NekoBPMAudioProcessor::acceptsMidi() const { return false; }
bool NekoBPMAudioProcessor::producesMidi() const { return false; }
bool NekoBPMAudioProcessor::isMidiEffect() const { return false; }
double NekoBPMAudioProcessor::getTailLengthSeconds() const { return 0.0; }

juce::AudioProcessorEditor* NekoBPMAudioProcessor::createEditor()
{
    return new NekoBPMAudioProcessorEditor (*this); 
}

bool NekoBPMAudioProcessor::hasEditor() const { return true; }

bool NekoBPMAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    #if JucePlugin_Build_Standalone
    return true;
    #else
    return layouts.getMainInputChannelSet()  == juce::AudioChannelSet::stereo()
        && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
    #endif
}

void NekoBPMAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (sampleRate, samplesPerBlock);
}

void NekoBPMAudioProcessor::releaseResources() {}

void NekoBPMAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if (auto* playHead = getPlayHead())
    {
        auto positionInfo = playHead->getPosition();
        
        if (positionInfo.hasValue())
        {
            auto bpmValue = positionInfo->getBpm();
            
            if (bpmValue.hasValue())
            {
                currentDAWBPM = (float)*bpmValue;
            }

            isPlaying = positionInfo->getIsPlaying();
        }
    }
    
}

int NekoBPMAudioProcessor::getNumPrograms() { return 1; }
int NekoBPMAudioProcessor::getCurrentProgram() { return 0; }
void NekoBPMAudioProcessor::setCurrentProgram (int index) { juce::ignoreUnused (index); }
const juce::String NekoBPMAudioProcessor::getProgramName (int index) { juce::ignoreUnused (index); return {}; }
void NekoBPMAudioProcessor::changeProgramName (int index, const juce::String& newName) { juce::ignoreUnused (index, newName); }

void NekoBPMAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, true);
    parameters.state.writeToStream(stream);
}

void NekoBPMAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
        parameters.state = tree;
}

juce::AudioProcessorValueTreeState::ParameterLayout NekoBPMAudioProcessor::createParameterLayout()
{
    return {};
}