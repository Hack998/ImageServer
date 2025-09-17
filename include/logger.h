#ifndef LOGGER_H
#define LOGGER_H
void log_request(const char *ip, const char *file, long size,
                 const char *cat, const char *out, int ok);
#endif