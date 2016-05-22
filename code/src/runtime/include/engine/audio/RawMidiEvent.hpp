#pragma once
#include <engine/events/Event.hpp>

struct RawMidiEvent : public Event<RawMidiEvent> {
  RawMidiEvent(int32_t timestamp, int32_t status, int32_t data1, int32_t data2)
      : timestamp(timestamp), status(status), data1(data1), data2(data2){};
  int32_t timestamp;
  int32_t status;
  int32_t data1;
  int32_t data2;
};