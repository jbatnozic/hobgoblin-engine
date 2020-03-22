
#include <Hobgoblin/Utility/Packet.hpp>

#include <iostream>
#include <type_traits>
#include <utility>

class InputPacker {
public:
    InputPacker(hg::util::PacketBase& packet)
        : _packet{packet}
    {
    }

    template <class ...NoArgs>
    typename std::enable_if_t<sizeof...(NoArgs) == 0, void>  operator()() {
        // Do nothing
    }

    template <class ArgsHead, class ...ArgsRest>
    void operator()(ArgsHead&& argsHead, ArgsRest&&... argsRest) {
        _packet << std::forward<ArgsHead>(argsHead);
        (*this)(std::forward<ArgsRest>(argsRest)...);
    }

private:
    hg::util::PacketBase& _packet;
};

class OutputPacker {
public:
    OutputPacker(hg::util::PacketBase& packet)
        : _packet{packet}
    {
    }

    template <class ...NoArgs>
    typename std::enable_if_t<sizeof...(NoArgs) == 0, void>  operator()() {
        // Do nothing
    }

    template <class ArgsHead, class ...ArgsRest>
    void operator()(ArgsHead&& argsHead, ArgsRest&&... argsRest) {
        _packet >> std::forward<ArgsHead>(argsHead);
        (*this)(std::forward<ArgsRest>(argsRest)...);
    }

private:
    hg::util::PacketBase& _packet;
};

struct TEST {
    int a, b;

    template <class Packer>
    void pack(Packer& archiver) {
        archiver(a, b);
    }

    friend hg::util::PacketBase& operator<<(hg::util::PacketBase& packet, TEST& self) {
        InputPacker packer{packet};
        self.pack(packer);
        return packet;
    }

    friend hg::util::PacketBase& operator>>(hg::util::PacketBase& packet, TEST& self) {
        OutputPacker packer{packet};
        self.pack(packer);
        return packet;
    }

};

int main() {
    TEST t1{2, 5};

    hg::util::Packet packet;
    packet << t1;

    TEST t2 = packet.extractOrThrow<TEST>();
    std::cout << t2.a << ' ' << t2.b << '\n';

    return 0;
}