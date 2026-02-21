#include "utils.h"
#include "constants.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int statusReportForUser(const ErrorCode error_code,const char *input_file_name,const char* extension) {
    if (error_code != SUCCESS) {
        if (strcmp(extension, FILE_EXTENSION_COMPRESS) == 0) {
            printf("Compression of %s failed.\n",input_file_name);
        } else {
            printf("Decompression of %s failed.\n",input_file_name);
        }
    } else {
        if (strcmp(extension, FILE_EXTENSION_COMPRESS) == 0) {
            printf("Successfully compressed %s.\n",input_file_name);
        } else {
            printf("Successfully decompressed %s.\n",input_file_name);
        }
    }
    return error_code;
}

int readFile(const char *input_file_name,const char *extension) {
    // getting file handle
    FILE *input = fopen(input_file_name, "r");
    // checking if file handle is valid
    if (input == NULL) {
        perror("Failed to open file");
        printf("File path: %s\n",input_file_name);
        return statusReportForUser(ERROR_FILE_NOT_FOUND,input_file_name,extension);
    }
    // get file size
    fseek(input, 0, SEEK_END);
    const long fileSize = ftell(input);
    fseek(input, 0, SEEK_SET);
    // edge case empty file
    if (fileSize <= 0) {
        printf("File is empty or invalid.\nFile path: %s\n",input_file_name);
        fclose(input);
        return statusReportForUser(ERROR_FILE_EMPTY,input_file_name,extension);
    }
    // dynamic memory allocation
    char *fileText = malloc(fileSize + 1);
    if (fileText == NULL) {
        perror("Failed to allocate memory");
        printf("File path: %s\n",input_file_name);
        fclose(input);
        return statusReportForUser(ERROR_MEMORY_ALLOCATION,input_file_name,extension);
    }
    // file reading
    const size_t bytesRead = fread(fileText, 1, fileSize, input);
    // closing file handle
    fclose(input);
    // checking if we effectively read all bytes
    if (bytesRead < (size_t) fileSize) {
        printf( "Error: Could not read entire file.\n");
        free(fileText);
        return statusReportForUser(ERROR_READ_FAILED,input_file_name,extension);
    }
    // terminating the file buffer with null character
    fileText[bytesRead] = '\0';
    // discriminating type of file and respective action
    char *data = NULL;
    if (strcmp(extension, FILE_EXTENSION_COMPRESS) == 0) {
        data = compressLogic(fileText,fileSize);
    } else {
        data = decompressLogic(fileText,fileSize);
    }
    // freeing file buffer
    free(fileText);
    // checking if data is null
    if (data == NULL) {
        printf("Error: Processing failed (Logic returned NULL).\n");
        return statusReportForUser(ERROR_LOGIC_FAILED,input_file_name,extension);
    }
    // determine the target output name
    char finalOutputPath[FILENAME_BUFFER_SIZE];
    char baseName[FILENAME_BUFFER_SIZE];
    // finding the extension dot in the input name using string reverse char function
    strcpy(baseName, input_file_name);
    char *dot = strrchr(baseName, '.');
    // chopping the string -> test.txt\0 -> test\0.txt or test
    if (dot != NULL) *dot = '\0';
    // assigning extension
    const char *newExt = (strcmp(extension, FILE_EXTENSION_COMPRESS) == 0) ?
                          FILE_EXTENSION_DECOMPRESS : FILE_EXTENSION_COMPRESS;
    // checking unversioned name first
    snprintf(finalOutputPath, sizeof(finalOutputPath), "%s%s", baseName, newExt);
    FILE *check = fopen(finalOutputPath, "r");
    if (check != NULL) {
        // it exists so we find a versioned name (test_1.rle, test_2.rle...)
        fclose(check);
        int version = 1;
        while (1) {
            snprintf(finalOutputPath, sizeof(finalOutputPath), "%s_%d%s",
                baseName, version, newExt);
            check = fopen(finalOutputPath, "r");
            // break if name is available
            if (check == NULL) break;
            fclose(check);
            version++;
        }
    }
    // getting a file handle to write
    FILE *output = fopen(finalOutputPath, "w");
    // if file handle is valid
    if (output != NULL) {
        // data is a null-terminated string, so we can use fputs or fwrite
        fputs(data, output);
        // saving the file
        fclose(output);
        // printing to the screen that file was saved
        printf("File saved as: %s\n", finalOutputPath);
        // freeing memory
        free(data);
        return statusReportForUser(SUCCESS,input_file_name,extension);
    }
    perror("Error: Could not open file for writing");
    // freeing memory
    free(data);
    return statusReportForUser(ERROR_WRITE_FAILED,input_file_name,extension);
}

char *compressLogic(const char *fileText,const long fileSize) {
    // allocating memory for compressed data
    char *compressedData = malloc(fileSize * 2 + 1);
    // checking if allocation failed
    if (compressedData == NULL) {
        perror("Failed to allocate compressed data buffer");
        return NULL;
    }
    // initializing the first byte so sprintf knows where string ends
    compressedData[0] = '\0';
    // establishing variables for logic
    int count = 1, cdIndex = 0;
    char currentChar = fileText[0];
    // iterating through the file text
    for (int i=1; i<fileSize; ++i) {
        //checking if the current character is distinct to the one saved
        if (currentChar != fileText[i]) {
            // concatenating current character and count plus updating tracking index
            cdIndex += sprintf(compressedData + cdIndex, "%c%d", currentChar, count);
            // updating current character
            currentChar = fileText[i];
            // resetting count
            count = 1;
        } else {
            // updating count
            ++count;
        }
    }
    // concatenating last character and count
    cdIndex += sprintf(compressedData + cdIndex, "%c%d", currentChar, count);
    return compressedData;
}

char *decompressLogic(const char *fileText, const long fileSize) {
    // for decompression, it is hard to guess the final size perfectly.
    // a safe starting point could possibly be around 4x the compressed size,
    const size_t outCapacity = fileSize * 4;
    // allocating memory
    char *decompressedData = malloc(outCapacity + 1);
    // checking if allocation failed
    if (decompressedData == NULL) {
        perror("Failed to allocate decompressed data buffer");
        return NULL;
    }
    // establishing variables
    int readIndex = 0,writeIndex = 0, count = 0, charsRead = 0;
    // iterating through the file text
    while (readIndex < fileSize) {
        // getting current character
        const char currentChar = fileText[readIndex];
        // updating readIndex
        ++readIndex;
        // resetting variables
        count = 0;
        charsRead = 0;

        if (readIndex < fileSize &&
            sscanf(fileText + readIndex, "%d%n", &count, &charsRead) == 1) {
            readIndex += charsRead;
            // expand the character
            for (int i = 0; i < count; ++i) {
                // safety check to prevent buffer overflow
                if (writeIndex < outCapacity) {
                    decompressedData[writeIndex++] = currentChar;
                }
            }
        } else {
            // if no number follows, we treat it as a single occurrence
            decompressedData[writeIndex++] = currentChar;
        }
    }
    // terminating the data with a null character
    decompressedData[writeIndex] = '\0';
    return decompressedData;
}
