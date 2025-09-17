CC=gcc
CFLAGS=-Wall -O2 -pthread -Iinclude
SRCS_SERVER=src/server.c src/image_proc.c src/config.c src/logger.c
OBJS_SERVER=$(SRCS_SERVER:.c=.o)

all: server client

server: $(OBJS_SERVER)
	$(CC) $(CFLAGS) -o $@ $^ -lm

client: src/client.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJS_SERVER) server client

install: all
	@echo "[*] Ejecutando script de instalación..."
	@bash ./scripts/install.sh

uninstall:
	@echo "[*] Ejecutando script de desinstalación..."
	@bash ./scripts/uninstall.sh

purge:
	@echo "[*] Ejecutando script de PURGE (limpieza total)..."
	@bash ./scripts/purge.sh
