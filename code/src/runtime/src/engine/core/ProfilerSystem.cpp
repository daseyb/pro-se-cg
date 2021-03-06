#include <engine/core/ProfilerSystem.hpp>

bool ProfilerSystem::startup() {
    rmt_CreateGlobalInstance(&m_instance);
    rmt_BindOpenGL();

    return true;
}

void ProfilerSystem::shutdown() {
    rmt_UnbindOpenGL();
    rmt_DestroyGlobalInstance(m_instance);
}
