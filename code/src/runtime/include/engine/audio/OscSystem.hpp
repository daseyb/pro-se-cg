#include <engine/core/Context.hpp>
#include <engine/core/System.hpp>
#include <engine/events/EventSystem.hpp>

#define OSCPKT_OSTREAM_OUTPUT
#include <engine/utils/oscpkt.hh>
#include <engine/utils/udp.hh>


#include <engine/audio/OscEvent.hpp>
#include <mutex>
#include <queue>
#include <thread>



class OscSystem : public System {
private:
  EventSystem *m_events;
  int m_port;
  oscpkt::UdpSocket m_socket;

  struct MessageInternal {
    oscpkt::Message oscMsg;
    oscpkt::SockAddr sender;
  };

  std::queue<MessageInternal> m_messageQueue;

  std::thread m_serverThread;
  std::mutex m_messageMutex;

public:
  CONSTRUCT_SYSTEM(OscSystem, int port), m_port(port) {}

  void runServer();
  void update();

  bool startup() override;
  void shutdown() override;
};