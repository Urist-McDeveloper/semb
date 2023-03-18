#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#   define _CRT_SECURE_NO_DEPRECATE
#endif

#define MAX_FILES           128
#define MAX_NAME_LENGTH     128
#define BUFFER_SIZE         4096
#define MAX_NUM_PER_LINE    64

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
    int read = (int)fwrite(wr_buf, 1, wr_used, stdout);
    if (read != wr_used) {
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
#define DATA_PFX        "       "
#define DATA_END        "\n};\n"

#define write_str(STR)  write(STR, sizeof(STR) - 1)

static unsigned char rd_buf[BUFFER_SIZE];

static void write_file(struct file_data data) {
    FILE *f = fopen(data.file_name, "rb");
    if (f == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    write_str(DATA_DECL);
    write(data.data_name, data.data_name_length);
    write_str(DATA_BGN);

    int i, idx = 0, got;
    char num[6] = " 0x00,";

    do {
        got = (int)fread(rd_buf, 1, BUFFER_SIZE, f);
        for (i = 0; i < got; i++) {
            if (idx == MAX_NUM_PER_LINE) {
                write("\n", 1);
                idx = 0;
            }
            if (idx++ == 0) {
                write_str(DATA_PFX);
            }
            num[3] = "0123456789abcdef"[rd_buf[i] / 16];
            num[4] = "0123456789abcdef"[rd_buf[i] % 16];
            write(num, 6);
        }
    } while (got == BUFFER_SIZE);

    if (ferror(f)) {
        perror("Failed to read file");
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
            switch (ch) {
                case '/':
                case '\\':
                    files[i].data_name_length = 0;
                    break;
                case '.':
                case ' ':
                    files[i].data_name[files[i].data_name_length++] = '_';
                    break;
                default:
                    files[i].data_name[files[i].data_name_length++] = ch;
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
