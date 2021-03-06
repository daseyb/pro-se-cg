#pragma once

#include <engine/core/Context.hpp>
#include <engine/core/System.hpp>
#include <engine/events/EventSystem.hpp>
#include <engine/core/SettingsSystem.hpp>

#include <portmidi.h>

struct MidiKeyState {
    bool isDown;
    float velocity;

    operator bool() {
        return isDown;
    }

    operator float() {
        return velocity;
    }
};


class MidiSystem : public System {
private:
    static const int CHANNEL_COUNT = 16;
    static const int VALUE_COUNT = 127;

    EventSystem* m_events;
    SettingsSystem* m_settings;

    PortMidiStream* m_inputStream;
    PmDeviceID m_connectedDeviceID;

    float m_controlValues[CHANNEL_COUNT][VALUE_COUNT];
    float m_pitchBendValues[CHANNEL_COUNT];
    MidiKeyState m_keyStates[CHANNEL_COUNT][VALUE_COUNT];

public:
    CONSTRUCT_SYSTEM(MidiSystem), m_inputStream(nullptr) {}

    void update();

    float controlValue(int index, int channel = 0) {
        return m_controlValues[channel][index];
    }

    float pitchBend(int channel = 0) {
        return m_pitchBendValues[channel];
    }

    MidiKeyState keyState(int index, int channel = 0) {
        return m_keyStates[channel][index];
    }

    void setDefaultControlValues(float val) {
        for(int c = 0; c < CHANNEL_COUNT; c++) {
            for(int i = 0; i< VALUE_COUNT; i++) {
                m_controlValues[c][i] = val;
            }
        }
    }

    bool startup() override;
    void shutdown() override;

    bool uiFader(std::string name, int index, int channel = 0);
};
