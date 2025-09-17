#!/bin/bash
set -e

PORT=1717
SERVER_BIN=./server
CLIENT_BIN=./client
TEST_IMG=test_green.jpg
OUT_DIR=/var/lib/image_server/output

echo "[*] Compilando..."
make

echo "[*] Verificando binarios..."
if [ ! -x "$SERVER_BIN" ] || [ ! -x "$CLIENT_BIN" ]; then
    echo "❌ Error: faltan los binarios 'server' o 'client'."
    exit 1
fi

echo "[*] Creando imagen de prueba..."
# Crear una imagen mínima de 10x10 en JPG con ImageMagick (si está disponible)
if command -v convert >/dev/null 2>&1; then
    convert -size 10x10 xc:green "$TEST_IMG"
else
    echo "⚠️ ImageMagick no encontrado, copia manualmente una imagen a $TEST_IMG"
    exit 1
fi

echo "[*] Arrancando servidor en background..."
$SERVER_BIN >server_test.log 2>&1 &
SERVER_PID=$!
sleep 1

echo "[*] Enviando imagen de prueba con cliente..."
$CLIENT_BIN 127.0.0.1 $PORT "$TEST_IMG"

echo "[*] Deteniendo servidor..."
kill $SERVER_PID
wait $SERVER_PID 2>/dev/null || true

echo ""
echo "[✔] Prueba completada."
echo "👉 Revisa '$OUT_DIR' para la imagen procesada."
echo "👉 Logs en 'server_test.log' y /var/log/image_server.log"
