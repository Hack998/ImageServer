#!/bin/bash
set -e

SERVICE_NAME=imageserver
BIN_DIR=/usr/local/bin
CONFIG_DIR=/etc/server
LOG_FILE=/var/log/${SERVICE_NAME}.log
BASE_DIR=/var/lib/${SERVICE_NAME}
SRC_DIR=/usr/local/src/${SERVICE_NAME}

echo "[*] Deteniendo servicio si está activo..."
sudo systemctl stop $SERVICE_NAME || true
sudo systemctl disable $SERVICE_NAME || true

echo "[*] Eliminando unidad systemd..."
sudo rm -f /etc/systemd/system/${SERVICE_NAME}.service

echo "[*] Eliminando binarios..."
sudo rm -f $BIN_DIR/$SERVICE_NAME $BIN_DIR/${SERVICE_NAME}_client

echo "[*] Eliminando configuración, logs y datos..."
sudo rm -rf $CONFIG_DIR
sudo rm -f $LOG_FILE
sudo rm -rf $BASE_DIR

echo "[*] Eliminando scripts y fuentes..."
sudo rm -f /usr/local/bin/install.sh /usr/local/bin/uninstall.sh /usr/local/bin/purge.sh || true
sudo rm -rf $SRC_DIR || true

echo "[*] Recargando systemd..."
sudo systemctl daemon-reload

echo "[*] PURGE completado: el sistema está limpio."
