// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2023 Achilleas Michailidis <achmichail@gmail.com>
 * 
 * This file is part of FaderVST.
 * 
 * FaderVST is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
 * details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>. 
 */

/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FaderVSTAudioProcessorEditor::FaderVSTAudioProcessorEditor (FaderVSTAudioProcessor& p, juce::AudioProcessorValueTreeState &tree)
    : AudioProcessorEditor (&p), audioProcessor (p), tree(tree) {
    
    faded = false;
    fadeDownTime = 1.0;
    fadeUpTime = 1.0;

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    // Configure the volume range slider
    volumeRange.setSliderStyle(juce::Slider::TwoValueHorizontal);
    volumeRange.setRange(0.0, 1.0, 0.01);
    volumeRange.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    volumeRange.setMinAndMaxValues(0.0, 1.0);
    volumeRange.onValueChange = [this](){
      this->audioProcessor.setGainRange(this->volumeRange.getMinValue(), this->volumeRange.getMaxValue());
    };

    // Configure the current volume slider
    currentVolume.setSliderStyle(juce::Slider::LinearHorizontal);
    currentVolume.setRange(0.0, 1.0, 0.01);
    currentVolume.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    currentVolume.setValue(1.0);

    // Create the attachment to the gain parameter
    currentVolumeAttachment.reset(new juce::ParameterAttachment(*tree.getParameter("gain"), [this](float value){
      currentVolume.setValue(value, juce::sendNotificationSync);
    }));

    // Configure the fade button
    fadeButton.setButtonText("Fade");
    fadeButton.onClick = std::bind(&FaderVSTAudioProcessorEditor::fade, this);

    // Configure the fade times textboxes
    fadeDownTimeLabel.setEditable(true);
    fadeDownTimeLabel.setJustificationType(juce::Justification::centred);
    fadeDownTimeLabel.setText("1.0", juce::dontSendNotification);
    fadeDownTimeLabel.addListener(this);

    fadeUpTimeLabel.setEditable(true);
    fadeUpTimeLabel.setJustificationType(juce::Justification::centred);
    fadeUpTimeLabel.setText("1.0", juce::dontSendNotification);
    fadeUpTimeLabel.addListener(this);

    // Add the sliders to the window
    addAndMakeVisible(volumeRange);
    addAndMakeVisible(currentVolume);

    // Add the button to the window
    addAndMakeVisible(fadeButton);

    // Add the fade time textboxes to the window
    addAndMakeVisible(fadeDownTimeLabel);
    addAndMakeVisible(fadeUpTimeLabel);
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

    // Set the positions of the fade time textboxes
    fadeDownTimeLabel.setBounds(40, 120, (getWidth() - 80) / 2, 20);
    fadeUpTimeLabel.setBounds(fadeDownTimeLabel.getX() + fadeDownTimeLabel.getWidth(), 120, fadeDownTimeLabel.getWidth(), 20);

    // Set the position of the fade button
    fadeButton.setBounds(40, 160, getWidth() - 80, 40);
}

void FaderVSTAudioProcessorEditor::fade(){
    if (this->faded){
        faded = false;
        this->audioProcessor.fadeUp(this->fadeUpTime);
    } else {
        faded = true;
        this->audioProcessor.fadeDown(this->fadeDownTime);
    }
}

void FaderVSTAudioProcessorEditor::labelTextChanged(juce::Label *label){
    juce::String text = label->getText();
    double value = text.getDoubleValue();
    if (label == &fadeDownTimeLabel){
      fadeDownTime = value;
    } else if (label == &fadeUpTimeLabel) {
      fadeUpTime = value;
    }
}

bool FaderVSTAudioProcessorEditor::keyPressed(const juce::KeyPress &key){
    if (key.getTextCharacter() == ' '){
        fade();
    }
    return false;
}