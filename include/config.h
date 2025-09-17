#ifndef CONFIG_H
#define CONFIG_H
struct config
{
    int port;
};
void read_config(const char *path, struct config *cfg);
#endif