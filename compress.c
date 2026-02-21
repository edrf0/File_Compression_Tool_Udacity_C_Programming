#include "compress.h"

#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "constants.h"

int compress(const char *input_file_name)
{
    if (strstr(input_file_name, FILE_EXTENSION_COMPRESS) == NULL) {
        printf("Invalid extension. Use %s\n",FILE_EXTENSION_COMPRESS);
        return 1;
    }

    readFile(input_file_name,FILE_EXTENSION_COMPRESS);

    return 0;
}
