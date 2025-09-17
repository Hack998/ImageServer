#ifndef IMAGE_PROC_H
#define IMAGE_PROC_H
#include <stddef.h>

int process_image(const char *data, size_t size, const char *filename,
                  char *category, size_t clen, char *outpath, size_t olen);

#endif