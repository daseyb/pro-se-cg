#include <engine/core/Context.hpp>
#include <engine/core/System.hpp>
#include <engine/events/EventSystem.hpp>

class OscToMidiSystem : public System {
private:
  EventSystem *m_events;
  int m_midiChannel;

  int m_lastVelocity;
public:
  CONSTRUCT_SYSTEM(OscToMidiSystem, int midiChannel), m_midiChannel(midiChannel) {}
  bool startup() override;
  void shutdown() override;
};