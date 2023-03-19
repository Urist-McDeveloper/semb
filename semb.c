/*  semb -- simple embedded files for CMake projects
 *
 *  (C) 2023 Urist McDeveloper
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty.  In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *      1. The origin of this software must not be misrepresented; you must not
 *         claim that you wrote the original software. If you use this software
 *         in a product, an acknowledgment in the product documentation would be
 *         appreciated but is not required.
 *      2. Altered source versions must be plainly marked as such, and must not be
 *         misrepresented as being the original software.
 *      3. This notice may not be removed or altered from any source distribution.
 */

#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#   define _CRT_SECURE_NO_DEPRECATE
#endif

#define MAX_FILES           255
#define MAX_NAME_LENGTH     255
#define BUFFER_SIZE         4096
#define MAX_PER_LINE        64

struct file_data {
    int file_name_length;
    int data_name_length;
    char file_name[MAX_NAME_LENGTH + 1];
    char data_name[MAX_NAME_LENGTH + 1];
};

static int files_count;
static struct file_data files[MAX_FILES];

static char wr_buf[BUFFER_SIZE];
static int wr_used;

static void flush() {
    int wrote = (int)fwrite(wr_buf, 1, wr_used, stdout);
    if (wrote != wr_used) {
        if (feof(stdout)) {
            perror("STDOUT is closed");
        } else if (ferror(stdout)) {
            perror("Failed to write to STDOUT");
        }
        exit(EXIT_FAILURE);
    } else {
        wr_used = 0;
    }
}

static void write(const char *str, int size) {
    int i = 0;
    while (i != size) {
        if (wr_used == BUFFER_SIZE) flush();
        wr_buf[wr_used++] = str[i++];
    }
}

#define DATA_DECL       "static const unsigned char "
#define DATA_BGN        "[] = {\n"
#define DATA_PFX        "        "
#define DATA_END        "\n};\n"

#define write_str(STR)  write(STR, sizeof(STR) - 1)

static unsigned char rd_buf[BUFFER_SIZE];

static void write_file(struct file_data data) {
    FILE *f = fopen(data.file_name, "rb");
    if (f == NULL) {
        perror("Failed to open file");
        fprintf(stderr, "Failed to open file: %s\n", data.file_name);
        exit(EXIT_FAILURE);
    }

    write_str(DATA_DECL);
    write(data.data_name, data.data_name_length);
    write_str(DATA_BGN);

    int i, idx = 0, got;
    char num[5] = "0x00,";

    do {
        got = (int)fread(rd_buf, 1, BUFFER_SIZE, f);
        for (i = 0; i < got; i++) {
            if (idx == MAX_PER_LINE) {
                write("\n", 1);
                idx = 0;
            }
            if (idx++ == 0) {
                write_str(DATA_PFX);
            }
            num[2] = "0123456789abcdef"[rd_buf[i] / 16];
            num[3] = "0123456789abcdef"[rd_buf[i] % 16];
            write(num, 5);
        }
    } while (got == BUFFER_SIZE);

    if (ferror(f)) {
        perror("Failed to read file");
        fprintf(stderr, "Failed to read file: %s\n", data.file_name);
        exit(EXIT_FAILURE);
    } else {
        fclose(f);
    }
    write_str(DATA_END);
}

int main(int argc, char **argv) {
    int i, j;
    files_count = argc - 1;

    if (files_count < 1) {
        fprintf(stderr, "Must provide at least 1 argument\n");
        return EXIT_FAILURE;
    }
    if (files_count > MAX_FILES) {
        fprintf(stderr, "Must provide at most %d arguments\n", MAX_FILES);
        return EXIT_FAILURE;
    }

    for (i = 0; i < files_count; i++) {
        for (j = 0; j < MAX_NAME_LENGTH; j++) {
            char ch = argv[i + 1][j];
            if (ch == '\0') break;

            files[i].file_name[files[i].file_name_length++] = ch;
            if (ch == '/' || ch == '\\') {
                files[i].data_name_length = 0;
            } else {
                int is_valid = (ch >= 'a' && ch <= 'z') ||
                               (ch >= 'A' && ch <= 'Z') ||
                               (files[i].data_name_length != 0 && ch >= '0' && ch <= '1');
                if (is_valid) {
                    files[i].data_name[files[i].data_name_length++] = ch;
                } else {
                    files[i].data_name[files[i].data_name_length++] = '_';
                }
            }
        }
        if (j == MAX_NAME_LENGTH) {
            fprintf(stderr, "Argument %d is longer than maximum allowed %d characters\n", i + 1, MAX_NAME_LENGTH);
            return EXIT_FAILURE;
        }
        if (files[i].data_name_length == 0) {
            fprintf(stderr, "Argument %d resulted in zero-length identifier\n", i + 1);
            return EXIT_FAILURE;
        }
    }

    for (i = 0; i < files_count; i++) {
        write_file(files[i]);
    }
    flush();
    return EXIT_SUCCESS;
}
