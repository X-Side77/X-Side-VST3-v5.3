#include "PluginEditor.h"

void XSideAudioProcessorEditor::XSideLookAndFeel::drawRotarySlider(
    juce::Graphics& g,
    int x, int y, int width, int height,
    float sliderPos,
    float rotaryStartAngle,
    float rotaryEndAngle,
    juce::Slider&)
{
    const float diameter = juce::jmin((float) width, (float) height) - 28.0f;
    const float radius = diameter * 0.5f;
    const float cx = x + width * 0.5f;
    const float cy = y + (height - 20) * 0.5f;
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // subtle shadow
    g.setColour(juce::Colour(0x66000000));
    g.fillEllipse(cx - radius + 3.0f, cy - radius + 5.0f, diameter, diameter);

    // metal outer ring
    juce::ColourGradient ring(
        juce::Colour(0xff777d82), cx, cy - radius,
        juce::Colour(0xff24282c), cx, cy + radius, false);
    g.setGradientFill(ring);
    g.fillEllipse(cx - radius, cy - radius, diameter, diameter);

    // dark inner cap
    const float inner = radius - 5.0f;
    juce::ColourGradient cap(
        juce::Colour(0xff555b60), cx - inner * 0.5f, cy - inner,
        juce::Colour(0xff171a1d), cx + inner * 0.5f, cy + inner, false);
    g.setGradientFill(cap);
    g.fillEllipse(cx - inner, cy - inner, inner * 2.0f, inner * 2.0f);

    // small highlight
    g.setColour(juce::Colour(0x33ffffff));
    g.drawEllipse(cx - inner + 2.0f, cy - inner + 2.0f,
                  inner * 2.0f - 4.0f, inner * 2.0f - 4.0f, 1.0f);

    // pointer
    juce::Path pointer;
    const float pointerLength = inner - 8.0f;
    const float pointerThickness = 2.6f;
    pointer.addRoundedRectangle(-pointerThickness * 0.5f,
                                -pointerLength,
                                pointerThickness,
                                pointerLength * 0.55f,
                                1.2f);

    g.setColour(juce::Colour(0xfff2f0e8));
    g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(cx, cy));

    // center dot
    g.setColour(juce::Colour(0xff0e1012));
    g.fillEllipse(cx - 2.0f, cy - 2.0f, 4.0f, 4.0f);
}


static const char* effectNames[32] =
{
    "Cathedral","Plate","Small Hall","Room","Studio","Concert","Stage","Vocal",
    "Percussion","Delay","Echo","Gated Reverb","Reverse Reverb","Vocal Distortion",
    "Rotary Speaker","Vocoder","Pitch","Flanger","Chorus","Tremolo & Delay",
    "Delay & Reverb","Pitch & Reverb","Flanger & Reverb","Chorus & Reverb",
    "Pitch / Reverb","Flanger / Reverb","Chorus / Reverb","Tremolo / Reverb",
    "Delay / Reverb","Pitch / Echo","Flanger / Echo","Chorus / Echo"
};

void XSideAudioProcessorEditor::setupKnob(
    juce::Slider& s,
    const juce::String& suffix)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 66, 20);
    s.setTextValueSuffix(suffix);

    s.setRotaryParameters(
        juce::MathConstants<float>::pi * 1.20f,
        juce::MathConstants<float>::pi * 2.80f,
        true);

    s.setColour(
        juce::Slider::rotarySliderOutlineColourId,
        juce::Colour(0xff353a40));

    s.setColour(
        juce::Slider::rotarySliderFillColourId,
        juce::Colour(0xff8d98a3));

    s.setColour(
        juce::Slider::thumbColourId,
        juce::Colour(0xffe8e8e4));

    s.setColour(
        juce::Slider::textBoxBackgroundColourId,
        juce::Colour(0xff101316));

    s.setColour(
        juce::Slider::textBoxOutlineColourId,
        juce::Colour(0xff30363d));

    s.setColour(
        juce::Slider::textBoxTextColourId,
        juce::Colour(0xff8dff78));
}

void XSideAudioProcessorEditor::setupKnobLabel(
    juce::Label& label,
    const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(juce::FontOptions(12.0f, juce::Font::bold));
    label.setColour(
        juce::Label::textColourId,
        juce::Colour(0xffd8d9d5));
}

XSideAudioProcessorEditor::XSideAudioProcessorEditor(
    XSideAudioProcessor& proc)
    : AudioProcessorEditor(&proc),
      p(proc)
{
    setSize(1120, 825);

    title.setText(
        "X-SIDE VST3 | v5.3 | DSP STORE | DSP1000P",
        juce::dontSendNotification);

    title.setFont(
        juce::FontOptions(20.0f, juce::Font::bold));

    title.setColour(
        juce::Label::textColourId,
        juce::Colour(0xff141414));

    addAndMakeVisible(title);

    presetCaption.setText(
        "PRESET",
        juce::dontSendNotification);

    presetCaption.setFont(
        juce::FontOptions(11.0f, juce::Font::bold));

    presetCaption.setColour(
        juce::Label::textColourId,
        juce::Colour(0xff9aa4ad));

    addAndMakeVisible(presetCaption);

    presetName.setText(
        p.getCurrentPresetName(),
        juce::dontSendNotification);

    presetName.setColour(
        juce::Label::textColourId,
        juce::Colour(0xff8dff78));

    presetName.setFont(
        juce::FontOptions(17.0f, juce::Font::bold));

    addAndMakeVisible(presetName);

    info.setColour(
        juce::Label::textColourId,
        juce::Colour(0xffa7b0b7));

    info.setFont(
        juce::FontOptions(11.5f));

    addAndMakeVisible(info);

    routingTitle.setText(
        "DSP ROUTING",
        juce::dontSendNotification);

    routingTitle.setFont(
        juce::FontOptions(12.0f, juce::Font::bold));

    routingTitle.setColour(
        juce::Label::textColourId,
        juce::Colour(0xffd7d8d4));

    addAndMakeVisible(routingTitle);

    routingInfo.setFont(
        juce::FontOptions(11.5f, juce::Font::bold));

    routingInfo.setColour(
        juce::Label::textColourId,
        juce::Colour(0xff8dff78));

    addAndMakeVisible(routingInfo);

    dualLeftInfo.setFont(
        juce::FontOptions(11.0f, juce::Font::bold));

    dualLeftInfo.setColour(
        juce::Label::textColourId,
        juce::Colour(0xffc6d6e2));

    addAndMakeVisible(dualLeftInfo);

    dualRightInfo.setFont(
        juce::FontOptions(11.0f, juce::Font::bold));

    dualRightInfo.setColour(
        juce::Label::textColourId,
        juce::Colour(0xffc6d6e2));

    addAndMakeVisible(dualRightInfo);

    liveTitle.setText(
        "LIVE SLOTS",
        juce::dontSendNotification);

    liveTitle.setFont(
        juce::FontOptions(13.0f, juce::Font::bold));

    liveTitle.setColour(
        juce::Label::textColourId,
        juce::Colour(0xffd7d8d4));

    addAndMakeVisible(liveTitle);

    midiStatus.setFont(
        juce::FontOptions(11.5f, juce::Font::bold));

    addAndMakeVisible(midiStatus);

    editedStatus.setFont(
        juce::FontOptions(11.5f, juce::Font::bold));

    editedStatus.setColour(
        juce::Label::textColourId,
        juce::Colour(0xffffc65c));

    addAndMakeVisible(editedStatus);

    setupPresetBrowser();

    presets.onChange =
        [this]
        {
            const int visibleIndex = presets.getSelectedItemIndex();

            if (!juce::isPositiveAndBelow(
                    visibleIndex,
                    visiblePresetFactoryIndices.size()))
                return;

            const int factoryIndex =
                visiblePresetFactoryIndices[visibleIndex];

            const auto fp = p.getFactoryPresets();

            if (auto* arr = fp.getArray())
            {
                if (juce::isPositiveAndBelow(
                        factoryIndex,
                        arr->size()))
                {
                    p.loadFactoryPreset((*arr)[factoryIndex]);
                    updateParameterNames();
                    updateRoutingInfo();
                    updateLabels();
                    updateFavouriteButton();
                }
            }
        };

    for (int i = 0; i < 32; ++i)
    {
        effect.addItem(
            juce::String(i + 1).paddedLeft('0', 2)
            + "  "
            + effectNames[i],
            i + 1);
    }

    for (int i = 0; i < 32; ++i)
        variation.addItem(juce::String(i + 1), i + 1);

    engine.addItemList(
        {
            "STEREO / COUPLE",
            "LEFT ENGINE",
            "RIGHT ENGINE"
        },
        1);

    addAndMakeVisible(effect);
    addAndMakeVisible(variation);
    addAndMakeVisible(engine);

    setupKnob(a, "");
    setupKnob(b, "");
    setupKnob(low, "");
    setupKnob(high, "");
    setupKnob(mix, "%");

    for (auto* slider : { &a, &b, &low, &high, &mix })
        slider->setLookAndFeel(&lookAndFeel);

    setupKnobLabel(labelA, "DECAY");
    setupKnobLabel(labelB, "PRE DELAY");
    setupKnobLabel(labelLow, "LOW EQ");
    setupKnobLabel(labelHigh, "HIGH EQ");
    setupKnobLabel(labelMix, "DRY / WET");

    for (auto* label : { &labelA, &labelB, &labelLow, &labelHigh, &labelMix })
        addAndMakeVisible(*label);

    for (auto* slider : { &a, &b, &low, &high, &mix })
        addAndMakeVisible(*slider);

    addAndMakeVisible(bypass);
    addAndMakeVisible(mixExternal);

    ae =
        std::make_unique<
            juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            p.apvts, "effect", effect);

    av =
        std::make_unique<
            juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            p.apvts, "variation", variation);

    aeng =
        std::make_unique<
            juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            p.apvts, "engine", engine);

    aa =
        std::make_unique<
            juce::AudioProcessorValueTreeState::SliderAttachment>(
            p.apvts, "editA", a);

    ab =
        std::make_unique<
            juce::AudioProcessorValueTreeState::SliderAttachment>(
            p.apvts, "editB", b);

    al =
        std::make_unique<
            juce::AudioProcessorValueTreeState::SliderAttachment>(
            p.apvts, "lowEQ", low);

    ah =
        std::make_unique<
            juce::AudioProcessorValueTreeState::SliderAttachment>(
            p.apvts, "highEQ", high);

    am =
        std::make_unique<
            juce::AudioProcessorValueTreeState::SliderAttachment>(
            p.apvts, "dryWet", mix);

    aby =
        std::make_unique<
            juce::AudioProcessorValueTreeState::ButtonAttachment>(
            p.apvts, "bypass", bypass);

    aMixExternal =
        std::make_unique<
            juce::AudioProcessorValueTreeState::ButtonAttachment>(
            p.apvts, "mixExternal", mixExternal);

    effect.onChange = [this]
    {
        updateParameterNames();
        updateRoutingInfo();
        updateLabels();
    };

    variation.onChange = [this] { updateLabels(); };

    engine.onChange = [this]
    {
        updateRoutingInfo();
        updateLabels();
    };

    mixExternal.onClick = [this]
    {
        updateRoutingInfo();
        updateLabels();
    };

    rebuildLiveButtons();

    addAndMakeVisible(midiOut);
    addAndMakeVisible(midiChannel);
    addAndMakeVisible(refreshMidi);
    addAndMakeVisible(connectMidi);
    addAndMakeVisible(disconnectMidi);
    addAndMakeVisible(autoConnect);

    autoConnect.setToggleState(
        true,
        juce::dontSendNotification);

    userSlotLabel.setText(
        "USER SLOT",
        juce::dontSendNotification);

    userSlotLabel.setFont(
        juce::FontOptions(11.5f, juce::Font::bold));

    userSlotLabel.setColour(
        juce::Label::textColourId,
        juce::Colour(0xffd7d8d4));

    addAndMakeVisible(userSlotLabel);
    addAndMakeVisible(userSlot);
    addAndMakeVisible(recallDSPPreset);
    addAndMakeVisible(storeDSPPreset);

    for (int slot = 1; slot <= 100; ++slot)
        userSlot.addItem(
            juce::String(slot),
            slot);

    userSlot.setSelectedId(
        p.getUserPresetSlot(),
        juce::dontSendNotification);

    userSlot.onChange = [this]
    {
        p.setUserPresetSlot(
            userSlot.getSelectedId());
    };

    recallDSPPreset.onClick = [this]
    {
        if (!p.isMidiOutConnected())
        {
            juce::NativeMessageBox::showMessageBoxAsync(
                juce::MessageBoxIconType::WarningIcon,
                "X-Side DSP",
                "DSP1000P MIDI OUT is not connected.");
            return;
        }

        p.setUserPresetSlot(
            userSlot.getSelectedId());

        p.recallDSPUserSlot();

        updateLabels();
    };

    storeDSPPreset.onClick = [this]
    {
        if (!p.isMidiOutConnected())
        {
            juce::NativeMessageBox::showMessageBoxAsync(
                juce::MessageBoxIconType::WarningIcon,
                "X-Side DSP",
                "DSP1000P MIDI OUT is not connected.");
            return;
        }

        p.setUserPresetSlot(
            userSlot.getSelectedId());

        const int slot =
            p.getUserPresetSlot();

        juce::AlertWindow::showOkCancelBox(
            juce::MessageBoxIconType::WarningIcon,
            "Store preset to DSP1000P",
            "Overwrite user preset "
                + juce::String(slot)
                + " on the DSP1000P?",
            "STORE",
            "CANCEL",
            nullptr,
            juce::ModalCallbackFunction::create(
                [this](int result)
                {
                    if (result != 0)
                    {
                        p.storeCurrentToDSPUserSlot();
                        updateLabels();
                    }
                }));
    };

    for (int ch = 1; ch <= 16; ++ch)
        midiChannel.addItem(juce::String(ch), ch);

    midiChannel.setSelectedId(
        p.getMidiChannel(),
        juce::dontSendNotification);

    midiChannel.onChange =
        [this]
        {
            p.setMidiChannel(midiChannel.getSelectedId());
            updateLabels();
        };

    rebuildMidiMenu();
    tryAutoConnect();

    midiOut.onChange =
        [this]
        {
            p.setSelectedMidiOutIndex(
                midiOut.getSelectedItemIndex());
        };

    refreshMidi.onClick =
        [this]
        {
            rebuildMidiMenu();
            updateLabels();
        };

    connectMidi.onClick =
        [this]
        {
            p.setSelectedMidiOutIndex(
                midiOut.getSelectedItemIndex());

            if (!p.connectMidiOut())
            {
                juce::NativeMessageBox::showMessageBoxAsync(
                    juce::MessageBoxIconType::WarningIcon,
                    "X-Side MIDI",
                    p.getLastMidiError());
            }

            updateLabels();
        };

    disconnectMidi.onClick =
        [this]
        {
            p.disconnectMidiOut();
            updateLabels();
        };

    autoConnect.onClick =
        [this]
        {
            if (autoConnect.getToggleState()
                && !p.isMidiOutConnected())
            {
                tryAutoConnect();
            }
        };

    updateParameterNames();
    updateRoutingInfo();
    updateLabels();
}

void XSideAudioProcessorEditor::setupPresetBrowser()
{
    addAndMakeVisible(instrumentFilter);
    addAndMakeVisible(effectTypeFilter);
    addAndMakeVisible(styleFilter);
    addAndMakeVisible(presetSearch);
    addAndMakeVisible(favouritesOnly);
    addAndMakeVisible(favouriteButton);
    addAndMakeVisible(presets);

    instrumentFilter.addItemList(
        {
            "ALL", "VOCAL", "BACK VOCAL", "ACOUSTIC GUITAR",
            "ELECTRIC GUITAR", "SAX", "VIOLIN", "KEYS", "PIANO",
            "SYNTH", "SNARE", "TOMS", "DRUMS", "PERCUSSION",
            "BASS", "FX / SPECIAL"
        }, 1);

    effectTypeFilter.addItemList(
        {
            "ALL", "HALL", "ROOM", "PLATE", "AMBIENCE",
            "DELAY", "ECHO", "CHORUS", "FLANGER", "PITCH",
            "MODULATION", "SPECIAL"
        }, 1);

    styleFilter.addItemList(
        {
            "ALL", "POP", "ROCK", "BALLAD", "80s",
            "RETRO", "JAZZ", "BLUES", "LIVE"
        }, 1);

    auto selectText = [](
        juce::ComboBox& box,
        const juce::String& text)
    {
        for (int i = 0; i < box.getNumItems(); ++i)
        {
            if (box.getItemText(i).equalsIgnoreCase(text))
            {
                box.setSelectedItemIndex(
                    i, juce::dontSendNotification);
                return;
            }
        }

        box.setSelectedItemIndex(
            0, juce::dontSendNotification);
    };

    selectText(instrumentFilter, p.getPresetInstrumentFilter());
    selectText(effectTypeFilter, p.getPresetEffectFilter());
    selectText(styleFilter, p.getPresetStyleFilter());

    favouritesOnly.setToggleState(
        p.getPresetFavouritesOnly(),
        juce::dontSendNotification);

    presetSearch.setTextToShowWhenEmpty(
        "SEARCH PRESETS",
        juce::Colour(0xff707980));

    presetSearch.setColour(
        juce::TextEditor::backgroundColourId,
        juce::Colour(0xff101316));

    presetSearch.setColour(
        juce::TextEditor::textColourId,
        juce::Colour(0xffe4e5e1));

    presetSearch.setColour(
        juce::TextEditor::outlineColourId,
        juce::Colour(0xff30363d));

    auto filtersChanged = [this]
    {
        p.setPresetBrowserFilters(
            instrumentFilter.getText(),
            effectTypeFilter.getText(),
            styleFilter.getText(),
            favouritesOnly.getToggleState());

        applyPresetFilters();
    };

    instrumentFilter.onChange = filtersChanged;
    effectTypeFilter.onChange = filtersChanged;
    styleFilter.onChange = filtersChanged;
    favouritesOnly.onClick = filtersChanged;

    presetSearch.onTextChange = [this]
    {
        applyPresetFilters();
    };

    favouriteButton.onClick = [this]
    {
        const int visibleIndex = presets.getSelectedItemIndex();

        if (!juce::isPositiveAndBelow(
                visibleIndex,
                visiblePresetFactoryIndices.size()))
            return;

        const int factoryIndex =
            visiblePresetFactoryIndices[visibleIndex];

        p.setPresetFavourite(
            factoryIndex,
            !p.isPresetFavourite(factoryIndex));

        updateFavouriteButton();

        if (favouritesOnly.getToggleState())
            applyPresetFilters();
    };

    applyPresetFilters();
}

void XSideAudioProcessorEditor::rebuildPresetMenu()
{
    applyPresetFilters();
}

juce::String XSideAudioProcessorEditor::getPresetNameAt(
    int factoryIndex) const
{
    const auto fp = p.getFactoryPresets();

    if (auto* arr = fp.getArray())
    {
        if (juce::isPositiveAndBelow(factoryIndex, arr->size()))
        {
            const auto& presetVar = (*arr)[factoryIndex];

            if (auto* object = presetVar.getDynamicObject())
            {
                if (object->hasProperty("name"))
                    return object->getProperty("name").toString();
            }
        }
    }

    return "Preset " + juce::String(factoryIndex + 1);
}

int XSideAudioProcessorEditor::getPresetEffectIndex(
    int factoryIndex) const
{
    const auto fp = p.getFactoryPresets();

    if (auto* arr = fp.getArray())
    {
        if (juce::isPositiveAndBelow(factoryIndex, arr->size()))
        {
            if (auto* object =
                    (*arr)[factoryIndex].getDynamicObject())
            {
                if (object->hasProperty("effect"))
                {
                    return juce::jlimit(
                        0, 31,
                        static_cast<int>(
                            object->getProperty("effect")));
                }
            }
        }
    }

    return 0;
}

juce::String XSideAudioProcessorEditor::getEffectTypeForPreset(
    int factoryIndex) const
{
    switch (getPresetEffectIndex(factoryIndex))
    {
        case 0: case 2: case 5: case 6: case 7: return "HALL";
        case 3: case 4: return "ROOM";
        case 1: return "PLATE";
        case 8: return "AMBIENCE";
        case 9: case 19: case 20: case 28: return "DELAY";
        case 10: case 29: case 30: case 31: return "ECHO";
        case 18: case 23: case 26: return "CHORUS";
        case 17: case 22: case 25: return "FLANGER";
        case 16: case 21: case 24: return "PITCH";
        case 14: case 27: return "MODULATION";
        default: return "SPECIAL";
    }
}

juce::String XSideAudioProcessorEditor::getInstrumentForPreset(
    int factoryIndex) const
{
    const auto name = getPresetNameAt(factoryIndex).toUpperCase();

    if (name.contains("BACK") || name.contains("BGV")) return "BACK VOCAL";
    if (name.contains("VOCAL") || name.contains("VOICE")) return "VOCAL";
    if (name.contains("SAX")) return "SAX";
    if (name.contains("VIOLIN") || name.contains("STRING")) return "VIOLIN";
    if (name.contains("PIANO")) return "PIANO";
    if (name.contains("SYNTH") || name.contains("PAD")) return "SYNTH";
    if (name.contains("KEY") || name.contains("ORGAN")) return "KEYS";
    if (name.contains("SNARE")) return "SNARE";
    if (name.contains("TOM")) return "TOMS";
    if (name.contains("DRUM") || name.contains("KIT")) return "DRUMS";
    if (name.contains("PERC") || name.contains("CONGA") || name.contains("BONGO"))
        return "PERCUSSION";
    if (name.contains("BASS")) return "BASS";
    if (name.contains("ACOUSTIC") || name.contains("AC GTR"))
        return "ACOUSTIC GUITAR";
    if (name.contains("GUITAR") || name.contains("GTR"))
        return "ELECTRIC GUITAR";

    if (getEffectTypeForPreset(factoryIndex) == "SPECIAL")
        return "FX / SPECIAL";

    return "VOCAL";
}

juce::String XSideAudioProcessorEditor::getStyleForPreset(
    int factoryIndex) const
{
    const auto name = getPresetNameAt(factoryIndex).toUpperCase();

    if (name.contains("80") || name.contains("EIGHT")) return "80s";
    if (name.contains("RETRO") || name.contains("VINTAGE")) return "RETRO";
    if (name.contains("JAZZ")) return "JAZZ";
    if (name.contains("BLUES")) return "BLUES";
    if (name.contains("BALLAD") || name.contains("BALLADE")) return "BALLAD";
    if (name.contains("ROCK")) return "ROCK";
    if (name.contains("POP")) return "POP";
    if (name.contains("LIVE") || name.contains("STAGE")) return "LIVE";

    const auto type = getEffectTypeForPreset(factoryIndex);

    if (type == "PLATE" || type == "CHORUS") return "POP";
    if (type == "DELAY" || type == "ECHO") return "ROCK";
    if (type == "HALL") return "BALLAD";

    return "LIVE";
}

void XSideAudioProcessorEditor::applyPresetFilters()
{
    const auto instrument =
        instrumentFilter.getText().isNotEmpty()
            ? instrumentFilter.getText()
            : "ALL";

    const auto effectType =
        effectTypeFilter.getText().isNotEmpty()
            ? effectTypeFilter.getText()
            : "ALL";

    const auto style =
        styleFilter.getText().isNotEmpty()
            ? styleFilter.getText()
            : "ALL";

    const auto searchText =
        presetSearch.getText().trim().toUpperCase();

    const bool favourites =
        favouritesOnly.getToggleState();

    presets.clear(juce::dontSendNotification);
    visiblePresetFactoryIndices.clear();

    const auto fp = p.getFactoryPresets();
    auto* arr = fp.getArray();

    if (arr == nullptr)
        return;

    int menuId = 1;

    for (int factoryIndex = 0;
         factoryIndex < arr->size();
         ++factoryIndex)
    {
        const auto presetName = getPresetNameAt(factoryIndex);
        const auto presetInstrument = getInstrumentForPreset(factoryIndex);
        const auto presetEffectType = getEffectTypeForPreset(factoryIndex);
        const auto presetStyle = getStyleForPreset(factoryIndex);

        if (instrument != "ALL" && presetInstrument != instrument) continue;
        if (effectType != "ALL" && presetEffectType != effectType) continue;
        if (style != "ALL" && presetStyle != style) continue;
        if (favourites && !p.isPresetFavourite(factoryIndex)) continue;

        if (searchText.isNotEmpty()
            && !presetName.toUpperCase().contains(searchText))
            continue;

        juce::String displayName;

        if (p.isPresetFavourite(factoryIndex))
            displayName << "* ";

        displayName
            << presetName
            << "  ["
            << presetInstrument
            << " | "
            << presetEffectType
            << " | "
            << presetStyle
            << "]";

        presets.addItem(displayName, menuId++);
        visiblePresetFactoryIndices.add(factoryIndex);
    }

    if (presets.getNumItems() > 0)
        presets.setSelectedItemIndex(
            0, juce::dontSendNotification);

    updateFavouriteButton();
}

void XSideAudioProcessorEditor::updateFavouriteButton()
{
    const int visibleIndex = presets.getSelectedItemIndex();

    if (!juce::isPositiveAndBelow(
            visibleIndex,
            visiblePresetFactoryIndices.size()))
    {
        favouriteButton.setButtonText("ADD FAVORITE");
        return;
    }

    const int factoryIndex =
        visiblePresetFactoryIndices[visibleIndex];

    favouriteButton.setButtonText(
        p.isPresetFavourite(factoryIndex)
            ? "REMOVE FAVORITE"
            : "ADD FAVORITE");
}

void XSideAudioProcessorEditor::rebuildLiveButtons()
{
    liveRecallButtons.clear();
    liveSetButtons.clear();

    for (int i = 0; i < 12; ++i)
    {
        auto* recall =
            liveRecallButtons.add(
                new juce::TextButton(
                    p.getLiveSlotName(i)));

        auto* store =
            liveSetButtons.add(
                new juce::TextButton(
                    "SET " + juce::String(i + 1)));

        recall->setColour(
            juce::TextButton::buttonColourId,
            juce::Colour(0xff252b30));

        recall->setColour(
            juce::TextButton::textColourOffId,
            juce::Colour(0xffe4e5e1));

        store->setColour(
            juce::TextButton::buttonColourId,
            juce::Colour(0xff353c43));

        addAndMakeVisible(recall);
        addAndMakeVisible(store);

        recall->onClick =
            [this, i]
            {
                p.recallLiveSlot(i);
                updateLabels();
            };

        store->onClick =
            [this, i]
            {
                p.storeLiveSlot(i);
                rebuildLiveButtons();
            };
    }

    resized();
    repaint();
}

void XSideAudioProcessorEditor::rebuildMidiMenu()
{
    const int previousIndex =
        p.getSelectedMidiOutIndex();

    midiOut.clear(
        juce::dontSendNotification);

    const auto names =
        p.getMidiOutNames();

    midiOut.addItemList(
        names,
        1);

    if (juce::isPositiveAndBelow(
            previousIndex,
            midiOut.getNumItems()))
    {
        midiOut.setSelectedItemIndex(
            previousIndex,
            juce::dontSendNotification);
    }
}

void XSideAudioProcessorEditor::updateParameterNames()
{
    auto* fxParam = p.apvts.getRawParameterValue("effect");

    if (fxParam == nullptr)
        return;

    const int fx = juce::jlimit(
        0, 31, juce::roundToInt(fxParam->load()));

    juce::String nameA = "PARAM A";
    juce::String nameB = "PARAM B";

    switch (fx)
    {
        // Reverbs
        case 0: case 1: case 2: case 3:
        case 4: case 5: case 6: case 7:
        case 8: case 11: case 12:
            nameA = "DECAY";
            nameB = "PRE DELAY";
            break;

        // Delay / Echo
        case 9: case 10:
            nameA = "DELAY TIME";
            nameB = "FEEDBACK";
            break;

        // Vocal distortion
        case 13:
            nameA = "DRIVE";
            nameB = "TONE";
            break;

        // Rotary
        case 14:
            nameA = "SPEED";
            nameB = "DEPTH";
            break;

        // Vocoder
        case 15:
            nameA = "SENSITIVITY";
            nameB = "RELEASE";
            break;

        // Pitch
        case 16:
            nameA = "PITCH";
            nameB = "FINE";
            break;

        // Flanger
        case 17:
            nameA = "RATE";
            nameB = "DEPTH";
            break;

        // Chorus
        case 18:
            nameA = "RATE";
            nameB = "DEPTH";
            break;

        // Tremolo + Delay
        case 19:
            nameA = "TREMOLO RATE";
            nameB = "DELAY TIME";
            break;

        // Combined algorithms
        case 20:
            nameA = "DELAY TIME";
            nameB = "REVERB DECAY";
            break;

        case 21:
            nameA = "PITCH";
            nameB = "REVERB DECAY";
            break;

        case 22:
            nameA = "FLANGER RATE";
            nameB = "REVERB DECAY";
            break;

        case 23:
            nameA = "CHORUS RATE";
            nameB = "REVERB DECAY";
            break;

        case 24:
            nameA = "PITCH";
            nameB = "REVERB MIX";
            break;

        case 25:
            nameA = "FLANGER RATE";
            nameB = "REVERB MIX";
            break;

        case 26:
            nameA = "CHORUS RATE";
            nameB = "REVERB MIX";
            break;

        case 27:
            nameA = "TREMOLO RATE";
            nameB = "REVERB MIX";
            break;

        case 28:
            nameA = "DELAY TIME";
            nameB = "REVERB MIX";
            break;

        case 29:
            nameA = "PITCH";
            nameB = "ECHO TIME";
            break;

        case 30:
            nameA = "FLANGER RATE";
            nameB = "ECHO TIME";
            break;

        case 31:
            nameA = "CHORUS RATE";
            nameB = "ECHO TIME";
            break;

        default:
            break;
    }

    labelA.setText(nameA, juce::dontSendNotification);
    labelB.setText(nameB, juce::dontSendNotification);
}

void XSideAudioProcessorEditor::tryAutoConnect()
{
    if (!autoConnect.getToggleState())
        return;

    if (p.isMidiOutConnected())
        return;

    const int selectedIndex =
        p.getSelectedMidiOutIndex();

    if (!juce::isPositiveAndBelow(
            selectedIndex,
            midiOut.getNumItems()))
        return;

    midiOut.setSelectedItemIndex(
        selectedIndex,
        juce::dontSendNotification);

    p.setSelectedMidiOutIndex(
        selectedIndex);

    p.connectMidiOut();

    updateLabels();
}

void XSideAudioProcessorEditor::updateRoutingInfo()
{
    auto* fxParam = p.apvts.getRawParameterValue("effect");
    auto* engineParam = p.apvts.getRawParameterValue("engine");
    auto* externalParam = p.apvts.getRawParameterValue("mixExternal");

    if (fxParam == nullptr || engineParam == nullptr || externalParam == nullptr)
        return;

    const int fx = juce::jlimit(
        0, 31, juce::roundToInt(fxParam->load()));

    const int engineTarget = juce::jlimit(
        0, 2, juce::roundToInt(engineParam->load()));

    const bool externalMix = externalParam->load() >= 0.5f;

    juce::String targetText;

    switch (engineTarget)
    {
        case 1: targetText = "EDIT TARGET: LEFT ENGINE"; break;
        case 2: targetText = "EDIT TARGET: RIGHT ENGINE"; break;
        default: targetText = "EDIT TARGET: STEREO / COUPLE"; break;
    }

    routingInfo.setText(
        targetText
        + " | "
        + (externalMix ? "MIX EXTERNAL (DRY/WET DISABLED)" : "MIX INTERNAL (DRY/WET ACTIVE)"),
        juce::dontSendNotification);

    mix.setEnabled(!externalMix);
    labelMix.setEnabled(!externalMix);

    // DSP1000P Dual-Mode algorithms are programs 25..32,
    // represented here as zero-based effect indexes 24..31.
    if (fx < 24)
    {
        dualLeftInfo.setText(
            "STEREO ALGORITHM",
            juce::dontSendNotification);

        dualRightInfo.setText(
            "LEFT + RIGHT COUPLED",
            juce::dontSendNotification);

        return;
    }

    static const char* leftEngine[8] =
    {
        "PITCH",
        "FLANGER",
        "CHORUS",
        "TREMOLO",
        "DELAY",
        "PITCH",
        "FLANGER",
        "CHORUS"
    };

    static const char* rightEngine[8] =
    {
        "REVERB",
        "REVERB",
        "REVERB",
        "REVERB",
        "ECHO",
        "ECHO",
        "ECHO",
        "ECHO"
    };

    const int dualIndex = fx - 24;

    dualLeftInfo.setText(
        "LEFT: " + juce::String(leftEngine[dualIndex]),
        juce::dontSendNotification);

    dualRightInfo.setText(
        "RIGHT: " + juce::String(rightEngine[dualIndex]),
        juce::dontSendNotification);
}

void XSideAudioProcessorEditor::updateLabels()
{
    presetName.setText(
        p.getCurrentPresetName(),
        juce::dontSendNotification);

    auto* fxParam =
        p.apvts.getRawParameterValue("effect");

    auto* varParam =
        p.apvts.getRawParameterValue("variation");

    if (fxParam != nullptr
        && varParam != nullptr)
    {
        const int fx =
            juce::jlimit(
                0,
                31,
                juce::roundToInt(
                    fxParam->load()));

        const int var =
            juce::jlimit(
                0,
                31,
                juce::roundToInt(
                    varParam->load()));

        info.setText(
            juce::String(effectNames[fx])
            + " | VAR "
            + juce::String(var + 1)
            + " | MIDI CH "
            + juce::String(p.getMidiChannel()),
            juce::dontSendNotification);
    }

    midiStatus.setText(
        p.getMidiStatusText(),
        juce::dontSendNotification);

    midiStatus.setColour(
        juce::Label::textColourId,
        p.isMidiOutConnected()
            ? juce::Colour(0xff8dff78)
            : juce::Colour(0xffff7366));

    editedStatus.setText(
        p.isEdited()
            ? "EDITED - DSP STORE FLASHING"
            : "SAVED / UNCHANGED",
        juce::dontSendNotification);
}

void XSideAudioProcessorEditor::paint(
    juce::Graphics& g)
{
    g.fillAll(
        juce::Colour(0xff111417));

    g.setColour(
        juce::Colour(0xff1a1e22));

    g.fillRoundedRectangle(
        getLocalBounds().toFloat().reduced(8),
        10.0f);

    g.setColour(
        juce::Colour(0xffb8b9b5));

    g.fillRoundedRectangle(
        12.0f,
        12.0f,
        static_cast<float>(getWidth() - 24),
        48.0f,
        5.0f);

    g.setColour(
        juce::Colour(0xff2c3238));

    g.drawRoundedRectangle(
        getLocalBounds().toFloat().reduced(8),
        10.0f,
        1.0f);

    // Main control panel
    g.setColour(
        juce::Colour(0xff171b1f));

    g.fillRoundedRectangle(
        18.0f,
        132.0f,
        static_cast<float>(getWidth() - 36),
        315.0f,
        7.0f);
}

void XSideAudioProcessorEditor::resized()
{
    title.setBounds(
        28,
        17,
        getWidth() - 56,
        36);

    presetCaption.setBounds(
        28,
        75,
        100,
        18);

    presetName.setBounds(
        28,
        92,
        500,
        28);

    info.setBounds(
        545,
        92,
        getWidth() - 573,
        28);

    // Preset Browser row
    instrumentFilter.setBounds(34, 138, 175, 30);
    effectTypeFilter.setBounds(219, 138, 140, 30);
    styleFilter.setBounds(369, 138, 120, 30);
    presetSearch.setBounds(499, 138, 220, 30);
    favouritesOnly.setBounds(729, 138, 125, 30);
    favouriteButton.setBounds(864, 138, 142, 30);

    // Preset and DSP selector row
    presets.setBounds(34, 178, 425, 34);
    effect.setBounds(475, 178, 235, 34);
    variation.setBounds(724, 178, 74, 34);
    engine.setBounds(800, 178, 128, 34);
    bypass.setBounds(936, 178, 70, 34);

    mixExternal.setBounds(
        820,
        376,
        186,
        30);

    // Compact knobs
    const int knobY = 244;
    const int knobLabelY = 224;
    const int knobW = 102;
    const int knobH = 122;
    const int gap = 58;
    const int totalW = knobW * 5 + gap * 4;
    const int startX = (getWidth() - totalW) / 2;

    labelA.setBounds(startX, knobLabelY, knobW, 20);
    labelB.setBounds(startX + (knobW + gap), knobLabelY, knobW, 20);
    labelLow.setBounds(startX + (knobW + gap) * 2, knobLabelY, knobW, 20);
    labelHigh.setBounds(startX + (knobW + gap) * 3, knobLabelY, knobW, 20);
    labelMix.setBounds(startX + (knobW + gap) * 4, knobLabelY, knobW, 20);

    a.setBounds(startX, knobY, knobW, knobH);
    b.setBounds(startX + (knobW + gap), knobY, knobW, knobH);
    low.setBounds(startX + (knobW + gap) * 2, knobY, knobW, knobH);
    high.setBounds(startX + (knobW + gap) * 3, knobY, knobW, knobH);
    mix.setBounds(startX + (knobW + gap) * 4, knobY, knobW, knobH);

    routingTitle.setBounds(
        32,
        420,
        120,
        22);

    routingInfo.setBounds(
        160,
        420,
        520,
        22);

    dualLeftInfo.setBounds(
        32,
        448,
        250,
        24);

    dualRightInfo.setBounds(
        290,
        448,
        250,
        24);

    // MIDI strip
    midiOut.setBounds(32, 505, 250, 32);
    midiChannel.setBounds(292, 505, 58, 32);
    refreshMidi.setBounds(360, 505, 105, 32);
    connectMidi.setBounds(475, 505, 115, 32);
    disconnectMidi.setBounds(600, 505, 105, 32);
    autoConnect.setBounds(715, 505, 125, 32);
    midiStatus.setBounds(850, 505, 240, 32);
    editedStatus.setBounds(850, 536, 240, 22);

    userSlotLabel.setBounds(
        32,
        566,
        80,
        28);

    userSlot.setBounds(
        115,
        566,
        75,
        30);

    recallDSPPreset.setBounds(
        202,
        566,
        155,
        30);

    storeDSPPreset.setBounds(
        367,
        566,
        135,
        30);

    liveTitle.setBounds(
        28,
        613,
        160,
        24);

    // LIVE: 6 columns x 2 rows for compact layout
    const int cols = 6;
    const int rows = 2;
    const int areaX = 24;
    const int areaY = 645;
    const int areaW = getWidth() - 48;
    const int areaH = getHeight() - areaY - 18;

    const int cellW = areaW / cols;
    const int cellH = areaH / rows;

    for (int i = 0; i < 12; ++i)
    {
        const int col = i % cols;
        const int row = i / cols;

        auto cell =
            juce::Rectangle<int>(
                areaX + col * cellW,
                areaY + row * cellH,
                cellW,
                cellH)
                .reduced(5);

        auto setArea =
            cell.removeFromBottom(25);

        if (juce::isPositiveAndBelow(
                i,
                liveRecallButtons.size()))
        {
            liveRecallButtons[i]
                ->setBounds(cell);
        }

        if (juce::isPositiveAndBelow(
                i,
                liveSetButtons.size()))
        {
            liveSetButtons[i]
                ->setBounds(setArea);
        }
    }
}
