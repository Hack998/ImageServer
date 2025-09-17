#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/config.h"

void read_config(const char *path, struct config *cfg)
{
    cfg->port = 1717;
    FILE *f = fopen(path, "r");
    if (!f)
        return;
    char line[256];
    while (fgets(line, sizeof(line), f))
    {
        if (line[0] == '#' || strlen(line) < 3)
            continue;
        char *eq = strchr(line, '=');
        if (!eq)
            continue;
        *eq = '\0';
        char *key = line;
        char *val = eq + 1;
        if (strcmp(key, "PORT") == 0)
            cfg->port = atoi(val);
    }
    fclose(f);
}