#include <engine/audio/ModSystem.hpp>
#include <engine/events/EventSystem.hpp>
#include <engine/audio/micromod.h>
#include <fstream>

bool ModSystem::startup() {
    RESOLVE_DEPENDENCY(m_events);
    return true;
}


void ModSystem::shutdown() {

}


bool ModSystem::startPlayback(std::string filename) {
    std::ifstream filestream(filename, std::fstream::binary);
    
    if (!filestream.is_open()) {
        return false;
    }

    m_currentPlaybackData.clear();

    m_currentPlaybackData =
        std::vector<char>(std::istreambuf_iterator<char>(filestream),
                          std::istreambuf_iterator<char>());

    return startPlayback(m_currentPlaybackData.data());
}

bool ModSystem::startPlayback(signed char * data) {


    int result = 0; // micromod_initialise(data, DEFAULT_SAMPLING_RATE);

    if (result != 0) {
        return false;
    }

    return true;
}