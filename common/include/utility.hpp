#pragma once

#include <cinttypes>

#define check_port(port) _check_port(#port, port)
#define check_sock(sock) _check_sock(#sock, sock)

#define max(a,b) (((a)>(b))?(a):(b))

void print_errno_message(const char* func);
void _check_port(const char* name, uint16_t port);
void _check_sock(const char* name, int sock);
