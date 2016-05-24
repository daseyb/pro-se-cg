#include <engine/core/ProfilerSystem.hpp>
#include <engine/utils/Remotery.h>

bool ProfilerSystem::startup() {
    rmt_CreateGlobalInstance(&m_instance);
    rmt_BindOpenGL();

    return true;
}

void ProfilerSystem::shutdown() {
    rmt_UnbindOpenGL();
    rmt_DestroyGlobalInstance(m_instance);
}
