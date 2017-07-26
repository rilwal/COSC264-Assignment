#pragma once

struct PacketHeader {
    int magic_number;
    int type;
    int seq_number;
    int data_length;
};

