
#include <cstdio>

#include <sys/socket.h>

#include <packet.hpp>


int main(int argc, char* argv[]) {

    if (argc < 2) {
        //TODO: Usage
    }

    uint16_t port = atoi(argv[1]);

    printf("Hello from sender\n");
    int sock = open_listen_socket(port);

    if (sock == -1) {
        fprintf(stderr, "Failed to open socket\n");
        exit(-1);
    }
    else {
        printf("Listening on %hu\n", port);
    }

    char buffer[1025];
    buffer[1024] = 0;

    while (true) {
        recv(sock, buffer, 1024, 0);
        printf("%s", buffer);

    }
}
