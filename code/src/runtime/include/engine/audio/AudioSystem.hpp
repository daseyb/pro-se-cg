#pragma once

#include <engine/core/Context.hpp>
#include <engine/core/System.hpp>
#include <engine/events/EventSystem.hpp>
#include <engine/scene/SceneGraphSystem.hpp>
#include <engine/core/SettingsSystem.hpp>
#include <fmod.hpp>
#include <tuple>
#include <engine/audio/Sound.hpp>

enum class SoundMode : uint8_t {
  MODE_3D,
  MODE_2D
};

class AudioSystem : public System {
private:
  friend struct Sound;

  EventSystem *m_events;
  SceneGraphSystem *m_scene;
  SettingsSystem *m_settings;

  FMOD::System *m_fmodSystem;

  FMOD::DSP* m_fftDsp;

  FMOD::DSP* m_lowPassDsp;

  SoundId m_nextId;

  bool m_isActive;

  Entity m_listener;

  Entity m_recordSoundEntity;

  FMOD::Channel* m_recordingChannel;
  unsigned int m_recordingSoundLength = 0;
  unsigned int m_recordingSamplesPlayed = 0;
  unsigned int m_recordingSamplesRecorded = 0;
  int m_recordingActualLatency = 0;
  unsigned int m_recordingNativeRate = 0;
  unsigned int m_recordingDriftThreshold = 0;
  unsigned int m_recordingAdjustedLatency = 0;
  unsigned int m_recordingDesiredLatency = 0;

  FMOD::Channel* playSound(Sound* sound);
  void release(FMOD::Sound* sound);
  void setListenerProperties();

public:
  CONSTRUCT_SYSTEM(AudioSystem), m_nextId(1), m_isActive(false) {}

  void update();

  void setListener(Entity listener) { m_listener = listener; setListenerProperties(); }
  std::shared_ptr<Sound> createSound(std::string filename, SoundMode mode);
  std::shared_ptr<Sound> createSound(FMOD::Sound* sound);

  void getSpectrum(float** data, unsigned int* length) {
    
    FMOD_DSP_PARAMETER_FFT* dspData;
    m_fftDsp->getParameterData(2, (void**)&dspData, length, 0, 0);

    *length = dspData->length;
    *data = dspData->spectrum[1];
  }

  void setLowPassCutoff(float val) {
    m_lowPassDsp->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, val);
  }

  bool startup() override;
  void shutdown() override;
};