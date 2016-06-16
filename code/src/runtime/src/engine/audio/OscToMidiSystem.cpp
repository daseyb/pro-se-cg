#include <engine/audio/OscToMidiSystem.hpp>
#include <engine/audio/OscEvent.hpp>
#include <engine/audio/MidiNoteEvent.hpp>

bool OscToMidiSystem::startup() {
  RESOLVE_DEPENDENCY(m_events);

  m_lastVelocity = 0;

  m_events->subscribe<OscEvent>([&](const OscEvent &e) {
    int32_t noteIndex;
    int32_t velocity;
    if (e.msg.match("/Velocity*").popInt32(velocity).isOkNoMoreArgs()) {
      m_lastVelocity = velocity;
    } else if (e.msg.match("/Note*").popInt32(noteIndex).isOkNoMoreArgs()) {
      m_events->fire<MidiNoteEvent>({m_midiChannel, m_lastVelocity != 0,
                                     noteIndex,
                                     float(m_lastVelocity) / 127.0f});
    }
  });

  return true;
}

void OscToMidiSystem::shutdown() {}
