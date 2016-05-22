#pragma once
#include <unordered_map>
#include <map>
#include <vector>

#include <engine/core/Context.hpp>
#include <engine/core/System.hpp>
#include <engine/utils/StringHash.hpp>
#include <engine/events/Event.hpp>
#include <memory>

#include <functional>

typedef unsigned long EventSubscriberId;

class EventSystem : public System {
private:
  struct EventCallbackWrapperBase {
    virtual void operator()(const void * /*event*/) {}
  };

  template <typename E>
  struct EventCallbackWrapper : public EventCallbackWrapperBase {
    explicit EventCallbackWrapper(std::function<void(const E &)> callback) : callback(callback) {}
    void operator()(const void *event) override { callback(*(static_cast<const E*>(event))); }
    std::function<void(const E &)> callback;
  };


  typedef std::function<void(const void*)> EventCallback;
  typedef std::map< EventSubscriberId, std::shared_ptr<EventCallbackWrapperBase> > EventMap;


  std::unordered_map< BaseEvent::EventId, EventMap> m_registeredCallbacks;
  EventSubscriberId m_nextId;
public:
  CONSTRUCT_SYSTEM(EventSystem), m_nextId(0) { }

  template<typename E, typename Callback>
  inline EventSubscriberId subscribe(Callback callback, int priorityOffset = 0) {
    auto eventId = Event<E>::id();
    auto callbacksIt = m_registeredCallbacks.find(eventId);
    EventSubscriberId resultId = m_nextId++ + priorityOffset*10;
    if (callbacksIt == m_registeredCallbacks.end()) {
      m_registeredCallbacks[eventId] = EventMap();
    }

    m_registeredCallbacks[eventId][resultId] = std::make_shared<EventCallbackWrapper<E>>(callback);
    return resultId;
  }

  template<StringHash hash, typename Callback>
  inline EventSubscriberId subscribe(Callback callback, int priorityOffset = 0) {
    return subscribe<SEvent<hash>>([callback](const SEvent<hash>& /*e*/) { callback(); }, priorityOffset);
  }

  template<typename E>
  inline void unsubscribe(EventSubscriberId id) {
    auto eventId = Event<E>::id();
    auto callbacksIt = m_registeredCallbacks.find(eventId);
    if (callbacksIt == m_registeredCallbacks.end()) {
      return;
    }
    callbacksIt->second.erase(id);
  }

  template<StringHash hash>
  void unsubscribe(EventSubscriberId id) {
    unsubscribe<SEvent<hash>>(id);
  }

  template<typename E>
  inline void fire(E data) {
    auto eventId = Event<E>::id();
    auto callbacksIt = m_registeredCallbacks.find(eventId);
    if (callbacksIt == m_registeredCallbacks.end()) {
      return;
    }

    for (auto callback : callbacksIt->second) {
      (*dynamic_cast<EventCallbackWrapper<E>*>(callback.second.get()))(&data);
    }
  }

  template<StringHash hash>
  inline void fire() {
    fire<SEvent<hash>>(SEvent<hash>());
  }

  bool startup() override;
  void shutdown() override;
};
