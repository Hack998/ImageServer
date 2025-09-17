# README

---

## 1. Estructura del proyecto

```
.
├── Makefile
├── config/     
│   └── config.config
├── include/     
│   ├── config.h
│   ├── image_proc.h
|   ├── logger.h
│   ├── stb_image.h
│   └── stb_image_write.h
├── scripts/   
|   ├── install.sh
│   ├── purge.sh
│   ├── test.sh
│   └── uninstall.sh
├── src/     
│   ├── cliente.c
│   ├── config.c
|   ├── image_proc.c
│   ├── logger.c
│   └── server.c
├── systemd/     
│   └── imageserver.service
└── README.md
```

- `config/` contiene la configuracion por defecto del programa
- `include/` contiene los headers (C)
- `scripts/` contiene scripts de instalacion, desinstalar y test
- `src/` contiene los programas (C)
- `systemd/` contiene el documento de servicio

---

## 2. Dependencias

```bash
sudo apt install build-essential make pkg-config
```
Si se desea usar test.sh, es necesario instalar imagemagick
```bash
sudo apt install imagemagick
```

- `build-essential` compilar gcc y librerias estandar
- `make` para compilar con el Makefile
- `pkg-config` soporte de compilacion
- `imagemagick` generacion de imagen en test.sh

---


## 3. Test Rapido (local/no servicio)

### Paso 1: Compilacion del programa
```bash
sudo make
```

### Paso 2: Ejecutar test
```bash
sudo ./scripts/test.sh
```

---

## 4. Test (local/servicio)

### Paso 1: Compilacion del programa
```bash
sudo make
```

### Paso 2: Instalar servicio
```bash
sudo make install
```
Deja el server corriendo

### Paso 3: Enviar imagen
```bash
sudo ./client 127.0.0.1 1717 test.jpg
```
Cambiar `test.jpg` por la imagen que se desea probar

---
## 5. Test (red/servicio)

### Paso 1: Compilacion del programa
```bash
sudo make
```

### Paso 2: Instalar servicio
```bash
sudo make install
```
Deja el server corriendo

### Paso 3: Obtener IP del server
```bash
ip addr show
```
Buscar el ip, es el que dice 192.168.xxx.xxx, para este caso se usara `192.169.1.50`

### Paso 4: En otro computador, generar cliente
```bash
sudo make
```
### Paso 5: En el computador cliente, enviar imagen
```bash
sudo ./client 192.169.1.50 1717 test.jpg
```
Cambiar `test.jpg` por la imagen que se desea probar

---

## 6. Funciones del servidor
### Start
```bash
sudo systemctl start imageserver
```
### Status
```bash
sudo systemctl status imageserver
```
### Restart
```bash
sudo systemctl restart imageserver
```
### stop
```bash
sudo systemctl stop imageserver
```
### Logs en vivo (servicio)
```bash
sudo journalctl -u imageserver -f
```





