#include <stdio.h>
#include <string.h>

#include "compress.h"
#include "decompress.h"

int main(const int argc, const char *argv[])
{
    if (argc == 1 || strcmp(argv[1], "-h") == 0) {
        printf("Usage:\n");
        printf("  Compress -c <path>\n");
        printf("  Decompress: -d <path>\n");
        printf("  Help: -h\n");
        return 0;
    }
    if (argc == 2) {
        fprintf(stderr, "Error: Missing file path argument.\n");
        return 1;
    }
    if (argc == 3) {
        if (strcmp(argv[1], "-c") == 0) {
            printf("Mode: Compression\n");
            printf("Target File: %s\n", argv[2]);
            compress(argv[2]);
        }
        else if (strcmp(argv[1], "-d") == 0) {
            printf("Mode: Decompression\n");
            printf("Target File: %s\n", argv[2]);
            decompress(argv[2]);
        }
    } else {
        fprintf(stderr, "Error: Unknown argument '%s'. Use -h for help.\n", argv[1]);
        return 1;
    }
    return 0;
}
