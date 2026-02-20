/* Include any necessary libraries and header files */
#include "decompress.h"

#include <stdio.h>
#include <string.h>

#include "utils.h"

int decompress(const char *input_file_name)
{
    if (strstr(input_file_name, ".rle") == NULL) {
        printf("Invalid extension. Use .rle\n");
        return 1;
    }

    readFile(input_file_name,".rle");

    return 0;
}
