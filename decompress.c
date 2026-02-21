/* Include any necessary libraries and header files */
#include "decompress.h"

#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "constants.h"

int decompress(const char *input_file_name)
{
    if (strstr(input_file_name, FILE_EXTENSION_DECOMPRESS) == NULL) {
        printf("Invalid extension. Use %s\n",FILE_EXTENSION_DECOMPRESS);
        return 1;
    }

    readFile(input_file_name,FILE_EXTENSION_DECOMPRESS);

    return 0;
}
