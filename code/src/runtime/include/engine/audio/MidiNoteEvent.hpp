#pragma once
#include <engine/events/Event.hpp>

struct MidiNoteEvent : public Event<MidiNoteEvent> {
  MidiNoteEvent(int32_t channel, bool on, int32_t noteIndex, float velocity)
      : channel(channel), on(on), noteIndex(noteIndex), velocity(velocity){};
  int32_t channel; 
  bool on; 
  int32_t noteIndex;
  float velocity;
};