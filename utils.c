#include "utils.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void readFile(const char *input_file_name,const char *extension) {
    // getting file handle
    FILE *input = fopen(input_file_name, "r");
    // checking if file handle is valid
    if (input == NULL) {
        printf("Error opening file\n");
        return;
    }
    // get file size
    fseek(input, 0, SEEK_END);
    const long fileSize = ftell(input);
    fseek(input, 0, SEEK_SET);

    // edge case
    if (fileSize == 0) {
        printf("File is empty\n");
        fclose(input);
        return;
    }

    // dynamic memory allocation
    char *fileText = malloc(fileSize + 1);
    if (fileText == NULL) {
        perror("Failed to allocate memory");
        fclose(input);
        return;
    }
    // file reading
    fread(fileText, 1, fileSize, input);
    fileText[fileSize] = '\0'; // null terminate

    // discriminating type of file and respective action
    char *data;
    if (strcmp(extension, ".txt") == 0) {
        data = compressLogic(fileText,fileSize);
    } else {
        data = decompressLogic(fileText,fileSize);
    }

    // closing the file handle
    fclose(input);
    // establishing the filename
    const char *outFileName = (strcmp(extension, ".txt") == 0) ? "output.rle" : "output.txt";
    // getting a file handle to write
    FILE *output = fopen(outFileName, "w");
    // if file handle is valid
    if (output != NULL) {
        // data is a null-terminated string, so we can use fputs or fwrite
        fputs(data, output);
        // saving the file
        fclose(output);
        // printing to the screen that file was saved
        printf("File saved as: %s\n", outFileName);
    }
    // freeing memory
    free(fileText);
    free(data);
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
            // we only concatenate if the current character is not a whitespace
            if (currentChar != ' ' && currentChar != '\n' && currentChar != '\t') {
                // concatenating current character and count plus updating tracking index
                cdIndex += sprintf(compressedData + cdIndex, "%c%d", currentChar, count);
            } else {
                // concatenating only the whitespace character without its count
                cdIndex += sprintf(compressedData + cdIndex, "%c", currentChar);
            }
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
        perror("Failed to allocate memory");
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
