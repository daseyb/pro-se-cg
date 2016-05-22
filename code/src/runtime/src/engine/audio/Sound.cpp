#include <engine/audio/Sound.hpp>
#include <engine/audio/AudioSystem.hpp>
#include <fmod.hpp>

Sound::~Sound() { m_system->release(m_soundPtr); }

FMOD::Channel* Sound::play() {
  return m_system->playSound(this);
}