#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include "embed/test.h"

#ifdef _WIN32
#   define _CRT_SECURE_NO_DEPRECATE
#   define stat _stat
#endif

#define assert(X) if (!(X)) {fprintf(stderr, "Assertion failed: " #X); exit(1); }

static unsigned char buffer[1024 * 1024];

static void read_fully(const char *filename, unsigned int *size) {
    FILE *f = fopen(filename, "rb");
    assert(f != NULL);

    struct stat st;
    assert(stat(filename, &st) == 0);
    assert(st.st_size <= sizeof(buffer));
    assert(st.st_size == fread(buffer, 1, st.st_size, f));
    assert(ferror(f) == 0);

    fclose(f);
    *size = st.st_size;
}

void assert_file_eq(const char *filename, const unsigned char *buf, unsigned int len) {
    unsigned int size;
    read_fully(filename, &size);

    if (size != len) {
        printf("%s differs in length; expected %u, got %u\n", filename, len, size);
        return;
    }

    unsigned int i;
    for (i = 0; i < len; i++) {
        if (buf[i] != buffer[i]) {
            printf("%s differs at %u; expected %u, got %u\n", filename, i, buf[i], buffer[i]);
            return;
        }
    }
    printf("%s matches; size = %u\n", filename, len);
}

#define assert_eq(filename, buf)    assert_file_eq(filename, buf, sizeof(buf))

int main() {
    assert_eq("main.c", main_c);
    assert_eq("test.c", test_c);
    return 0;
}
