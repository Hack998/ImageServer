#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

static void send_file(const char *server, int port, const char *path)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, server, &addr.sin_addr);
    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        return;
    }

    struct stat st;
    if (stat(path, &st) < 0)
    {
        perror("stat");
        close(fd);
        return;
    }
    uint32_t nlen = htonl(strlen(path));
    uint64_t fsize = htobe64(st.st_size);

    write(fd, &nlen, 4);
    write(fd, path, strlen(path));
    write(fd, &fsize, 8);

    char buf[64];
    int r = read(fd, buf, sizeof(buf) - 1);
    buf[r] = '\0';
    if (strncmp(buf, "READY", 5) != 0)
    {
        printf("Server error: %s\n", buf);
        close(fd);
        return;
    }

    int infd = open(path, O_RDONLY);
    char block[4096];
    ssize_t n;
    while ((n = read(infd, block, sizeof(block))) > 0)
    {
        write(fd, block, n);
    }
    close(infd);

    r = read(fd, buf, sizeof(buf) - 1);
    if (r > 0)
    {
        buf[r] = '\0';
        printf("Response: %s\n", buf);
    }
    else
        printf("No response.\n");

    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Usage: %s <server> <port> <image1> [image2..]\n", argv[0]);
        return 1;
    }
    char *server = argv[1];
    int port = atoi(argv[2]);
    for (int i = 3; i < argc; i++)
    {
        send_file(server, port, argv[i]);
    }
    return 0;
}