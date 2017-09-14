
#include <utility.hpp>

#include <cstring>
#include <cerrno>
#include <cstdio>
#include <cstdlib>

constexpr uint16_t min_port = 1024;
constexpr uint16_t max_port = 64000;



void print_errno_message(const char* func) {
    int error = errno;
    char* error_text = strerror(error);
    fprintf(stderr, "Error in %s(): %s\n", func, error_text);
}

void _check_port(const char* name, uint16_t port) {
      if (port <= min_port || port >= max_port) {
        fprintf(stderr, "%s must be between %hu and %hu\n", name, min_port, max_port);
        exit(1);
    }

}

void _check_sock(const char* name, int sock) {
    if (sock == -1) {
        fprintf(stderr, "Error opening socket %s\n", name);
    }
}
