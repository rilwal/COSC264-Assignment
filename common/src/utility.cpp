
#include <utility.hpp>

#include <cstring>
#include <cerrno>
#include <cstdio>

void print_errno_message(const char* func) {
    int error = errno;
    char* error_text = strerror(error);
    fprintf(stderr, "Error in %s(): %s\n", func, error_text);
}
