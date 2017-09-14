
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <packet.hpp>
#include <utility.hpp>

constexpr uint16_t min_port = 1024;
constexpr uint16_t max_port = 64000;

const char * remote_ip = "127.0.0.1";

int main(int argc, char* argv[]) {

    // Usage:
    if (argc < 5) {
        fprintf(stderr, "Usage: %s <port_sin> <port_sout> <port_csin> <filename>\n", argv[0]);
        fprintf(stderr, "ports should be between %hu and %hu\n", min_port, max_port);
        exit(1);
    }

    uint16_t port_sin  = atoi(argv[1]),
             port_sout = atoi(argv[2]),
             port_csin = atoi(argv[3]);
    
    check_port(port_sin);
    check_port(port_sout);
    check_port(port_csin);

    const char* filename = argv[4];
    FILE* file = fopen(filename, "rb");
    
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        exit(1);
    }

    int socket_sin  = open_listen_socket(port_sin),
        socket_sout = open_send_socket(remote_ip, port_csin, port_sout);

    check_sock(socket_sin);
    check_sock(socket_sout);

    // Set up structures for select
    fd_set file_descriptors;
    FD_ZERO(&file_descriptors);
    FD_SET(socket_sin, &file_descriptors);

    timeval timeout = {};
    timeout.tv_usec = 0;
    timeout.tv_sec = 1;

    uint8_t buffer[512];
    uint8_t packetBuffer[sizeof(PacketHeader) + 512];

    int32_t next = 0;
    uint32_t packets_sent = 0;
    bool exit = false;


    int sock_client = accept(socket_sin, 0, 0);

    while (!exit) {
        size_t bytes_read = fread(buffer, 1, 512, file);

        PacketHeader p = {};

        if (bytes_read == 0) {
           p.magic_number = 0x497E;
           p.type = DATA_PACKET;
           p.seq_number = next;
           p.data_length = 0;

           memcpy((void*)packetBuffer, (void*)&p, sizeof(PacketHeader));

           exit = true;
        } else {
            p.magic_number = 0x497E;
            p.type = DATA_PACKET;
            p.seq_number = next;
            p.data_length = bytes_read;

           memcpy((void*)packetBuffer, (void*)&p, sizeof(PacketHeader));
           memcpy((void*)(packetBuffer + sizeof(PacketHeader)), buffer, bytes_read);
        }

        while(true) {
            // Transmission loop
            packets_sent++;
            timeout.tv_sec = 1;
            send(socket_sout, packetBuffer, sizeof(PacketHeader) + ((PacketHeader*)packetBuffer)->data_length, 0);

            // Use select to wait for a response
            if(select(socket_sin + 1, &file_descriptors, 0, 0, &timeout) != 0) {
                PacketHeader recvd;
                recv(sock_client, (void*)&recvd, sizeof(PacketHeader), 0);
                if (recvd.magic_number !=0x497E || recvd.type != ACK_PACKET || recvd.data_length != 0 || recvd.seq_number != next) {
                    continue;
                }
               
                next = 1 - next;
                break;
            }
        }
    }
    

    printf("Total packets sent: %u\n", packets_sent);

    fclose(file);
    close(socket_sin);
    close(socket_sout);
}
