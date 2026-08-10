#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class XSideAudioProcessorEditor final
    : public juce::AudioProcessorEditor
{
public:
    explicit XSideAudioProcessorEditor(XSideAudioProcessor&);
    ~XSideAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    class XSideLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        void drawRotarySlider(juce::Graphics&, int, int, int, int,
                              float, float, float, juce::Slider&) override;
    };

    XSideAudioProcessor& p;
    XSideLookAndFeel lookAndFeel;

    juce::Label title;
    juce::Label presetCaption, presetName, info;
    juce::Label liveTitle, midiStatus;
    juce::Label routingTitle, routingInfo, dualLeftInfo, dualRightInfo;
    juce::Label editedStatus;

    juce::Label labelA, labelB, labelLow, labelHigh, labelMix;

    juce::ComboBox presets, effect, variation, engine;
    juce::ComboBox instrumentFilter, effectTypeFilter, styleFilter;
    juce::TextEditor presetSearch;
    juce::ToggleButton favouritesOnly { "FAVORITES" };
    juce::TextButton favouriteButton { "ADD FAVORITE" };
    juce::Slider a,b,low,high,mix;
    juce::ToggleButton bypass { "BYPASS" };
    juce::ToggleButton mixExternal { "MIX EXTERNAL" };

    juce::ComboBox midiOut, midiChannel;
    juce::TextButton refreshMidi { "REFRESH MIDI" };
    juce::TextButton connectMidi { "CONNECT DSP" };
    juce::TextButton disconnectMidi { "DISCONNECT" };
    juce::ToggleButton autoConnect { "AUTO CONNECT" };

    juce::Label userSlotLabel;
    juce::ComboBox userSlot;
    juce::TextButton recallDSPPreset { "RECALL DSP PRESET" };
    juce::TextButton storeDSPPreset { "STORE TO DSP" };

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> ae,av,aeng;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> aa,ab,al,ah,am;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> aby;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> aMixExternal;

    juce::OwnedArray<juce::TextButton> liveRecallButtons;
    juce::OwnedArray<juce::TextButton> liveSetButtons;
    juce::Array<int> visiblePresetFactoryIndices;

    void rebuildPresetMenu();
    void setupPresetBrowser();
    void applyPresetFilters();
    void updateFavouriteButton();
    juce::String getPresetNameAt(int factoryIndex) const;
    int getPresetEffectIndex(int factoryIndex) const;
    juce::String getEffectTypeForPreset(int factoryIndex) const;
    juce::String getInstrumentForPreset(int factoryIndex) const;
    juce::String getStyleForPreset(int factoryIndex) const;
    void rebuildLiveButtons();
    void rebuildMidiMenu();
    void updateLabels();
    void updateParameterNames();
    void updateRoutingInfo();
    void tryAutoConnect();

    static void setupKnob(juce::Slider&, const juce::String&);
    static void setupKnobLabel(juce::Label&, const juce::String&);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XSideAudioProcessorEditor)
};
