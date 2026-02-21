#ifndef UTILS_H
#define UTILS_H
#include "constants.h"

int statusReportForUser(ErrorCode error_code,const char *input_file_name,const char* extension);
int readFile(const char *input_file_name,const char *extension);
char *compressLogic(const char *fileText,long fileSize);
char *decompressLogic(const char *fileText,long fileSize);

#endif