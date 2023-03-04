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

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

/**
 * Attaches a two value slider to two parameters, one for the min and one for
 * the max value.
 */
class TwoValueSliderAttachment : private juce::Slider::Listener {
public:
	TwoValueSliderAttachment(
		juce::Slider &slider,
		juce::RangedAudioParameter &minParam,
		juce::RangedAudioParameter &maxParam
	) : slider(slider),
		minAttachment(minParam, [this](float v){
			this->slider.setMinValue(v, juce::sendNotificationSync);
		}),
		maxAttachment(maxParam, [this](float v){
			this->slider.setMaxValue(v, juce::sendNotificationSync);
		})
	{
		slider.addListener(this);
	}

	~TwoValueSliderAttachment(){
		slider.removeListener(this);
	}

private:
	juce::Slider &slider;

	juce::ParameterAttachment minAttachment;
	juce::ParameterAttachment maxAttachment;

	void sliderValueChanged(juce::Slider *slider) override {
		minAttachment.setValueAsPartOfGesture(slider->getMinValue());
		maxAttachment.setValueAsPartOfGesture(slider->getMaxValue());
	}

	void sliderDragStarted(juce::Slider *slider) override {
		minAttachment.beginGesture();
		maxAttachment.beginGesture();
	}

	void sliderDragEnded(juce::Slider *slider) override {
		minAttachment.endGesture();
		maxAttachment.endGesture();
	}
};