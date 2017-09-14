#include <packet.hpp>

#include <cinttypes>
#include <cstdlib>
#include <cerrno>
#include <cstdio>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <utility.hpp>

int send(uint8_t* data, size_t length) {
    PacketHeader header __attribute__((unused));
    header = {};
    header.magic_number = 0x497e;
    header.type = DATA_PACKET;
    header.data_length = length;
    return 0;
}


// Opens a socket for sending data to a remote host. If local_port is non zero the socket
// will be bound to a local port also.
int open_send_socket(const char* address, uint16_t remote_port, uint16_t local_port) {
    sockaddr_in remote_address = {};
    remote_address.sin_family = AF_INET;
    remote_address.sin_port = htons(remote_port);
    inet_aton(address, &remote_address.sin_addr);

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sock == -1) {
        print_errno_message("socket");
        return -1;
    }
   
    if (local_port != 0) {
        sockaddr_in local_address = {};
        local_address.sin_family = AF_INET;
        local_address.sin_port = htons(local_port);
        local_address.sin_addr.s_addr = INADDR_ANY;

        bind(sock, (sockaddr*)&local_address, sizeof(local_address));
    }

    int result = connect(sock, (sockaddr*)&remote_address, sizeof(remote_address));
    
    if (result == -1) {
        //print_errno_message("connect");
        //close(sock);
        //return -1;
    }

    return sock;
}

int open_listen_socket(uint16_t port) {
    sockaddr_in local_address = {};
    local_address.sin_family = AF_INET;
    local_address.sin_port = htons(port);
    local_address.sin_addr.s_addr = INADDR_ANY;

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1) {
        print_errno_message("socket");
        return -1;
    }

    int result = bind(sock, (sockaddr*)&local_address, sizeof(local_address));

    if (result == -1) {
        print_errno_message("bind");
        close(sock);
        return -1;
    }

    result = listen(sock, 5);

    if (result == -1) {
        print_errno_message("listen");
        close(sock);
        return -1;
    }

    return sock;
}
