#include <engine/core/Context.hpp>
#include <engine/core/System.hpp>
#include <engine/utils/Remotery.h>


class ProfilerSystem : public System {
private:
    Remotery* m_instance;
public:

    CONSTRUCT_SYSTEM(ProfilerSystem) {

    }

    virtual bool startup() override;
    virtual void shutdown() override;
};