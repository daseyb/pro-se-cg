#include <engine/core/Context.hpp>
#include <engine/core/System.hpp>
#include <engine/events/EventSystem.hpp>
#include <portmidi.h>

class MidiSystem : public System {
private:
    EventSystem* m_events;

    PortMidiStream* m_inputStream;

public:
    CONSTRUCT_SYSTEM(MidiSystem), m_inputStream(nullptr) {}

    void update();

    bool startup() override;
    void shutdown() override;
};