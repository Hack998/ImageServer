#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>

#include "../include/image_proc.h"
#include "../include/config.h"
#include "../include/logger.h"

#define BACKLOG 10

struct thread_arg
{
    int client_fd;
    struct sockaddr_in client_addr;
};

static int running = 1;
static int sockfd = -1;

static void sigint_handler(int signo)
{
    (void)signo;
    running = 0;
    if (sockfd >= 0)
        close(sockfd);
}

static void *client_handler(void *arg)
{
    struct thread_arg *targ = (struct thread_arg *)arg;
    int fd = targ->client_fd;
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(targ->client_addr.sin_addr), ip, sizeof(ip));
    free(targ);

    // Header: name_len (uint32_t), name, file_size (uint64_t)
    uint32_t name_len_net;
    if (read(fd, &name_len_net, sizeof(name_len_net)) <= 0)
    {
        close(fd);
        return NULL;
    }
    uint32_t name_len = ntohl(name_len_net);

    char filename[256];
    if (name_len >= sizeof(filename))
        name_len = sizeof(filename) - 1;
    if (read(fd, filename, name_len) <= 0)
    {
        close(fd);
        return NULL;
    }
    filename[name_len] = '\0';

    uint64_t size_net;
    if (read(fd, &size_net, sizeof(size_net)) <= 0)
    {
        close(fd);
        return NULL;
    }
    uint64_t fsize = be64toh(size_net);

    if (fsize > 20 * 1024 * 1024)
    {
        (void)write(fd, "ERROR too large\n", 16);
        close(fd);
        return NULL;
    }

    (void)write(fd, "READY\n", 6);

    char *buf = malloc(fsize);
    if (!buf)
    {
        close(fd);
        return NULL;
    }

    size_t recvd = 0;
    while (recvd < fsize)
    {
        ssize_t r = read(fd, buf + recvd, fsize - recvd);
        if (r <= 0)
        {
            free(buf);
            close(fd);
            return NULL;
        }
        recvd += r;
    }

    // Procesar imagen
    char category[16];
    char outpath[512];
    int ok = process_image(buf, fsize, filename, category, sizeof(category), outpath, sizeof(outpath));
    free(buf);

    if (ok == 0)
    {
        char reply[600];
        snprintf(reply, sizeof(reply), "OK %s %s\n", category, outpath);
        (void)write(fd, reply, strlen(reply));
        log_request(ip, filename, fsize, category, outpath, 1);
    }
    else
    {
        (void)write(fd, "ERROR processing\n", 17);
        log_request(ip, filename, fsize, "-", "-", 0);
    }

    close(fd);
    return NULL;
}

int main(int argc, char *argv[])
{
    struct config cfg;
    read_config("/etc/server/config.conf", &cfg);

    // Preparar directorios base
    mkdir("/var/lib/image_server", 0755);
    mkdir("/var/lib/image_server/output", 0755);
    mkdir("/var/lib/image_server/output/rojo", 0755);
    mkdir("/var/lib/image_server/output/verde", 0755);
    mkdir("/var/lib/image_server/output/azul", 0755);

    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigint_handler);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(1);
    }

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in servaddr = {0};
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(cfg.port);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) < 0)
    {
        perror("listen");
        exit(1);
    }

    printf("Server listening on port %d...\n", cfg.port);

    while (running)
    {
        struct sockaddr_in cliaddr;
        socklen_t len = sizeof(cliaddr);
        int cfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
        if (cfd < 0)
        {
            if (!running)
                break;
            if (errno == EINTR)
                continue;
            perror("accept");
            continue;
        }

        pthread_t tid;
        struct thread_arg *targ = malloc(sizeof(*targ));
        targ->client_fd = cfd;
        targ->client_addr = cliaddr;
        pthread_create(&tid, NULL, client_handler, targ);
        pthread_detach(tid);
    }

    if (sockfd >= 0)
        close(sockfd);
    printf("Server shutting down.\n");
    return 0;
}
