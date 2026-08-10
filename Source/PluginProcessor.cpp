#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <BinaryData.h>

static int midiCcForParameter(const juce::String& id)
{
    if (id == "effect")    return 20;
    if (id == "variation") return 21;
    if (id == "engine")    return 22;
    if (id == "editA")     return 23;
    if (id == "editB")     return 24;
    if (id == "lowEQ")     return 25;
    if (id == "highEQ")    return 26;
    if (id == "dryWet")    return 27;
    if (id == "bypass")    return 29;
    if (id == "mixExternal") return 30;
    return -1;
}

XSideAudioProcessor::XSideAudioProcessor()
    : AudioProcessor(
        BusesProperties()
            .withInput("Input", juce::AudioChannelSet::stereo(), true)
            .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "STATE", createLayout())
{
    const auto json =
        juce::String::fromUTF8(
            BinaryData::factory_presets_json,
            BinaryData::factory_presets_jsonSize);

    factoryPresets = juce::JSON::parse(json);
    liveSlots.resize(12);

    for (auto id : {
            "effect","variation","engine","editA","editB",
            "lowEQ","highEQ","dryWet","bypass","mixExternal"
        })
    {
        apvts.addParameterListener(id, this);
    }

    // IMPORTANT:
    // Do not enumerate or open MIDI devices in the constructor.
    // Some hosts instantiate plugins while scanning.
}

XSideAudioProcessor::~XSideAudioProcessor()
{
    for (auto id : {
            "effect","variation","engine","editA","editB",
            "lowEQ","highEQ","dryWet","bypass","mixExternal"
        })
    {
        apvts.removeParameterListener(id, this);
    }

    disconnectMidiOut();
}

juce::AudioProcessorValueTreeState::ParameterLayout
XSideAudioProcessor::createLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "effect", "Effect", 0, 31, 0));

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "variation", "Variation", 0, 31, 0));

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "engine", "Engine", 0, 2, 0));

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "editA", "Param A", 0, 63, 0));

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "editB", "Param B", 0, 63, 0));

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "lowEQ", "Low EQ", 0, 32, 16));

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "highEQ", "High EQ", 0, 32, 16));

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "dryWet", "Dry/Wet", 0, 100, 100));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "bypass", "Bypass", false));

    // CC30: 0 = Mix Internal, 1 = Mix External
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "mixExternal", "Mix External", false));

    return { params.begin(), params.end() };
}

bool XSideAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layout) const
{
    const auto& input = layout.getMainInputChannelSet();
    const auto& output = layout.getMainOutputChannelSet();

    return input == output
        && (output == juce::AudioChannelSet::mono()
         || output == juce::AudioChannelSet::stereo());
}

void XSideAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(buffer);
}

juce::StringArray XSideAudioProcessor::getMidiOutNames() const
{
    juce::StringArray names;

    for (const auto& device : juce::MidiOutput::getAvailableDevices())
        names.add(device.name);

    return names;
}

bool XSideAudioProcessor::connectMidiOut()
{
    disconnectMidiOut();
    lastMidiError.clear();

    const auto devices = juce::MidiOutput::getAvailableDevices();

    if (!juce::isPositiveAndBelow(selectedMidiOutIndex, devices.size()))
    {
        lastMidiError = "Select MIDI OUT first.";
        return false;
    }

    auto newOutput =
        juce::MidiOutput::openDevice(
            devices[selectedMidiOutIndex].identifier);

    if (!newOutput)
    {
        lastMidiError =
            "Could not open MIDI OUT. "
            "The port may already be used by the host.";
        return false;
    }

    {
        const juce::ScopedLock lock(midiOutLock);
        midiOut = std::move(newOutput);
    }

    return true;
}

void XSideAudioProcessor::disconnectMidiOut()
{
    const juce::ScopedLock lock(midiOutLock);
    midiOut.reset();
}

juce::String XSideAudioProcessor::getMidiStatusText() const
{
    return juce::String(
        isMidiOutConnected()
            ? "DSP MIDI OUT CONNECTED"
            : "DSP MIDI OUT OFF");
}

void XSideAudioProcessor::sendCC(int cc, int value)
{
    if (suppressMidiOut.load())
        return;

    const juce::ScopedLock lock(midiOutLock);

    if (!midiOut)
        return;

    midiOut->sendMessageNow(
        juce::MidiMessage::controllerEvent(
            midiChannel,
            cc,
            juce::jlimit(0, 127, value)));
}

void XSideAudioProcessor::parameterChanged(
    const juce::String& id,
    float newValue)
{
    const int cc = midiCcForParameter(id);

    if (cc < 0)
        return;

    if (!suppressMidiOut.load())
        edited.store(true);

    int midiValue = juce::roundToInt(newValue);

    // DSP1000P:
    // CC29 -> 0 = bypass, 1 = effect active.
    if (id == "bypass")
        midiValue = (newValue >= 0.5f) ? 0 : 1;

    // Real DSP1000P hardware test showed the previous CC30 mapping was inverted.
    if (id == "mixExternal")
        midiValue = (newValue >= 0.5f) ? 0 : 1;

    sendCC(cc, midiValue);
}

juce::var XSideAudioProcessor::snapshot() const
{
    auto* object = new juce::DynamicObject();

    object->setProperty("name", currentPresetName);

    const char* ids[] =
    {
        "effect",
        "variation",
        "engine",
        "editA",
        "editB",
        "lowEQ",
        "highEQ",
        "dryWet"
    };

    for (const auto* id : ids)
    {
        if (auto* value = apvts.getRawParameterValue(id))
            object->setProperty(id, value->load());
    }

    return juce::var(object);
}

void XSideAudioProcessor::applySnapshot(
    const juce::var& snapshotVar)
{
    if (!snapshotVar.isObject())
        return;

    auto* object = snapshotVar.getDynamicObject();

    if (object == nullptr)
        return;

    if (object->hasProperty("name"))
        currentPresetName =
            object->getProperty("name").toString();

    const char* ids[] =
    {
        "effect",
        "variation",
        "engine",
        "editA",
        "editB",
        "lowEQ",
        "highEQ",
        "dryWet"
    };

    for (const auto* id : ids)
    {
        if (!object->hasProperty(id))
            continue;

        if (auto* parameter = apvts.getParameter(id))
        {
            const float raw =
                static_cast<float>(
                    object->getProperty(id));

            parameter->setValueNotifyingHost(
                parameter->convertTo0to1(raw));
        }
    }
}

void XSideAudioProcessor::loadFactoryPreset(
    const juce::var& presetVar)
{
    if (!presetVar.isObject())
        return;

    auto* object =
        presetVar.getDynamicObject();

    if (object == nullptr)
        return;

    struct Map
    {
        const char* parameterId;
        const char* jsonId;
    };

    const Map mapping[] =
    {
        { "effect",    "effect" },
        { "variation", "variation" },
        { "engine",    "engine" },
        { "editA",     "edit_a" },
        { "editB",     "edit_b" },
        { "lowEQ",     "eq_lo" },
        { "highEQ",    "eq_hi" },
        { "dryWet",    "mix" }
    };

    for (const auto& map : mapping)
    {
        if (!object->hasProperty(map.jsonId))
            continue;

        if (auto* parameter =
                apvts.getParameter(map.parameterId))
        {
            const float raw =
                static_cast<float>(
                    object->getProperty(map.jsonId));

            parameter->setValueNotifyingHost(
                parameter->convertTo0to1(raw));
        }
    }

    if (object->hasProperty("name"))
        currentPresetName =
            object->getProperty("name").toString();
    else
        currentPresetName = "Preset";
}

void XSideAudioProcessor::storeLiveSlot(int slot)
{
    if (!juce::isPositiveAndBelow(slot, 12))
        return;

    liveSlots.set(slot, snapshot());
}

void XSideAudioProcessor::recallLiveSlot(int slot)
{
    if (!juce::isPositiveAndBelow(slot, 12))
        return;

    if (liveSlots[slot].isVoid())
        return;

    applySnapshot(liveSlots[slot]);
}

juce::String XSideAudioProcessor::getLiveSlotName(
    int slot) const
{
    if (!juce::isPositiveAndBelow(slot, 12))
        return "EMPTY";

    if (liveSlots[slot].isVoid())
        return "EMPTY";

    const auto& slotVar =
        liveSlots[slot];

    if (!slotVar.isObject())
        return "LIVE";

    auto* object =
        slotVar.getDynamicObject();

    if (object == nullptr)
        return "LIVE";

    if (!object->hasProperty("name"))
        return "LIVE";

    return object
        ->getProperty("name")
        .toString();
}

bool XSideAudioProcessor::liveSlotHasData(
    int slot) const
{
    return juce::isPositiveAndBelow(slot, 12)
        && !liveSlots[slot].isVoid();
}

void XSideAudioProcessor::setPresetBrowserFilters(
    const juce::String& instrument,
    const juce::String& effectType,
    const juce::String& style,
    bool favouritesOnly)
{
    presetInstrumentFilter = instrument.isNotEmpty() ? instrument : "ALL";
    presetEffectFilter = effectType.isNotEmpty() ? effectType : "ALL";
    presetStyleFilter = style.isNotEmpty() ? style : "ALL";
    presetFavouritesOnly = favouritesOnly;
}

bool XSideAudioProcessor::isPresetFavourite(int factoryIndex) const
{
    return presetFavourites.contains(factoryIndex);
}

void XSideAudioProcessor::setPresetFavourite(
    int factoryIndex,
    bool favourite)
{
    if (factoryIndex < 0)
        return;

    if (favourite)
    {
        if (!presetFavourites.contains(factoryIndex))
            presetFavourites.add(factoryIndex);
    }
    else
    {
        presetFavourites.removeAllInstancesOf(factoryIndex);
    }
}

void XSideAudioProcessor::storeCurrentToDSPUserSlot()
{
    // DSP1000P CC28 = Store destination.
    // Plugin UI uses slots 1..100; MIDI value is 0..99.
    sendCC(28, juce::jlimit(0, 99, userPresetSlot - 1));

    // After a successful Store command, the local edit indicator can be cleared.
    edited.store(false);
}

void XSideAudioProcessor::recallDSPUserSlot()
{
    // DSP1000P recalls user programs through MIDI Program Change.
    // Plugin UI uses slots 1..100; MIDI program is 0..99.
    const juce::ScopedLock lock(midiOutLock);

    if (!midiOut)
        return;

    midiOut->sendMessageNow(
        juce::MidiMessage::programChange(
            midiChannel,
            juce::jlimit(0, 99, userPresetSlot - 1)));

    edited.store(false);
}

void XSideAudioProcessor::getStateInformation(
    juce::MemoryBlock& destinationData)
{
    auto state =
        apvts.copyState();

    state.setProperty(
        "presetName",
        currentPresetName,
        nullptr);

    juce::Array<juce::var> liveArray;

    for (int i = 0; i < 12; ++i)
        liveArray.add(liveSlots[i]);

    state.setProperty(
        "liveSlotsJson",
        juce::JSON::toString(
            juce::var(liveArray)),
        nullptr);

    // Save only the selection.
    // MIDI device is NOT auto-opened on reload.
    state.setProperty(
        "selectedMidiOut",
        selectedMidiOutIndex,
        nullptr);

    state.setProperty(
        "midiChannel",
        midiChannel,
        nullptr);

    state.setProperty(
        "userPresetSlot",
        userPresetSlot,
        nullptr);

    state.setProperty("presetInstrumentFilter", presetInstrumentFilter, nullptr);
    state.setProperty("presetEffectFilter", presetEffectFilter, nullptr);
    state.setProperty("presetStyleFilter", presetStyleFilter, nullptr);
    state.setProperty("presetFavouritesOnly", presetFavouritesOnly, nullptr);

    juce::Array<juce::var> favouriteArray;
    for (const auto index : presetFavourites)
        favouriteArray.add(index);

    state.setProperty(
        "presetFavouritesJson",
        juce::JSON::toString(juce::var(favouriteArray)),
        nullptr);

    if (auto xml = state.createXml())
        copyXmlToBinary(
            *xml,
            destinationData);
}

void XSideAudioProcessor::setStateInformation(
    const void* data,
    int sizeInBytes)
{
    auto xml =
        getXmlFromBinary(
            data,
            sizeInBytes);

    if (xml == nullptr)
        return;

    auto state =
        juce::ValueTree::fromXml(*xml);

    if (!state.isValid())
        return;

    // Prevent restored automation values from sending MIDI
    // while the host is loading the project.
    suppressMidiOut.store(true);

    apvts.replaceState(state);

    suppressMidiOut.store(false);
    edited.store(false);

    currentPresetName =
        state.getProperty(
            "presetName",
            "DSP Capture").toString();

    liveSlots.clear();
    liveSlots.resize(12);

    const auto liveJson =
        state.getProperty(
            "liveSlotsJson",
            "[]").toString();

    const auto parsed =
        juce::JSON::parse(liveJson);

    if (auto* array = parsed.getArray())
    {
        const int count =
            juce::jmin(
                12,
                array->size());

        for (int i = 0; i < count; ++i)
            liveSlots.set(
                i,
                (*array)[i]);
    }

    selectedMidiOutIndex =
        static_cast<int>(
            state.getProperty(
                "selectedMidiOut",
                -1));

    midiChannel =
        juce::jlimit(
            1,
            16,
            static_cast<int>(
                state.getProperty(
                    "midiChannel",
                    1)));

    userPresetSlot =
        juce::jlimit(
            1,
            100,
            static_cast<int>(
                state.getProperty(
                    "userPresetSlot",
                    1)));

    presetInstrumentFilter =
        state.getProperty("presetInstrumentFilter", "ALL").toString();

    presetEffectFilter =
        state.getProperty("presetEffectFilter", "ALL").toString();

    presetStyleFilter =
        state.getProperty("presetStyleFilter", "ALL").toString();

    presetFavouritesOnly =
        static_cast<bool>(
            state.getProperty("presetFavouritesOnly", false));

    presetFavourites.clear();

    const auto favouritesParsed =
        juce::JSON::parse(
            state.getProperty(
                "presetFavouritesJson",
                "[]").toString());

    if (auto* favouriteArray = favouritesParsed.getArray())
    {
        for (const auto& item : *favouriteArray)
        {
            const int index = static_cast<int>(item);

            if (index >= 0 && !presetFavourites.contains(index))
                presetFavourites.add(index);
        }
    }

    // IMPORTANT:
    // We intentionally do NOT call connectMidiOut() here.
}

juce::AudioProcessorEditor*
XSideAudioProcessor::createEditor()
{
    return new XSideAudioProcessorEditor(*this);
}

juce::AudioProcessor*
JUCE_CALLTYPE
createPluginFilter()
{
    return new XSideAudioProcessor();
}
