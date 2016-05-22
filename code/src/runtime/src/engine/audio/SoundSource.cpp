#include <engine/audio/SoundSource.hpp>
#include <fmod.hpp>

void SoundSource::stop() {
  if (m_channel) {
    m_channel->stop();
    m_channel = nullptr;
    m_state = PlaybackState::STOPPED;
  }
}

void SoundSource::pause() {
  if (m_channel) {
    m_channel->setPaused(true);
    m_state = PlaybackState::PAUSED;
  }
}

void SoundSource::play(int loops) {
  bool isPaused;
  m_channel->getPaused(&isPaused);
  if (isPaused && m_channel) {
    m_channel->setPaused(false);
  } else if(!m_channel) {
    m_channel = m_sound->play();
  }
  m_channel->setVolume(m_volume);
  m_channel->setLoopCount(loops);
  m_state = PlaybackState::PLAYING;
}

void SoundSource::setVolume(float vol) {
  m_volume = vol;
  if (m_channel) {
    m_channel->setVolume(vol);
  }
}