#include <engine/audio/OscSystem.hpp>
#include <engine/core/SimulateEvent.hpp>
#include <glow/common/log.hh>

using std::cout;
using std::cerr;
using namespace oscpkt;

void OscSystem::runServer() {
    m_socket.bindTo(m_port);

    if (!m_socket.isOk()) {
        glow::error() << "Error opening port " << m_port << ": " << m_socket.errorMessage() << "\n";
    } else {
        std::cout << "Server started, will listen to packets on port " << m_port << "\n";
        PacketReader pr;
        while (m_socket.isOk()) {
            if (m_socket.receiveNextPacket(30 /* timeout, in ms */)) {
                pr.init(m_socket.packetData(), m_socket.packetSize());
                oscpkt::Message *msg;
                while (pr.isOk() && (msg = pr.popMessage()) != 0) {
                    m_messageQueue.push({ *msg, m_socket.packetOrigin() });

                }
            }
        }
    }
}

bool OscSystem::startup() {
    RESOLVE_DEPENDENCY(m_events);

    m_events->subscribe<SimulateEvent>([&](const SimulateEvent &e) { update(); });

    m_serverThread = std::thread(&OscSystem::runServer, this);

    return true;
}

void OscSystem::update() {

    PacketWriter pw;

    while (!m_messageQueue.empty()) {
        MessageInternal internalMsg = m_messageQueue.front();
        m_messageQueue.pop();

        auto msg = internalMsg.oscMsg;

        int iarg;
        if (msg.match("/ping").popInt32(iarg).isOkNoMoreArgs()) {
            glow::debug() << "Server: received /ping " << iarg << " from " << internalMsg.sender << "\n";
            Message repl; repl.init("/pong").pushInt32(iarg + 1);
            pw.init().addMessage(repl);

            m_socket.sendPacketTo(pw.packetData(), pw.packetSize(), internalMsg.sender);
        } else {
            std::cout << "Server: unhandled message: " << msg << "\n";
        }
    }

}

void OscSystem::shutdown() {
    m_socket.close();
    m_serverThread.join();
}
