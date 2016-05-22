#pragma once
#include <engine/events/Event.hpp>

struct MidiPitchBendEvent : public Event<MidiPitchBendEvent> {
    MidiPitchBendEvent(int32_t channel, float value)
      : channel(channel), value(value){};
  int32_t channel;
  float value;
};