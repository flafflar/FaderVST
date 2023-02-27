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
