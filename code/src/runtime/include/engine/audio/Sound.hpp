#pragma once

typedef unsigned long SoundId;

class AudioSystem;

namespace FMOD {
  class Sound;
  class Channel;
}

struct Sound {
  friend class AudioSystem;
  friend struct SoundSource;
public:
  inline SoundId id() const { return m_id; };
  ~Sound();

private:
  Sound(SoundId id, FMOD::Sound* ptr, AudioSystem* system) : m_id(id), m_soundPtr(ptr), m_system(system) {}
  FMOD::Channel* play();
  SoundId m_id;
  AudioSystem* m_system;
  FMOD::Sound* m_soundPtr;
};

