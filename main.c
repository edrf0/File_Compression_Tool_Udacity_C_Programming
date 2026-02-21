#include <stdio.h>
#include <string.h>

#include "compress.h"
#include "decompress.h"
#include "constants.h"

int main(const int argc, const char *argv[]){
    // argument count is 1 meaning no flags or 2+ arguments with the second being the -h flag
    if (argc == 1 || strcmp(argv[1], "-h") == 0) {
        printf("Usage:\n");
        printf("  Compress -c <path>\n");
        printf("  Decompress: -d <path>\n");
        printf("  Help: -h\n");
        return SUCCESS;
    }
    // argument count is 2 which necessarily fails due to lacking a file path or being unknown
    if (argc == 2) {
        fprintf(stderr, "Error: Missing file path argument.\n");
        return ERROR_MISSING_ARGUMENT;
    }
    // argument count is 3 which is ideal, next checks will confirm if flag is known
    if (argc == 3) {
        // checking if flag is for compress
        if (strcmp(argv[1], "-c") == 0) {
            printf("Mode: Compression\n");
            printf("Target File: %s\n", argv[2]);
            return compress(argv[2]);
        }
        // checking for decompress flag
        if (strcmp(argv[1], "-d") == 0) {
            printf("Mode: Decompression\n");
            printf("Target File: %s\n", argv[2]);
            return decompress(argv[2]);
        }
        // argument count is right but flag is unknown
        fprintf(stderr, "Error: Unknown flag '%s'. Use -h for help.\n", argv[1]);
        return ERROR_UNKNOWN_FLAG;
    }
    // incorrect number of arguments
    fprintf(stderr, "Error: Too many arguments Use -h for help.\n");
    return ERROR_ARGUMENT_NUMBER_EXCEEDED;
}
