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

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FaderVSTAudioProcessorEditor::FaderVSTAudioProcessorEditor (FaderVSTAudioProcessor& p, juce::AudioProcessorValueTreeState &tree)
    : AudioProcessorEditor (&p), audioProcessor (p), tree(tree),
    volumeRangeAttachment(volumeRange, *tree.getParameter("gainLow"), *tree.getParameter("gainHigh")),
    volumeRangeLowInputAttachment(volumeRangeLowInput, *tree.getParameter("gainLow")),
    volumeRangeHighInputAttachment(volumeRangeHighInput, *tree.getParameter("gainHigh")),
    currentVolumeInputAttachment(currentVolumeInput, *tree.getParameter("gain"))
    {

    faded = false;
    fadeDownTime = 1.0;
    fadeUpTime = 1.0;

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (420, 370);

    // Configure the volume range slider
    volumeRange.setSliderStyle(juce::Slider::TwoValueHorizontal);
    volumeRange.setRange(0.0, 1.0, 0.01);
    volumeRange.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    volumeRange.setMinAndMaxValues(0.0, 1.0);


    // Configure the volume range low point input and its label
    volumeRangeLowInput.setText("0.00", juce::dontSendNotification);
    volumeRangeLowInput.setFont(inputFont);
    volumeRangeLowInput.setEditable(true);
    addAndMakeVisible(volumeRangeLowInput);

    volumeRangeLowLabel.setText("Low gain", juce::dontSendNotification);
    volumeRangeLowLabel.setFont(labelFont);
    volumeRangeLowLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(volumeRangeLowLabel);


    // Configure the volume range high point input and its label
    volumeRangeHighInput.setText("1.00", juce::dontSendNotification);
    volumeRangeHighInput.setFont(inputFont);
    volumeRangeHighInput.setEditable(true);
    addAndMakeVisible(volumeRangeHighInput);

    volumeRangeHighLabel.setText("High gain", juce::dontSendNotification);
    volumeRangeHighLabel.setFont(labelFont);
    volumeRangeHighLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(volumeRangeHighLabel);


    // Configure the current volume slider
    currentVolume.setSliderStyle(juce::Slider::LinearHorizontal);
    currentVolume.setRange(0.0, 1.0, 0.01);
    currentVolume.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    currentVolume.setValue(1.0);
    currentVolume.onValueChange = [this](){
        if (this->unlockCurrentVolume.getToggleState()){
            if (currentVolume.getThumbBeingDragged() == 0){
                this->audioProcessor.stopFading();
                this->tree.getParameter("gain")->setValueNotifyingHost(this->currentVolume.getValue());
            }
        }
    };

    currentVolumeLabel.setText("Current gain", juce::dontSendNotification);
    currentVolumeLabel.setFont(labelFont);
    currentVolumeLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(currentVolumeLabel);

    currentVolumeInput.setText("1.0", juce::dontSendNotification);
    currentVolumeInput.setFont(inputFont);
    currentVolumeInput.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(currentVolumeInput);

    // Configure the current volume slider unlock checkbox
    addAndMakeVisible(unlockCurrentVolume);

    unlockCurrentVolumeLabel.setText("Unlock gain slider", juce::dontSendNotification);
    unlockCurrentVolumeLabel.setFont(labelFont);
    unlockCurrentVolumeLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(unlockCurrentVolumeLabel);

    // Create the attachment to the gain parameter
    currentVolumeAttachment.reset(new juce::ParameterAttachment(*tree.getParameter("gain"), [this](float value){
      currentVolume.setValue(value, juce::sendNotificationSync);
    }));

    // Configure the fade button
    fadeButton.setButtonText("Fade");
    fadeButton.onClick = std::bind(&FaderVSTAudioProcessorEditor::fade, this);

    // Configure the fade times textboxes
    fadeDownTimeInput.setEditable(true);
    fadeDownTimeInput.setJustificationType(juce::Justification::centredRight);
    fadeDownTimeInput.setText("1.0", juce::dontSendNotification);
    fadeDownTimeInput.setFont(inputFont);
    fadeDownTimeInput.addListener(this);

    fadeDownTimeLabel.setText("Fade down time", juce::dontSendNotification);
    fadeDownTimeLabel.setFont(labelFont);
    fadeDownTimeLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(fadeDownTimeLabel);

    fadeUpTimeInput.setEditable(true);
    fadeUpTimeInput.setJustificationType(juce::Justification::centredRight);
    fadeUpTimeInput.setText("1.0", juce::dontSendNotification);
    fadeUpTimeInput.setFont(inputFont);
    fadeUpTimeInput.addListener(this);

    fadeUpTimeLabel.setText("Fade up time", juce::dontSendNotification);
    fadeUpTimeLabel.setFont(labelFont);
    fadeUpTimeLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(fadeUpTimeLabel);

    // Configure the keyboard shortcut buttons
    addAndMakeVisible(enableKeyboardShortcut);

    enableKeyboardShortcutLabel.setText("Enable keyboard shortcut", juce::dontSendNotification);
    enableKeyboardShortcutLabel.setFont(labelFont);
    enableKeyboardShortcutLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(enableKeyboardShortcutLabel);

    keyboardShortcutButton.setButtonText("Set shortcut");
    keyboardShortcutButton.onClick = [this](){
        keyboardShortcutState = KeyboardShortcutState::Registering;
        keyboardShortcutButton.setButtonText("Press a key...");
    };
    addAndMakeVisible(keyboardShortcutButton);


    // Add the sliders to the window
    addAndMakeVisible(volumeRange);
    addAndMakeVisible(currentVolume);

    // Add the button to the window
    addAndMakeVisible(fadeButton);

    // Add the fade time textboxes to the window
    addAndMakeVisible(fadeDownTimeInput);
    addAndMakeVisible(fadeUpTimeInput);
}

FaderVSTAudioProcessorEditor::~FaderVSTAudioProcessorEditor() {
}

//==============================================================================
void FaderVSTAudioProcessorEditor::paint (juce::Graphics& g){
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void FaderVSTAudioProcessorEditor::resized(){
    // TODO: Make this design responsive

    volumeRangeLowLabel.setBounds(42, 25, 69, 18);
    volumeRangeLowInput.setBounds(111, 23, 40, 22);
    volumeRangeHighLabel.setBounds(244, 25, 74, 18);
    volumeRangeHighInput.setBounds(318, 23, 40, 22);

    // Set the position of the range slider
    volumeRange.setBounds(42, 72, 316, 24);

    // Set the position of the current volume slider
    currentVolume.setBounds(42, 150, 316, 24);

    currentVolumeLabel.setBounds(42, 109, 91, 18);
    currentVolumeInput.setBounds(134, 107, 40, 22);

    unlockCurrentVolume.setBounds(232, 110, 136, 18);
    unlockCurrentVolumeLabel.setBounds(258, 108, 110, 18);

    // Set the positions of the fade time textboxes
    fadeDownTimeLabel.setBounds(43, 186, 100, 18);
    fadeDownTimeInput.setBounds(143, 184, 45, 22);
    fadeUpTimeLabel.setBounds(214, 186, 100, 18);
    fadeUpTimeInput.setBounds(314, 184, 45, 22);

    enableKeyboardShortcut.setBounds(32, 230, 200, 18);
    enableKeyboardShortcutLabel.setBounds(62, 230, 170, 18);

    keyboardShortcutButton.setBounds(232, 230, 152, 18);

    // Set the position of the fade button
    fadeButton.setBounds(32, 300, 356, 36);
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
    if (label == &fadeDownTimeInput){
      fadeDownTime = value;
    } else if (label == &fadeUpTimeInput){
      fadeUpTime = value;
    }
}

bool FaderVSTAudioProcessorEditor::keyPressed(const juce::KeyPress &key){
    switch (keyboardShortcutState) {
        case KeyboardShortcutState::Registering:
            keyboardShortcut = new juce::KeyPress(key);
            keyboardShortcutButton.setButtonText(keyboardShortcut->getTextDescription());
            keyboardShortcutState = KeyboardShortcutState::Listening;
            break;

        case KeyboardShortcutState::Listening:
            if (enableKeyboardShortcut.getToggleState() && keyboardShortcut){
                if (key == *keyboardShortcut){
                    fade();
                }
            }
            break;
    }

    return false;
}