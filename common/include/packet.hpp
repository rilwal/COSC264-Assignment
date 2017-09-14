#pragma once

#include <cinttypes>
#include <cstdlib> 

enum PacketType {
    DATA_PACKET,
    ACK_PACKET 
};

struct PacketHeader {
    int magic_number;
    PacketType type;
    int seq_number;
    int data_length;
};


int send(uint8_t* data, size_t length);
int open_send_socket(const char* address, uint16_t remote_port, uint16_t local_port=0);
int open_listen_socket(uint16_t port);

