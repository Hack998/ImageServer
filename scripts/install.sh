#!/bin/bash
set -e

SERVICE_NAME=imageserver
BIN_SERVER=server
BIN_CLIENT=client
BIN_DIR=/usr/local/bin
CONFIG_DIR=/etc/server
LOG_DIR=/var/log/server
DATA_DIR=/var/lib/server
SYSTEMD_UNIT=/etc/systemd/system/$SERVICE_NAME.service

echo "[*] Instalando servidor y cliente..."

# Copiar binarios
sudo cp -f $BIN_SERVER $BIN_DIR/
sudo cp -f $BIN_CLIENT $BIN_DIR/
sudo chmod 755 $BIN_DIR/$BIN_SERVER $BIN_DIR/$BIN_CLIENT

# Crear usuario/grupo si no existen
if ! id -u $SERVICE_NAME >/dev/null 2>&1; then
    echo "[*] Creando usuario $SERVICE_NAME..."
    sudo useradd -r -s /bin/false $SERVICE_NAME
fi

# Crear directorios necesarios
echo "[*] Creando directorios..."
sudo mkdir -p $CONFIG_DIR $LOG_DIR $DATA_DIR
sudo touch $CONFIG_DIR/config.conf
sudo touch $LOG_DIR/$SERVICE_NAME.log
sudo chown -R $SERVICE_NAME:$SERVICE_NAME $CONFIG_DIR $LOG_DIR $DATA_DIR

# Copiar unidad systemd
echo "[*] Instalando unidad systemd..."
sudo tee $SYSTEMD_UNIT > /dev/null <<EOL
[Unit]
Description=Image Processing Server (Histogram Equalization and Classification)
After=network.target

[Service]
Type=simple
ExecStart=$BIN_DIR/$BIN_SERVER
Restart=on-failure
User=$SERVICE_NAME
Group=$SERVICE_NAME
WorkingDirectory=$DATA_DIR
StandardOutput=append:$LOG_DIR/$SERVICE_NAME.log
StandardError=append:$LOG_DIR/$SERVICE_NAME.log

[Install]
WantedBy=multi-user.target
EOL

# Recargar systemd
echo "[*] Recargando systemd..."
sudo systemctl daemon-reload

# Habilitar servicio al arranque
echo "[*] Habilitando servicio..."
sudo systemctl enable $SERVICE_NAME

# Iniciar servicio
echo "[*] Iniciando servicio..."
sudo systemctl start $SERVICE_NAME

echo "[✔] Instalación completada."
echo ""
echo "Puedes verificar el estado con:"
echo "  systemctl status $SERVICE_NAME"
echo ""
echo "Logs disponibles en: $LOG_DIR/$SERVICE_NAME.log"
