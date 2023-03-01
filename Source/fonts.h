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