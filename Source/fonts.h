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
#include <juce_graphics/juce_graphics.h>
#include <BinaryData.h>

namespace fonts {

	using juce::Font;
	using juce::Typeface;

	static Font NotoSans(){
		return Font(Typeface::createSystemTypefaceFor(
			BinaryData::NotoSansRegular_ttf,
			BinaryData::NotoSansRegular_ttfSize
		));
	}

	static Font NotoSansBold(){
		return Font(Typeface::createSystemTypefaceFor(
			BinaryData::NotoSansBold_ttf,
			BinaryData::NotoSansBold_ttfSize
		));
	}
}