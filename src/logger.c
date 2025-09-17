#include <stdio.h>
#include <time.h>
#include "../include/logger.h"

void log_request(const char *ip, const char *file, long size,
                 const char *cat, const char *out, int ok)
{
    FILE *f = fopen("/var/log/image_server.log", "a");
    if (!f)
        return;
    time_t now = time(NULL);
    char ts[64];
    strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));
    fprintf(f, "%s %s file=%s size=%ld -> %s path=%s status=%s\n",
            ts, ip, file, size, cat, out, ok ? "OK" : "FAIL");
    fclose(f);
}