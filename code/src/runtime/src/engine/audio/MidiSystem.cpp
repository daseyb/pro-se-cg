#include <engine/audio/MidiSystem.hpp>
#include <ACGL/ACGL.hh>
#include <engine/core/SimulateEvent.hpp>

bool MidiSystem::startup() {
    RESOLVE_DEPENDENCY(m_events);

    PmError error = Pm_Initialize();

    if (error) {
        ACGL::Utils::error() << Pm_GetErrorText(error) << std::endl;
        return false;
    }

    int deviceCount = Pm_CountDevices();
    ACGL::Utils::debug() << "MIDI Device count: " << Pm_CountDevices() << std::endl;

    if (deviceCount == 0) {
        ACGL::Utils::debug() << "No MIDI input devices found!" << std::endl;
        return true;
    }

    PmDeviceID  defaultDeviceId = Pm_GetDefaultInputDeviceID();
    ACGL::Utils::debug() << "Default Input Device ID: " << defaultDeviceId << std::endl;

    const PmDeviceInfo* defaultDeviceInfo = Pm_GetDeviceInfo(defaultDeviceId);
    ACGL::Utils::debug() << "Default Input Device Name: " << defaultDeviceInfo->name << std::endl;

    error = Pm_OpenInput(&m_inputStream, defaultDeviceId, NULL, 32, NULL, NULL);
    if (error) {
        ACGL::Utils::error() << Pm_GetErrorText(error) << std::endl;
        return false;
    }

    m_events->subscribe<SimulateEvent>([&](const SimulateEvent& e) {
        update();
    });

    return true;
}

void MidiSystem::update() {
    if (!m_inputStream) {
        return;
    }

    PmEvent inBuffer[32];

    int eventCount = Pm_Read(m_inputStream, inBuffer, 32);

    for (int i = 0; i < eventCount; i++) {
        PmEvent e = inBuffer[i];
        ACGL::Utils::debug() << "Recieved MIDI event..."
                             << "|Timestamp:" << e.timestamp
                             << "|Status:" << Pm_MessageStatus(e.message)
                             << "|Data1:" << Pm_MessageData1(e.message)
                             << "|Data2:" << Pm_MessageData2(e.message)
                             << std::endl;
    }
}


void MidiSystem::shutdown() {

    if (m_inputStream) {
        Pm_Close(m_inputStream);
    }
    Pm_Terminate();
}
