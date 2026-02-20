#ifndef UTILS_H
#define UTILS_H

void readFile(const char *input_file_name,const char *extension);
char *compressLogic(const char *fileText,long fileSize);
char *decompressLogic(const char *fileText,long fileSize);

#endif