#!/bin/bash
set -e

SERVICE_NAME=imageserver
BIN_DIR=/usr/local/bin
CONFIG_DIR=/etc/server
CONFIG_FILE=$CONFIG_DIR/config.conf
LOG_DIR=/var/log/server
LOG_FILE=$LOG_DIR/imageserver.log
DATA_DIR=/var/lib/server

echo "[*] Compilando proyecto..."
make clean
make

echo "[*] Instalando binarios en $BIN_DIR..."
sudo install -m 755 server $BIN_DIR/$SERVICE_NAME
sudo install -m 755 client $BIN_DIR/client

echo "[*] Creando directorio de configuración en $CONFIG_DIR..."
sudo mkdir -p $CONFIG_DIR

if [ ! -f "$CONFIG_FILE" ]; then
    echo "[*] Copiando configuración por defecto..."
    sudo cp config/config.conf $CONFIG_FILE
else
    echo "[*] Configuración existente detectada, no se sobrescribe."
fi

echo "[*] Creando directorio de logs en $LOG_DIR..."
sudo mkdir -p $LOG_DIR
sudo touch $LOG_FILE
sudo chown root:adm $LOG_FILE
sudo chmod 664 $LOG_FILE

echo "[*] Creando directorios de clasificación en $DATA_DIR..."
sudo mkdir -p $DATA_DIR/green
sudo mkdir -p $DATA_DIR/red
sudo mkdir -p $DATA_DIR/blue
sudo chown -R root:adm $DATA_DIR
sudo chmod -R 775 $DATA_DIR

echo "[*] Instalando servicio systemd..."
sudo cp systemd/imageserver.service /etc/systemd/system/$SERVICE_NAME.service

echo "[*] Recargando systemd..."
sudo systemctl daemon-reload

echo "[*] Habilitando servicio para iniciar en el arranque..."
sudo systemctl enable $SERVICE_NAME

echo "[*] Iniciando servicio..."
sudo systemctl restart $SERVICE_NAME

echo "[✔] Instalación completada."
echo "Usa 'systemctl status $SERVICE_NAME' para verificar el estado."
