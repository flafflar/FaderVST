/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FaderVSTAudioProcessor::FaderVSTAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter(gainLow = new juce::AudioParameterFloat("gainLow", "Low Gain", 0.0, 1.0, 0.0));
    addParameter(gainHigh = new juce::AudioParameterFloat("gainHigh", "High Gain", 0.0, 1.0, 1.0));
    addParameter(faded = new juce::AudioParameterBool("faded", "Is Fading", false));
    gain = 1.0;
    fadeDuration = 0;
}

FaderVSTAudioProcessor::~FaderVSTAudioProcessor(){
}

//==============================================================================
const juce::String FaderVSTAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FaderVSTAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FaderVSTAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FaderVSTAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FaderVSTAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FaderVSTAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FaderVSTAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FaderVSTAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FaderVSTAudioProcessor::getProgramName (int index)
{
    return {};
}

void FaderVSTAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FaderVSTAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    this->sampleRate = sampleRate;
}

void FaderVSTAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FaderVSTAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void FaderVSTAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if (fadeDuration == 0) return;
    
    /** How many samples remain until the fading ends */
    int remaining;
    if (faded->get()){
        // Going down
        remaining = (gain - gainLow->get()) / (gainHigh->get() - gainLow->get()) * fadeDuration;
    } else {
        // Going up
        remaining = (gainHigh->get() - gain) / (gainHigh->get() - gainLow->get()) * fadeDuration;
    }

    /** How many samples to process in the current block */
    int samplesToProcess = juce::jmin(remaining, buffer.getNumSamples());
    /** The gain at the end of the block */
    float finalGain;

    float gainStep = (gainHigh->get() - gainLow->get()) / fadeDuration * samplesToProcess;
    if (faded->get()){
        // Going down
        finalGain = gain - gainStep;
    } else {
        // Going up
        finalGain = gain + gainStep;
    }

    // Apply the gain ramp
    buffer.applyGainRamp(0, buffer.getNumSamples(), gain, finalGain);

    // If any samples remain after the ramp, apply a constant gain
    if (samplesToProcess < buffer.getNumSamples()){
        buffer.applyGain(samplesToProcess, buffer.getNumSamples(), finalGain);
    }

    gain = finalGain;
}

//==============================================================================
bool FaderVSTAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FaderVSTAudioProcessor::createEditor()
{
    return new FaderVSTAudioProcessorEditor (*this);
}

//==============================================================================
void FaderVSTAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FaderVSTAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FaderVSTAudioProcessor();
}
