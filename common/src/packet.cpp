#include <packet.hpp>
#include <cinttypes>
#include <cstdlib>

int send(uint8_t* data, size_t length) {
    PacketHeader header __attribute__((unused));
    header = {};
    header.magic_number = 123;
    return 0;
}


