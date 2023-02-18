/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <array>

//==============================================================================
/**
*/
class FaderVSTAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Label::Listener {
public:
    FaderVSTAudioProcessorEditor (FaderVSTAudioProcessor&);
    ~FaderVSTAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

protected:
    void labelTextChanged(juce::Label*) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FaderVSTAudioProcessor& audioProcessor;

    juce::Slider volumeRange;
    juce::Slider currentVolume;
    juce::TextButton fadeButton;
    juce::Label fadeDownTimeLabel;
    juce::Label fadeUpTimeLabel;

    double fadeDownTime;
    double fadeUpTime;

    /**
     * Whether the audio is faded (or in the process of fading). 
     * 
     * True when the audio is fading downwards, false when it is fading upwards.
     */
    bool faded;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FaderVSTAudioProcessorEditor)
};
