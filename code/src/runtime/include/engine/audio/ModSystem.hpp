#include <engine/core/Context.hpp>
#include <engine/core/System.hpp>

class EventSystem;

class ModSystem : public System {
private:
    static const long DEFAULT_SAMPLING_RATE = 48000;
    static const long OVERSAMPLE = 2;
    static const long NUM_CHANNELS = 2;
    static const long BUFFER_SAMPLES = 16384;

    EventSystem *m_events;

    std::vector<char> m_currentPlaybackData;

    short m_mixbuffer[BUFFER_SAMPLES * NUM_CHANNELS * OVERSAMPLE];

public:
    CONSTRUCT_SYSTEM(ModSystem){}

    bool startPlayback(std::string filename);
    bool startPlayback(signed char* data);

    bool startup() override;
    void shutdown() override;
};