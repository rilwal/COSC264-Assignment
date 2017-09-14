#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <packet.hpp>
#include <utility.hpp>

const char* remote_ip = "127.0.0.1";

int main(int argc, char* argv[]) {
    
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <port_rin> <port_rout> <port_crin> <filename>", argv[0]);
        exit(1);
    }
    
    uint16_t port_rin  = atoi(argv[1]),
             port_rout = atoi(argv[2]),
             port_crin = atoi(argv[3]);

    check_port(port_rin);
    check_port(port_rout);
    check_port(port_crin);

    int sock_rin  = open_listen_socket(port_rin),
        sock_rout = open_send_socket(remote_ip, port_crin, port_rout);

    check_sock(sock_rin);
    check_sock(sock_rout);


    int client_sock_rin = accept(sock_rin, 0, 0);

    const char* filename = argv[4];
    FILE* file = fopen(filename, "rb");

    bool exit = false;
    int expected = 0;

    while (!exit) {
        PacketHeader header;
        uint64_t bytes_read = recv(client_sock_rin, (void*)&header, sizeof(PacketHeader), 0);

        if(bytes_read == sizeof(PacketHeader)) {
            if(header.magic_number == 0x497e && header.type == DATA_PACKET) {
                   
                    PacketHeader response = {};
                    response.magic_number = 0x497e;
                    response.data_length = 0;
                    response.type = ACK_PACKET;
                    response.seq_number = header.seq_number;

                    send(sock_rout, (void*)&response, sizeof(PacketHeader), 0);

                    if (header.seq_number == expected) {
                        expected = 1 - expected;
                        if (header.data_length > 0) {
                            uint8_t* buffer = new uint8_t[header.data_length];
                            recv(client_sock_rin, (void*)&header, header.data_length, 0);
                    
                            fwrite((void*)buffer, 1, header.data_length, file);

                            delete[] buffer;
                        }
                    } else {
                        exit = true;
                    }

            }
        }
    }

    close(sock_rin);
    close(sock_rout);

}
