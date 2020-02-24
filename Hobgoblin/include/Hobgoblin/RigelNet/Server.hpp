#ifndef UHOBGOBLIN_RN_SERVER_HPP
#define UHOBGOBLIN_RN_SERVER_HPP

#include <Hobgoblin/Common.hpp>

#include <chrono>
#include <cstdint>
#include <string>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_START
namespace rn {

// TODO constexpr Any port

struct RN_RemoteInfo;

template <class T>
class RN_Server {
public:
#if 0
    bool start(std::uint16_t port, std::string passphrase);
    void stop();
    bool isRunning() const;
    void update();
    void updateWithoutUpload();

    // Client management:
    RN_RemoteInfo& getClientInfo(PZInteger index);
    void swapClients(PZInteger index1, PZInteger index2);

    // Utility:
    int getSenderIndex() const;
    std::uint16_t getLocalPort() const;
    PZInteger getSize() const;
    void resize(PZInteger newSize);

    std::chrono::microseconds getTimeoutLimit() const;
    void setTimeoutLimit(std::chrono::microseconds limit);

    const std::string& getPassphrase() const;
#endif
};

} // namespace rn
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_undef.hpp>

#endif // !UHOBGOBLIN_RN_SERVER_HPP
