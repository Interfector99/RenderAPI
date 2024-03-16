#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>

int scan_file(FILE* stream, const char* format, ...);
size_t read_file(void* ptr, size_t size, size_t count, FILE* stream);

#endif // CONFIG_H
