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
FaderVSTAudioProcessor::FaderVSTAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       )
#else
    : AudioProcessor()
#endif
, parameters(*this, nullptr, "FaderVST", {
    std::make_unique<juce::AudioParameterFloat>("gainLow", "Low Gain", 0.0, 1.0, 0.0),
    std::make_unique<juce::AudioParameterFloat>("gainHigh", "High Gain", 0.0, 1.0, 1.0),
    std::make_unique<juce::AudioParameterFloat>("gain", "Gain", 0.0, 1.0, 1.0),
    std::make_unique<juce::AudioParameterBool>("fading", "Is Fading", true),
}) {
    gainLow = parameters.getRawParameterValue("gainLow");
    gainHigh = parameters.getRawParameterValue("gainHigh");
    gain = parameters.getRawParameterValue("gain");
    fading = parameters.getRawParameterValue("fading");
    fadeDuration = 0;
}

FaderVSTAudioProcessor::~FaderVSTAudioProcessor(){
}

const juce::String FaderVSTAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool FaderVSTAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool FaderVSTAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool FaderVSTAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double FaderVSTAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int FaderVSTAudioProcessor::getNumPrograms(){
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FaderVSTAudioProcessor::getCurrentProgram(){
    return 0;
}

void FaderVSTAudioProcessor::setCurrentProgram(int index){
}

const juce::String FaderVSTAudioProcessor::getProgramName(int index){
    return {};
}

void FaderVSTAudioProcessor::changeProgramName(int index, const juce::String& newName){
}

void FaderVSTAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock){
    this->sampleRate = sampleRate;
}

void FaderVSTAudioProcessor::releaseResources(){
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FaderVSTAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
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
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet()){
        return false;
    }
    #endif

    return true;
#endif
}
#endif

void FaderVSTAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages){
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

    if (fadeDuration == 0){
        // When the fade duration is 0, make instant changes
        if (*fading < 0.5){
            // Set the gain to the low point instantly
            *gain = gainLow->load();
        } else {
            // Set the gain to the high point instantly
            *gain = gainHigh->load();
        }

        buffer.applyGain(*gain);

        // Notify the host of the new gain value
        parameters.getParameter("gain")->setValueNotifyingHost(*gain);

        return;
    };

    /** How many samples remain until the fading ends */
    int remaining;
    if (*fading < 0.5){
        // Going down
        remaining = (*gain - *gainLow) / (*gainHigh - *gainLow) * fadeDuration;
    } else {
        // Going up
        remaining = (*gainHigh - *gain) / (*gainHigh - *gainLow) * fadeDuration;
    }

    // If the remaining samples are negative, that means that the end point
    // was moved past the current gain value while the audio was fading.
    // In that case, set the remaining samples to 0 to not crash the
    // processing.
    if (remaining < 0) remaining = 0;

    /** How many samples to process in the current block */
    int samplesToProcess = juce::jmin(remaining, buffer.getNumSamples());
    /** The gain at the end of the block */
    float finalGain;

    float gainStep = (*gainHigh - *gainLow) / fadeDuration * samplesToProcess;
    if (*fading < 0.5){
        // Going down
        finalGain = *gain - gainStep;
    } else {
        // Going up
        finalGain = *gain + gainStep;
    }

    // Apply the gain ramp
    buffer.applyGainRamp(0, samplesToProcess, *gain, finalGain);

    // If any samples remain after the ramp, apply a constant gain
    if (samplesToProcess < buffer.getNumSamples()){
        buffer.applyGain(samplesToProcess, buffer.getNumSamples() - samplesToProcess, finalGain);
        // Since the fading has ended, set the duration to 0 so that next
        // blocks are processed with a constant gain directly
        fadeDuration = 0;
    }

    // Notify the editor that the parameter has changed so it can update the GUI
    parameters.getParameter("gain")->setValueNotifyingHost(finalGain);
    // Also update the gain directly because the above method does not update the value if the difference is too small
    *gain = finalGain;
}

bool FaderVSTAudioProcessor::hasEditor() const {
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FaderVSTAudioProcessor::createEditor(){
    return new FaderVSTAudioProcessorEditor (*this, parameters);
}

void FaderVSTAudioProcessor::getStateInformation (juce::MemoryBlock& destData){
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FaderVSTAudioProcessor::setStateInformation (const void* data, int sizeInBytes){
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){
    return new FaderVSTAudioProcessor();
}
