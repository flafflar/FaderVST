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
 * Attaches a label to display (and possibly edit) the value of a parameter.
 */
class LabelAttachment : private juce::Label::Listener {
public:
	LabelAttachment(juce::Label &label, juce::RangedAudioParameter &param)
	: label(label), attachment(param, [this](float v){ this->setLabel(v); })
	{
		label.addListener(this);
	}

	~LabelAttachment(){
		label.removeListener(this);
	}

private:
	juce::Label &label;

	juce::ParameterAttachment attachment;

	void setLabel(float value){
		label.setText(juce::String(value), juce::dontSendNotification);
	}

	void labelTextChanged(juce::Label *label) override {
		float value = label->getText().getFloatValue();
		attachment.setValueAsCompleteGesture(value);
	}
};