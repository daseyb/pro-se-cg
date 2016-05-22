#include <engine/audio/MidiSystem.hpp>
#include <ACGL/ACGL.hh>
#include <engine/core/SimulateEvent.hpp>
#include <engine/audio/RawMidiEvent.hpp>
#include <engine/audio/MidiNoteEvent.hpp>
#include <engine/audio/MidiControlEvent.hpp>
#include <engine/audio/MidiPitchBendEvent.hpp>
#include <engine/ui/UISystem.hpp>

bool MidiSystem::startup() {
  RESOLVE_DEPENDENCY(m_events);


  for (int i = 0; i < CHANNEL_COUNT; i++) {
      m_pitchBendValues[i] = 0;
      for (int j = 0; j < VALUE_COUNT; j++) {
          m_controlValues[i][j] = 0;
          m_keyStates[i][j] = { false, 0 };
      }
  }

  PmError error = Pm_Initialize();

  if (error) {
    ACGL::Utils::error() << Pm_GetErrorText(error) << std::endl;
    return false;
  }

  int deviceCount = Pm_CountDevices();
  ACGL::Utils::debug() << "MIDI Device count: " << Pm_CountDevices()
                       << std::endl;

  if (deviceCount == 0) {
    ACGL::Utils::debug() << "No MIDI input devices found!" << std::endl;
    return true;
  }

  PmDeviceID defaultDeviceId = Pm_GetDefaultInputDeviceID();
  ACGL::Utils::debug() << "Default Input Device ID: " << defaultDeviceId
                       << std::endl;

  const PmDeviceInfo *defaultDeviceInfo = Pm_GetDeviceInfo(defaultDeviceId);
  ACGL::Utils::debug() << "Default Input Device Name: "
                       << defaultDeviceInfo->name << std::endl;

  error = Pm_OpenInput(&m_inputStream, defaultDeviceId, NULL, 32, NULL, NULL);
  if (error) {
    ACGL::Utils::error() << Pm_GetErrorText(error) << std::endl;
    return false;
  }

  m_events->subscribe<SimulateEvent>([&](const SimulateEvent &e) { update(); });


  m_events->subscribe<MidiNoteEvent>([&](const MidiNoteEvent &e) {
      m_keyStates[e.channel][e.noteIndex].isDown = e.on;
      m_keyStates[e.channel][e.noteIndex].velocity = e.velocity;
  });

  m_events->subscribe<MidiControlEvent>([&](const MidiControlEvent &e) {
      m_controlValues[e.channel][e.controlIndex] = e.value;
  });

  m_events->subscribe<MidiPitchBendEvent>([&](const MidiPitchBendEvent &e) {
      m_pitchBendValues[e.channel] = e.value;
  });


  m_events->subscribe<"DrawUI"_sh>([this] {
      static bool valuesOpened = true;
      if (!ImGui::Begin("Midi Controls", &valuesOpened, ImVec2(275, 0), 0.3f)) {
          ImGui::End();
          return;
      }

      static int currentChannel = 0;

      ImGui::DragInt("Channel", &currentChannel, 1.0f, 0, CHANNEL_COUNT - 1);
      float controlValues[VALUE_COUNT];

      for (int i = 0; i < VALUE_COUNT; i++) {
          controlValues[i] = controlValue(i, currentChannel);
      }

      ImGui::PlotHistogram("Values", controlValues, VALUE_COUNT, 0, NULL, 0, 1.0f, glm::vec2(450, 100));

      ImGui::Separator();

      float keyValues[VALUE_COUNT];
      for (int i = 0; i < VALUE_COUNT; i++) {
          keyValues[i] = keyState(i, currentChannel);
      }

      ImGui::PlotHistogram("Keys", keyValues, VALUE_COUNT, 0, NULL, 0, 1.0f, glm::vec2(450, 100));
      ImGui::Separator();

      float val = pitchBend(currentChannel);
      ImGui::DragFloat("Pitch Bend", &val, 1.0f, -1.0f, 1.0f);

      ImGui::End();
  }, 1);



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

    RawMidiEvent rawEvent = {e.timestamp, Pm_MessageStatus(e.message),
                             Pm_MessageData1(e.message),
                             Pm_MessageData2(e.message)};

    m_events->fire<RawMidiEvent>(rawEvent);

    int channel = rawEvent.status % 16;

    if (rawEvent.status >= 128 && rawEvent.status <= 159) {
      m_events->fire<MidiNoteEvent>(
          { channel, (rawEvent.status - 128) / 16 != 0,
           rawEvent.data1, (float)(rawEvent.data2) / 127.0f});

    } else if (rawEvent.status >= 176 && rawEvent.status <= 191) {
      m_events->fire<MidiControlEvent>({ channel,
                                        rawEvent.data1 - 1,
                                        (float)(rawEvent.data2) / 127.0f});
    } else if (rawEvent.status >= 224 && rawEvent.status <= 239) {

        uint16_t intVal = rawEvent.data1 | (rawEvent.data2 << 8);
        float val = ((float)(intVal) / 32768.0f) * 2.0f - 1.0f;
        m_events->fire<MidiPitchBendEvent>({ channel, val });
    }
  }
}

void MidiSystem::shutdown() {

  if (m_inputStream) {
    Pm_Close(m_inputStream);
  }
  Pm_Terminate();
}
