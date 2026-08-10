X-SIDE VST3 v4.7 MIDI OUT SAFE

База:
рабочая v4.6.2 LIVE Safe.

Добавлено:
- MIDI OUT device list;
- REFRESH MIDI;
- CONNECT DSP;
- DISCONNECT;
- MIDI channel 1-16;
- отправка CC при изменении параметров;
- пресеты и LIVE также отправляют параметры через VST3 automation listeners;
- выбранный MIDI OUT и MIDI channel сохраняются в проекте.

Безопасность:
- MIDI устройства НЕ открываются в constructor;
- MIDI устройства НЕ открываются в setStateInformation;
- после загрузки проекта требуется нажать CONNECT DSP вручную;
- MIDI IN и Auto Read полностью отсутствуют.

Тест:
1. Сначала открыть Ableton без DSP.
2. Вставить X-Side.
3. Убедиться, что Live не падает.
4. Открыть окно X-Side.
5. Нажать REFRESH MIDI.
6. Выбрать USB-MIDI OUT.
7. Выбрать MIDI Channel.
8. Нажать CONNECT DSP.
9. Проверить Effect / Variation / A / B / EQ / Dry-Wet.

Если эта версия стабильна, следующим этапом можно добавить MIDI IN Auto Read
отдельным безопасным модулем.
