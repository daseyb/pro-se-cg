#pragma once
#include <unordered_map>
#include <vector>
#include <typeinfo>
#include <engine/core/System.hpp>

#define RESOLVE_DEPENDENCY(system) \
  system = m_context->get<std::decay<decltype(*system)>::type>(); \
  if (!system) {ACGL::Utils::error() << "Could not resolve dependency on system: " << #system \
                                     << " in system " << typeid(*this).name() << std::endl; return false; }

class Context {
public:
  template<typename T>
  T* get() {
    static_assert(std::is_base_of<System, T>::value, "You can only get systems from a context");
    
    auto hashCode = typeid(T).hash_code();
    auto index = m_registeredSystems.find(hashCode);
    if (index == m_registeredSystems.end()) {
      return nullptr;
    }

    auto system = m_registeredSystems[hashCode];
    if (!system->m_IsInitialized) {
      return nullptr;
    }

    return dynamic_cast<T*>(system);
  }

  template<typename T>
  Context* add(T* system) {
    static_assert(std::is_base_of<System, T>::value, "You can only add systems to a context");
    m_registeredSystems.insert({ typeid(T).hash_code(), system });
    m_startupList.push_back(system);
    return this;
  }

  bool startup();
  void shutdown();

private:
  std::unordered_map<size_t, System*> m_registeredSystems;
  std::vector<System*> m_startupList;
  int m_initializedSystemCount;
};