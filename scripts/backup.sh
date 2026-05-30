#!/bin/bash
# ─────────────────────────────────────
#  backup.sh — Backup de netmonitor_dev
# ─────────────────────────────────────

FECHA=$(date +%Y%m%d_%H%M%S)
DIRECTORIO="$HOME/Claude_Project/backups"
ARCHIVO="$DIRECTORIO/netmonitor_$FECHA.sql"
USUARIO="claude"
BASE="netmonitor_dev"

# Crear directorio si no existe
mkdir -p $DIRECTORIO

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "  NetMonitor — Backup DB"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "Fecha    : $FECHA"
echo "Archivo  : $ARCHIVO"
echo ""

# Ejecutar backup
pg_dump -U $USUARIO -h localhost $BASE > $ARCHIVO

# Verificar si fue exitoso
if [ $? -eq 0 ]; then
    TAMANIO=$(du -sh $ARCHIVO | cut -f1)
    echo "✓ Backup exitoso"
    echo "  Tamaño : $TAMANIO"
else
    echo "✗ Error en el backup"
    exit 1
fi

# Mantener solo los últimos 5 backups
CANTIDAD=$(ls $DIRECTORIO/*.sql 2>/dev/null | wc -l)
if [ $CANTIDAD -gt 5 ]; then
    ls -t $DIRECTORIO/*.sql | tail -n +6 | xargs rm
    echo "  Backups antiguos eliminados"
fi

echo ""
echo "Backups disponibles:"
ls -lh $DIRECTORIO/*.sql
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
