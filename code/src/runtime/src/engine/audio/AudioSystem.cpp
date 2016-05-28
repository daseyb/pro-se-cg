#include <engine/audio/AudioSystem.hpp>
#include <engine/audio/SoundSource.hpp>
#include <engine/scene/Transform.hpp>
#include <assert.h>
#include <glow/common/log.hh>

#define CHECK_RESULT(res, msg) if(res != FMOD_OK) {glow::error() << msg << "\n"; return false; }


bool AudioSystem::startup() {
  RESOLVE_DEPENDENCY(m_events);
  RESOLVE_DEPENDENCY(m_scene);
  RESOLVE_DEPENDENCY(m_settings);
  
  FMOD_RESULT result;
  unsigned int version;

  result = FMOD::System_Create(&m_fmodSystem);
  CHECK_RESULT(result, "Could not create fmod system.");

  result = m_fmodSystem->getVersion(&version);
  CHECK_RESULT(result, "Could not get fmod version.");

  assert(version == FMOD_VERSION);
  result = m_fmodSystem->init(32, FMOD_INIT_NORMAL, 0);
  CHECK_RESULT(result, "Could not init fmod system.");

  result = m_fmodSystem->createDSPByType(FMOD_DSP_TYPE_FFT, &m_fftDsp);
  CHECK_RESULT(result, "Could not create FFT DSP.");

  FMOD::ChannelGroup* masterChannelGroup;
  result = m_fmodSystem->getMasterChannelGroup(&masterChannelGroup);
  CHECK_RESULT(result, "Could not get master channel group.");

  result = masterChannelGroup->addDSP(1, m_fftDsp);
  CHECK_RESULT(result, "Could not add FFT DSP to master channel group.");


  result = m_fmodSystem->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &m_lowPassDsp);
  CHECK_RESULT(result, "Could not create Low Pass DSP.");

  result = masterChannelGroup->addDSP(0, m_lowPassDsp);
  CHECK_RESULT(result, "Could not add Low Pass to master channel group.");


  setLowPassCutoff(800);

  m_isActive = true;
  return true;
}

std::shared_ptr<Sound> AudioSystem::createSound(std::string filename, SoundMode mode) {
  FMOD::Sound* sound;
  auto result = m_fmodSystem->createSound((m_settings->getFullSoundPath() +  filename).c_str(), mode == SoundMode::MODE_2D ? FMOD_2D : FMOD_3D, 0, &sound);
  assert(result == FMOD_OK);

  return std::shared_ptr<Sound>(new Sound(m_nextId++, sound, this));
}

FMOD::Channel* AudioSystem::playSound(Sound* sound) {
  FMOD::Channel* resultChannel;
  m_fmodSystem->playSound(sound->m_soundPtr, 0, false, &resultChannel);
  return resultChannel;
}

void AudioSystem::setListenerProperties() {
  if (m_listener.valid()) {
    auto transform = m_listener.component<Transform>();
    if (transform.valid()) {
      auto pos = transform->position;
      auto rot = transform->rotation;

      FMOD_VECTOR fmodPos = { pos.x, pos.y, pos.z };
      FMOD_VECTOR fmodVel = { 0, 0, 0 };

      auto up = rot * glm::vec4(0, 1, 0, 0);
      auto forward = rot * glm::vec4(0, 0, -1, 0);

      FMOD_VECTOR fmodUp = { up.x, up.y, up.z };
      FMOD_VECTOR fmodForward = { forward.x, forward.y, forward.z };

      m_fmodSystem->set3DListenerAttributes(0, &fmodPos, &fmodVel, &fmodForward, &fmodUp);
    }
  }
}

void AudioSystem::update() {
  setListenerProperties();

  auto soundSources = m_scene->entities_with_components<SoundSource, Transform>();

  SoundSource::Handle soundSource;
  Transform::Handle transform;

  for (auto e : soundSources) {
    e.unpack<SoundSource, Transform>(soundSource, transform);
    FMOD::Sound* currentSound;
    auto result = soundSource->m_channel->getCurrentSound(&currentSound);
    if (result != FMOD_OK || currentSound != soundSource->m_sound->m_soundPtr) {
      continue;
    }

    FMOD_VECTOR fmodPos = { transform->position.x, transform->position.y, transform->position.z };
    FMOD_VECTOR fmodVel = { 0, 0, 0 };
    soundSource->m_channel->set3DAttributes(&fmodPos, &fmodVel);
  }

  m_fmodSystem->update();
}

void AudioSystem::release(FMOD::Sound* sound) {
  if (m_isActive) {
    sound->release();
  }
}

void AudioSystem::shutdown() {
  m_isActive = false;
  m_fmodSystem->release();
}
