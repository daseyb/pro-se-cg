#include <engine/audio/AudioSystem.hpp>
#include <engine/audio/SoundSource.hpp>
#include <engine/scene/Transform.hpp>
#include <assert.h>
#include <glow/common/log.hh>

#define CHECK_RESULT(res, msg) if(res != FMOD_OK) {glow::error() << msg << "\n"; return false; }
#define LATENCY_MS      (10) /* Some devices will require higher latency to avoid glitches */
#define DRIFT_MS        (1)

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

  int numRecordDrivers = 0;
  int numConnectedRecordDrivers = 0;
  result = m_fmodSystem->getRecordNumDrivers(&numRecordDrivers, &numConnectedRecordDrivers);
  CHECK_RESULT(result, "Could not get record driver count.");
  glow::debug() << "Record Drivers: " << numRecordDrivers << ", connected: " << numConnectedRecordDrivers << '\n';

  for (int i = 0; i < numRecordDrivers; i++) {
      char driverName[128] = {};
      FMOD_GUID guid;
      int systemrate;
      FMOD_SPEAKERMODE speakerMode;
      int speakermodechannels;
      FMOD_DRIVER_STATE driverState;

      result = m_fmodSystem->getRecordDriverInfo(i, driverName, 128, &guid, &systemrate, &speakerMode, &speakermodechannels, &driverState);
      CHECK_RESULT(result, "Could not get record driver info.");
      glow::debug() << i << ": " << driverName << ", GUID: " << guid.Data1
                    << ", System Rate: " << systemrate
                    << ", Speaker Mode: " << speakerMode
                    << ", Channels: " << speakermodechannels
                    << ", Driver State: " << driverState << '\n';
  }

  FMOD_CREATESOUNDEXINFO exInfo = {};
  exInfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
  exInfo.numchannels = 2;
  exInfo.format = FMOD_SOUND_FORMAT_PCM16;
  exInfo.defaultfrequency = 44100;
  exInfo.length = exInfo.defaultfrequency * sizeof(short) * exInfo.numchannels;

  m_recordingNativeRate = exInfo.defaultfrequency;

  m_recordingDriftThreshold = (m_recordingNativeRate * DRIFT_MS) / 1000;       /* The point where we start compensating for drift */
  m_recordingDesiredLatency = (m_recordingNativeRate * LATENCY_MS) / 1000;     /* User specified latency */
  m_recordingAdjustedLatency = m_recordingDesiredLatency;                      /* User specified latency adjusted for driver update granularity */
  m_recordingActualLatency = m_recordingDesiredLatency;                                 /* Latency measured once playback begins (smoothened for jitter) */


  FMOD::Sound* recordingSound;
  result = m_fmodSystem->createSound(NULL, FMOD_OPENUSER | FMOD_LOOP_NORMAL, &exInfo, &recordingSound);
  CHECK_RESULT(result, "Could not create sound for recording.");
  
  auto recordSound = createSound(recordingSound);

  m_recordSoundEntity = m_scene->create();
  auto soundSource = m_recordSoundEntity.assign<SoundSource>(recordSound);

  result = m_fmodSystem->recordStart(0, recordingSound, true);
  CHECK_RESULT(result, "Could not start recording.");

  recordingSound->getLength(&m_recordingSoundLength, FMOD_TIMEUNIT_PCM);

 // setLowPassCutoff(800);

  soundSource->setVolume(1.0f);
  soundSource->play(-1);

  m_recordingChannel = soundSource->m_channel;

  m_isActive = true;
  return true;
}

std::shared_ptr<Sound> AudioSystem::createSound(FMOD::Sound* sound) {
    assert(sound);
    return std::shared_ptr<Sound>(new Sound(m_nextId++, sound, this));
}

std::shared_ptr<Sound> AudioSystem::createSound(std::string filename, SoundMode mode) {
  FMOD::Sound* sound;
  auto result = m_fmodSystem->createSound((m_settings->getFullSoundPath() +  filename).c_str(), (mode == SoundMode::MODE_2D ? FMOD_2D : FMOD_3D) | FMOD_LOOP_NORMAL, 0, &sound);
  assert(result == FMOD_OK);

  return createSound(sound);
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

  unsigned int recordPos = 0;
  m_fmodSystem->getRecordPosition(0, &recordPos);

  static unsigned int lastRecordPos = 0;
  unsigned int recordDelta = (recordPos >= lastRecordPos) ? (recordPos - lastRecordPos) : (recordPos + m_recordingSoundLength - lastRecordPos);
  lastRecordPos = recordPos;
  m_recordingSamplesRecorded += recordDelta;

  static unsigned int minRecordDelta = (unsigned int)-1;
  if (recordDelta && (recordDelta < minRecordDelta)) {
      minRecordDelta = recordDelta; /* Smallest driver granularity seen so far */
      m_recordingAdjustedLatency = (recordDelta <= m_recordingDesiredLatency) ? m_recordingDesiredLatency : recordDelta; /* Adjust our latency if driver granularity is high */
  }

  /*
  Determine how much has been played since we last checked.
  */
  unsigned int playPos = 0;
  m_recordingChannel->getPosition(&playPos, FMOD_TIMEUNIT_PCM);

  static unsigned int lastPlayPos = 0;
  unsigned int playDelta = (playPos >= lastPlayPos) ? (playPos - lastPlayPos) : (playPos + m_recordingSoundLength - lastPlayPos);
  lastPlayPos = playPos;
  m_recordingSamplesPlayed += playDelta;

  /*
  Compensate for any drift.
  */
  int latency = m_recordingSamplesRecorded - m_recordingSamplesPlayed;
  m_recordingActualLatency = (0.97f * m_recordingActualLatency) + (0.03f * latency);

  int playbackRate = m_recordingNativeRate;
  if (m_recordingActualLatency < (m_recordingAdjustedLatency - m_recordingDriftThreshold)) {
      /* Play position is catching up to the record position, slow playback down by 2% */
      playbackRate = m_recordingNativeRate - (m_recordingNativeRate / 50);
  } else if (m_recordingActualLatency > (m_recordingAdjustedLatency + m_recordingDriftThreshold)) {
      /* Play position is falling behind the record position, speed playback up by 2% */
      playbackRate = m_recordingNativeRate + (m_recordingNativeRate / 50);
  }

  m_recordingChannel->setFrequency((float)playbackRate);
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
