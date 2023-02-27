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
    FaderVSTAudioProcessorEditor (FaderVSTAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~FaderVSTAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

protected:
    void labelTextChanged(juce::Label*) override;

    bool keyPressed(const juce::KeyPress&) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FaderVSTAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState &tree;

    juce::Slider volumeRange;
    juce::Slider currentVolume;
    juce::TextButton fadeButton;
    juce::Label fadeDownTimeLabel;
    juce::Label fadeUpTimeLabel;

    std::unique_ptr<juce::ParameterAttachment> currentVolumeAttachment;

    double fadeDownTime;
    double fadeUpTime;

    /**
     * Whether the audio is faded (or in the process of fading). 
     * 
     * True when the audio is fading downwards, false when it is fading upwards.
     */
    bool faded;

    void fade();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FaderVSTAudioProcessorEditor)
};
