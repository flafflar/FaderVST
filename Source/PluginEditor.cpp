/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FaderVSTAudioProcessorEditor::FaderVSTAudioProcessorEditor (FaderVSTAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p) {
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    // Configure the volume range slider
    volumeRange.setSliderStyle(juce::Slider::TwoValueHorizontal);
    volumeRange.setRange(0.0, 1.0, 0.01);
    volumeRange.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    volumeRange.setMinAndMaxValues(0.0, 1.0);

    // Configure the current volume slider
    currentVolume.setSliderStyle(juce::Slider::LinearHorizontal);
    currentVolume.setRange(0.0, 1.0, 0.01);
    currentVolume.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    currentVolume.setValue(1.0);

    // Add the sliders to the window
    addAndMakeVisible(volumeRange);
    addAndMakeVisible(currentVolume);
}

FaderVSTAudioProcessorEditor::~FaderVSTAudioProcessorEditor(){
}

//==============================================================================
void FaderVSTAudioProcessorEditor::paint (juce::Graphics& g){
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void FaderVSTAudioProcessorEditor::resized(){
    // Set the position of the range slider
    volumeRange.setBounds(40, 0, getWidth() - 80, 60);

    // Set the position of the current volume slider
    currentVolume.setBounds(40, 60, getWidth() - 80, 90);
}
