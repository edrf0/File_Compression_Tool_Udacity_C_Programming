/* Include any necessary libraries and header files */
#include "compress.h"

#include <stdio.h>
#include <string.h>

#include "utils.h"

int compress(const char *input_file_name)
{
    if (strstr(input_file_name, ".txt") == NULL) {
        printf("Invalid extension. Use .txt\n");
        return 1;
    }

    readFile(input_file_name,".txt");

    return 0;
}
