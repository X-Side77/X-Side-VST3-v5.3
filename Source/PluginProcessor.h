#pragma once
#include <JuceHeader.h>
#include <atomic>

class XSideAudioProcessor final
    : public juce::AudioProcessor,
      private juce::AudioProcessorValueTreeState::Listener
{
public:
    XSideAudioProcessor();
    ~XSideAudioProcessor() override;

    void prepareToPlay(double, int) override {}
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "X-Side"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();

    juce::var getFactoryPresets() const { return factoryPresets; }
    void loadFactoryPreset(const juce::var&);
    juce::String getCurrentPresetName() const { return currentPresetName; }

    void storeLiveSlot(int slot);
    void recallLiveSlot(int slot);
    juce::String getLiveSlotName(int slot) const;
    bool liveSlotHasData(int slot) const;

    // Safe MIDI OUT only
    juce::StringArray getMidiOutNames() const;
    void setSelectedMidiOutIndex(int index) { selectedMidiOutIndex = index; }
    int getSelectedMidiOutIndex() const { return selectedMidiOutIndex; }

    bool connectMidiOut();
    void disconnectMidiOut();
    bool isMidiOutConnected() const { return midiOut != nullptr; }
    juce::String getMidiStatusText() const;
    juce::String getLastMidiError() const { return lastMidiError; }

    void setMidiChannel(int channel) { midiChannel = juce::jlimit(1, 16, channel); }
    int getMidiChannel() const { return midiChannel; }

    void setUserPresetSlot(int slot) { userPresetSlot = juce::jlimit(1, 100, slot); }
    int getUserPresetSlot() const { return userPresetSlot; }

    void storeCurrentToDSPUserSlot();
    void recallDSPUserSlot();

    bool isEdited() const { return edited.load(); }
    void clearEdited() { edited.store(false); }

    void setPresetBrowserFilters(const juce::String& instrument,
                                 const juce::String& effectType,
                                 const juce::String& style,
                                 bool favouritesOnly);
    juce::String getPresetInstrumentFilter() const { return presetInstrumentFilter; }
    juce::String getPresetEffectFilter() const { return presetEffectFilter; }
    juce::String getPresetStyleFilter() const { return presetStyleFilter; }
    bool getPresetFavouritesOnly() const { return presetFavouritesOnly; }

    bool isPresetFavourite(int factoryIndex) const;
    void setPresetFavourite(int factoryIndex, bool favourite);

private:
    juce::var factoryPresets;
    juce::String currentPresetName { "DSP Capture" };
    juce::Array<juce::var> liveSlots;

    std::unique_ptr<juce::MidiOutput> midiOut;
    juce::CriticalSection midiOutLock;

    int selectedMidiOutIndex = -1;
    int midiChannel = 1;
    int userPresetSlot = 1;
    juce::String lastMidiError;

    std::atomic<bool> suppressMidiOut { false };
    std::atomic<bool> edited { false };

    juce::String presetInstrumentFilter { "ALL" };
    juce::String presetEffectFilter { "ALL" };
    juce::String presetStyleFilter { "ALL" };
    bool presetFavouritesOnly = false;
    juce::Array<int> presetFavourites;

    juce::var snapshot() const;
    void applySnapshot(const juce::var&);

    void parameterChanged(const juce::String&, float) override;
    void sendCC(int cc, int value);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XSideAudioProcessor)
};
