#pragma once
#include <engine/core/PlatformMacros.hpp>

class Context;

#define CONSTRUCT_SYSTEM(system, ...) system(Context* context, ##__VA_ARGS__) : System(context->add(this)) 

class System {
  friend class Context;
protected:
  bool m_IsInitialized;
  Context* m_context;
public:
  System(Context* context) : m_IsInitialized(false), m_context(context) { };
  
  virtual bool startup() = 0;
  virtual void shutdown() = 0;
};