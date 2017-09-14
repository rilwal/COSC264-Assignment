
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <packet.hpp>
#include <utility.hpp>

const char* remote_ip = "127.0.0.1";

constexpr uint16_t min_port = 1024;
constexpr uint16_t max_port = 64000;


bool check_packet(uint8_t* packet)
{
    PacketHeader header = *(PacketHeader*)packet;
//    uint8_t* data = packet + sizeof(PacketHeader);

    return header.magic_number == 0x497e;
}


double random_float()
{
    return (double)rand() / (double)RAND_MAX ;
}


int main(int argc, char* argv[])
{

    //process args
    if(argc < 7) {
        fprintf(stderr, "Usage: %s <port_csin> <port_csout> <port_crin> <port_crout> <port_sin> <port_rin> <precision>\n", argv[0]);
        exit(1);
    }

    uint16_t port_csin  = atoi(argv[1]),
             port_csout = atoi(argv[2]),
             port_crin = atoi(argv[3]),
             port_crout = atoi(argv[4]),
             port_sin   = atoi(argv[5]),
             port_rin   = atoi(argv[6]);

    double precision = atof(argv[7]);

    check_port(port_csin);
    check_port(port_csout);
    check_port(port_crin);
    check_port(port_crout);
    check_port(port_sin);
    check_port(port_rin);

    if(precision < 0.0 || precision > 1.0) {
        fprintf(stderr, "Precision must be between 0.0 and 1.0\n");
        exit(1);
    }

    //open / bind / connect sockets
    int sock_csin  = open_listen_socket(port_csin),
        sock_crin  = open_listen_socket(port_crin),
        sock_csout = open_send_socket(remote_ip, port_sin, port_csout),
        sock_crout = open_send_socket(remote_ip, port_rin, port_crout);

    check_sock(sock_csin);
    check_sock(sock_crin);
    check_sock(sock_csout);
    check_sock(sock_crout);

    int sock_csin_client = accept(sock_csin, 0, 0);
    int sock_crin_client = accept(sock_crin, 0, 0);

    // Set up structures for select
    fd_set file_descriptors;

    while(true) {
        printf("Listening\n");

        FD_ZERO(&file_descriptors);
        FD_SET(sock_csin_client, &file_descriptors);
        FD_SET(sock_crin_client, &file_descriptors);

        if(select(max(sock_csin_client, sock_crin_client)+1, &file_descriptors, 0, 0, 0) != 0) {
            printf("Select triggered\n");
            PacketHeader header = {};

            if(FD_ISSET(sock_csin_client, &file_descriptors)) {
                int64_t bytes_read = recv(sock_csin_client, (void*)&header, sizeof(PacketHeader), 0);
                if (bytes_read == -1) {
                    print_errno_message("recv");
                } else if (bytes_read == 0) {
                    printf("Sender disconnected, attempting to reestablish\n");
                    sock_csin_client = accept(sock_csin, 0, 0);
                } else if (bytes_read > 0) {
                    if(header.magic_number == 0x497e) {
                        uint64_t total_length = sizeof(PacketHeader) + header.data_length;
                        uint8_t* buffer = new uint8_t[total_length];

                        memcpy((void*)buffer, (void*)&header, sizeof(PacketHeader));
                        recv(sock_csin_client, (void*)(buffer + sizeof(PacketHeader)), header.data_length, 0);

                        //drop packet?
                        if(random_float() < precision) {
                            continue;
                        }

                        if (random_float() < 0.1) {
                            ((PacketHeader*)buffer)->data_length += rand() % 10 + 1;
                        }

                        send(sock_crout, (void*)buffer, total_length, 0);

                        delete[] buffer;
                    }
                }
            }

            if(FD_ISSET(sock_crin_client, &file_descriptors)) {
                int64_t bytes_read = recv(sock_crin_client, (void*)&header, sizeof(PacketHeader), 0);
                if (bytes_read == -1) {
                    print_errno_message("recv");
                } else if (bytes_read == 0) {
                    printf("Reciever disconnected, attempting to reestablish\n");
                    sock_crin_client = accept(sock_crin, 0, 0);
                } else if(bytes_read > 0) {

                    if(header.magic_number == 0x497e) {
                        uint64_t total_length = sizeof(PacketHeader) + header.data_length;
                        uint8_t* buffer = new uint8_t[total_length];

                        memcpy((void*)buffer, (void*)&header, sizeof(PacketHeader));
                        recv(sock_csin_client, (void*)(buffer + sizeof(PacketHeader)), header.data_length, 0);

                        //drop packet?
                        if(random_float() < precision) {
                            continue;
                        }

                        if (random_float() < 0.1) {
                            ((PacketHeader*)buffer)->data_length += rand() % 10 + 1;
                        }

                        send(sock_csout, (void*)buffer, total_length, 0);

                        delete[] buffer;
                    }

                }
            }

        }
    }


    close(sock_csin);
    close(sock_csout);
    close(sock_crin);
    close(sock_crout);
}
