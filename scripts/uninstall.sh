#!/bin/bash
set -e

SERVICE_NAME=imageserver
BIN_DIR=/usr/local/bin
CONFIG_DIR=/etc/server
LOG_DIR=/var/log/server
DATA_DIR=/var/lib/server

echo "[*] Deteniendo servicio..."
sudo systemctl stop $SERVICE_NAME || true

echo "[*] Deshabilitando servicio..."
sudo systemctl disable $SERVICE_NAME || true

echo "[*] Eliminando unidad systemd..."
sudo rm -f /etc/systemd/system/$SERVICE_NAME.service
sudo systemctl daemon-reload
sudo systemctl reset-failed

echo "[*] Eliminando binarios..."
sudo rm -f $BIN_DIR/$SERVICE_NAME
sudo rm -f $BIN_DIR/client

echo "[✔] Desinstalación completada."
echo ""
echo "⚠️ Los siguientes archivos NO fueron eliminados:"
echo "  - Configuración: $CONFIG_DIR/config.conf"
echo "  - Logs: $LOG_DIR/imageserver.log"
echo "  - Directorios de datos: $DATA_DIR/"
echo ""
echo "Si deseas eliminarlos manualmente, ejecuta:"
echo "  sudo rm -rf $CONFIG_DIR $LOG_DIR $DATA_DIR"
