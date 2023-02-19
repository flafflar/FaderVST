/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class FaderVSTAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    FaderVSTAudioProcessor();
    ~FaderVSTAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void fade(double seconds){
      // Invert the fading direction
      *fading = 1.0 - *fading;
      fadeDuration = seconds * sampleRate;
    }
    
    void fadeDown(double seconds){
      *fading = 0.0;
      fadeDuration = seconds * sampleRate;
    }

    void fadeUp(double seconds){
      *fading = 1.0;
      fadeDuration = seconds * sampleRate;
    }

    void setGainRange(float low, float high){
      *gainLow = low;
      *gainHigh = high;
    }

private:
    /** The current sample rate, needed to calculate some durations in samples. */
    double sampleRate;

    /** The value of the gain when faded. */
    juce::AudioParameterFloat *gainLow;
    /** The value of the gain when normal. */
    juce::AudioParameterFloat *gainHigh;

    /** 
     * The current value of the gain. 
     */
    juce::AudioParameterFloat *gain;

    /** 
     * Indicates the current state of the fading.
     * 
     * A value of 0.0 means the audio should be fading downwards, and a value of 1.0 
     * means it should be fading upwards.
     * After the fading has ended, the value stays the same, so 0.0 means faded to 
     * the low gain and 1.0 faded to the high gain.
     */
    juce::AudioParameterFloat *fading;

    /** The total duration of the current fade (in samples). */
    int fadeDuration;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FaderVSTAudioProcessor)
};
