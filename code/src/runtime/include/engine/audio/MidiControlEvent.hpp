#pragma once
#include <engine/events/Event.hpp>

struct MidiControlEvent : public Event<MidiControlEvent> {
  MidiControlEvent(int32_t channel, int32_t controlIndex, float value)
      : channel(channel), controlIndex(controlIndex), value(value){};
  int32_t channel;
  int32_t controlIndex;
  float value;
};