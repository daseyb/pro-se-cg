#pragma once
#include <stddef.h>
#include <engine/utils/StringHash.hpp>

// This is a neat trick to easily get numbered type ids
// Since there is one instantiation for each event class that derives from Event
// id() returns a different value for each.
class BaseEvent {
public:
  typedef size_t EventId;
  virtual ~BaseEvent();
protected:
  static EventId s_idCounter;
};

template<typename Derived>
class Event : public BaseEvent {
public:
  static EventId id() {
    static EventId id = s_idCounter++;
    return id;
  }
};


template<StringHash hash>
struct SEvent : public Event<SEvent<hash>> {
  const StringHash hash_value = hash;
};