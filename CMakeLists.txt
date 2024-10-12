cmake_minimum_required(VERSION 3.24)

if(APPLE)
    set(FORMATS Standalone AU VST3)
elseif(LINUX)
        set(CMAKE_CXX_COMPILER "g++")
        set(FORMATS Standalone LV2 VST3)
else()
    set(FORMATS Standalone VST3)
endif()


# Change to your project name
project(DistorK VERSION 2.1.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_XCODE_GENERATE_SCHEME OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


include(FetchContent)
FetchContent_Declare(
  juce
  GIT_REPOSITORY https://github.com/juce-framework/JUCE
  GIT_TAG master
  GIT_SHALLOW ON
)
FetchContent_MakeAvailable(juce)

# Make sure you include any new source files here
set(SourceFiles
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/DSP/BitCrusherData.cpp
        Source/DSP/BitCrusherData.h
        Source/DSP/clipperData.cpp
        Source/DSP/clipperData.h
        Source/DSP/DistorkEngine.cpp
        Source/DSP/DistorkEngine.h
        Source/DSP/kLevelMeterData.h
        Source/DSP/SaturationData.cpp
        Source/DSP/SaturationData.h
        Source/DSP/WaveShaperData.cpp
        Source/DSP/WaveShaperData.h
        Source/GUI/kLookAndFeel.cpp
        Source/GUI/kLookAndFeel.h
        Source/GUI/BitCrusher.cpp
        Source/GUI/BitCrusher.h
        Source/GUI/Clipper.cpp
        Source/GUI/Clipper.h
        Source/GUI/kLevelMeter.h
        Source/GUI/MasterControls.cpp
        Source/GUI/MasterControls.h
        Source/GUI/Saturation.cpp
        Source/GUI/Saturation.h
        Source/GUI/SliderWithLabels.cpp
        Source/GUI/SliderWithLabels.h
        Source/GUI/Toggle.cpp
        Source/GUI/Toggle.h
        Source/GUI/Toolbar.cpp
        Source/GUI/Toolbar.h
        Source/GUI/WaveShaper.cpp
        Source/GUI/WaveShaper.h
        Source/GUI/rotarySliderWithLabels.cpp
        Source/GUI/rotarySliderWithLabels.h
        Source/Utility/KitikToolbar.cpp
        Source/Utility/KitikToolbar.h
        Source/Utility/overSampleGain.cpp
        Source/Utility/overSampleGain.h
)

# Change these to your own preferences
juce_add_plugin(${PROJECT_NAME}
        COMPANY_NAME "KiTiK Music"
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT FALSE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
        JUCE_VST3_CAN_REPLACE_VST2 FALSE
        COPY_PLUGIN_AFTER_BUILD TRUE
        PLUGIN_MANUFACTURER_CODE 
        PLUGIN_CODE 
        FORMATS "${FORMATS}"
        PRODUCT_NAME "DistorK"
        LV2URI https://github.com/KiTiKDub/ChopChop.git
        LV2_SHARED_LIBRARY_NAME ChopChop      
        )

# How we want our SourceFiles to appear in our IDE
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SourceFiles})

# Make the SourceFiles buildable
target_sources(${PROJECT_NAME} PRIVATE ${SourceFiles})

# Adds a BinaryData target for embedding assets into the binary
file(GLOB_RECURSE AssetFiles CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Assets/*")
juce_add_binary_data(Assets SOURCES ${AssetFiles})

# Make linux happy
set_target_properties(Assets PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

# These are some toggleable options from the JUCE CMake API
target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# JUCE libraries to bring into our project
target_link_libraries(${PROJECT_NAME}
        PUBLIC
        Assets
        juce::juce_analytics
        juce::juce_audio_basics
        juce::juce_audio_devices
        #juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

