#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_SIZE 4096

static int send_file(const char *ip, int port, const char *filename)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        return -1;
    }

    struct sockaddr_in servaddr = {0};
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0)
    {
        perror("inet_pton");
        close(sockfd);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        close(sockfd);
        return -1;
    }

    // Obtener tamaño del archivo
    struct stat st;
    if (stat(filename, &st) < 0)
    {
        perror("stat");
        close(sockfd);
        return -1;
    }
    uint64_t filesize = st.st_size;

    // Enviar nombre
    uint32_t name_len = strlen(filename);
    uint32_t name_len_net = htonl(name_len);
    if (write(sockfd, &name_len_net, sizeof(name_len_net)) != sizeof(name_len_net) ||
        write(sockfd, filename, name_len) != (ssize_t)name_len)
    {
        perror("write filename");
        close(sockfd);
        return -1;
    }

    // Enviar tamaño
    uint64_t size_net = htobe64(filesize);
    if (write(sockfd, &size_net, sizeof(size_net)) != sizeof(size_net))
    {
        perror("write size");
        close(sockfd);
        return -1;
    }

    // Esperar READY
    char resp[64];
    int n = read(sockfd, resp, sizeof(resp) - 1);
    if (n <= 0)
    {
        perror("read READY");
        close(sockfd);
        return -1;
    }
    resp[n] = '\0';
    if (strncmp(resp, "READY", 5) != 0)
    {
        fprintf(stderr, "Server error: %s\n", resp);
        close(sockfd);
        return -1;
    }

    // Enviar archivo
    int fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        perror("open file");
        close(sockfd);
        return -1;
    }

    char buf[BUF_SIZE];
    ssize_t r;
    while ((r = read(fd, buf, sizeof(buf))) > 0)
    {
        if (write(sockfd, buf, r) != r)
        {
            perror("write file");
            close(fd);
            close(sockfd);
            return -1;
        }
    }
    close(fd);

    // Leer respuesta final
    n = read(sockfd, resp, sizeof(resp) - 1);
    if (n > 0)
    {
        resp[n] = '\0';
        printf("Server reply: %s\n", resp);
    }

    close(sockfd);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Uso: %s <server_ip> <port>\n", argv[0]);
        return 1;
    }

    const char *server_ip = argv[1];
    int port = atoi(argv[2]);

    char input[512];
    while (1)
    {
        printf("Enter image path (or Exit to quit): ");
        fflush(stdout);

        if (!fgets(input, sizeof(input), stdin))
            break;

        // Eliminar salto de línea
        input[strcspn(input, "\n")] = '\0';

        if (strcasecmp(input, "Exit") == 0)
        {
            printf("Exiting client.\n");
            break;
        }

        if (strlen(input) == 0)
            continue;

        send_file(server_ip, port, input);
    }

    return 0;
}
