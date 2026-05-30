#!/bin/bash
# ─────────────────────────────────────
#  iniciar.sh — Levanta todo NetMonitor
# ─────────────────────────────────────

PROYECTO="$HOME/Claude_Project"

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "  NetMonitor — Iniciando..."
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# 1. PostgreSQL
echo ""
echo "[ 1/3 ] PostgreSQL..."
sudo service postgresql start > /dev/null 2>&1

if pg_isready -h localhost -q; then
    echo "  ✓ PostgreSQL corriendo"
else
    echo "  ✗ Error al iniciar PostgreSQL"
    exit 1
fi

# 2. Verificar binarios compilados
echo ""
echo "[ 2/3 ] Verificando binarios..."

cd $PROYECTO

if [ ! -f "netmonitor" ] || [ ! -f "captura" ] || [ ! -f "scanner" ] || [ ! -f "servidor" ]; then
    echo "  Compilando..."
    make
    if [ $? -ne 0 ]; then
        echo "  ✗ Error al compilar"
        exit 1
    fi
fi
echo "  ✓ Binarios listos"

# 3. Backup automático al iniciar
echo ""
echo "[ 3/3 ] Backup automático..."
$PROYECTO/scripts/backup.sh > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "  ✓ Backup realizado"
else
    echo "  ⚠ Backup falló — continuando igual"
fi

# Menú de inicio
echo ""
echo ""
echo "================================"
echo "  Sistema listo. Que iniciamos?"
while true; do
echo "================================"
echo "  1. Menu principal (netmonitor)"
echo "  2. Captura de paquetes"
echo "  3. Scanner de red"
echo "  4. Servidor HTTP"
echo "  5. Reporte de trafico"
echo "  6. Salir"
echo "================================"
printf "  Opcion: "
read opcion

case $opcion in
    1) $PROYECTO/netmonitor ;;
    2) sudo $PROYECTO/captura ;;
    3) $PROYECTO/scanner ;;
    4) $PROYECTO/servidor ;;
    5) $PROYECTO/scripts/reporte.sh ;;
    6) echo "Saliendo..."; exit 0 ;;
    *) echo "Opcion invalida"; exit 1 ;;
esac
echo ""
echo "Presiona Enter para continuar..."
read
done
