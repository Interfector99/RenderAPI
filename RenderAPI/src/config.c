#include "config.h"
#include <stdarg.h>

int scan_file(FILE* stream, const char* format, ...) 
{
    int result;
    va_list args;
    va_start(args, format);
#ifdef __linux__
    result = vfscanf(stream, format, args);
#else
    result = vfscanf(stream, format, args);
#endif
    va_end(args);

    return result;
}

size_t read_file(void* ptr, size_t size, size_t count, FILE* stream) 
{
#ifdef __linux__
    return fread(ptr, size, count, stream);
#else
    return fread(ptr, size, count, stream);
#endif
}