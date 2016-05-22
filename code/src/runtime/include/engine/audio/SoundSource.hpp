#pragma once
#include <engine/scene/Entity.hpp>
#include <engine/audio/Sound.hpp>

namespace FMOD {
  class Channel;
}

class AudioSystem;

enum class PlaybackState {
  STOPPED,
  PLAYING,
  PAUSED
};

struct SoundSource : Component<SoundSource> {
  friend class AudioSystem;
public:
  SoundSource() : m_sound(nullptr), m_channel(nullptr) {}
  SoundSource(std::shared_ptr<Sound> sound) : m_sound(sound), m_channel(nullptr), m_state(PlaybackState::STOPPED) { }

  void stop();
  void pause();
  void play(int loops = 0);
  void setVolume(float vol);

  PlaybackState getState() { return m_state; }

private:
  PlaybackState m_state;
  std::shared_ptr<Sound> m_sound;
  FMOD::Channel *m_channel;
  float m_volume = 1;
};