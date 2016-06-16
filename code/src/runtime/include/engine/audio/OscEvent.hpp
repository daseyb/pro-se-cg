#pragma once
#define OSCPKT_OSTREAM_OUTPUT
#include <engine/events/Event.hpp>
#include <engine/utils/oscpkt.hh>

struct OscEvent : public Event<OscEvent> {
    OscEvent(oscpkt::Message msg) : msg(msg) {};
    oscpkt::Message msg;
};