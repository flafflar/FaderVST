cmake_minimum_required(VERSION 3.11)

project(FaderVST VERSION 0.0.0)

include(FetchContent)
FetchContent_Declare(
	JUCE
	GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
	GIT_TAG 7.0.2
	GIT_SHALLOW true
	GIT_PROGRESS true
)
FetchContent_MakeAvailable(JUCE)

set(FORMATS VST3 Standalone)
juce_add_plugin(
	FaderVST
	FORMATS "${FORMATS}"
	PRODUCT_NAME "FaderVST"
)
	
juce_add_binary_data(
	BinaryData
	SOURCES
	assets/fonts/Noto/NotoSans-Regular.ttf
	assets/fonts/Noto/NotoSans-Bold.ttf
)
set_target_properties(BinaryData PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

target_sources(
	FaderVST
	PRIVATE
	Source/PluginProcessor.cpp
	Source/PluginEditor.cpp
)

target_compile_definitions(
	FaderVST
	PUBLIC
	JUCE_DISPLAY_SPLASH_SCREEN=0
	JUCE_WEB_BROWSER=0
	JUCE_USE_CURL=0
	JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(
	FaderVST
	PRIVATE
	BinaryData
	juce::juce_core
	juce::juce_audio_processors
	juce::juce_audio_plugin_client
	juce::juce_audio_utils
	juce::juce_audio_devices
	juce::juce_graphics
	juce::juce_gui_basics
	juce::juce_gui_extra
    PUBLIC
	juce::juce_recommended_config_flags
	juce::juce_recommended_lto_flags
	juce::juce_recommended_warning_flags
)
