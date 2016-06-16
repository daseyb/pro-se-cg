#include <engine/core/Context.hpp>
#include <engine/core/System.hpp>
#include <engine/events/EventSystem.hpp>
#include <queue>
#include <thread>

#define OSCPKT_OSTREAM_OUTPUT
#include <engine/utils/oscpkt.hh>
#include <engine/utils/udp.hh>


class OscSystem : public System {
private:
    static const int CHANNEL_COUNT = 16;
    static const int VALUE_COUNT = 127;

    EventSystem* m_events;
    int m_port;
    oscpkt::UdpSocket m_socket;

    struct MessageInternal {
        oscpkt::Message oscMsg;
        oscpkt::SockAddr sender;
    };

    std::queue<MessageInternal> m_messageQueue;

    std::thread m_serverThread;

public:
    CONSTRUCT_SYSTEM(OscSystem, int port), m_port(port) {}

    void runServer();
    void update();

    bool startup() override;
    void shutdown() override;
};