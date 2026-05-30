#!/bin/bash
# ─────────────────────────────────────
#  reporte.sh — Reporte de tráfico
# ─────────────────────────────────────

USUARIO="claude"
BASE="netmonitor_dev"
FECHA=$(date +%Y%m%d_%H%M%S)
ARCHIVO="$HOME/Claude_Project/reportes/reporte_$FECHA.txt"

mkdir -p $HOME/Claude_Project/reportes

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" | tee $ARCHIVO
echo "  NetMonitor — Reporte de tráfico"            | tee -a $ARCHIVO
echo "  Generado: $(date)"                          | tee -a $ARCHIVO
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" | tee -a $ARCHIVO

echo ""                                             | tee -a $ARCHIVO
echo "── Dispositivos registrados ──"               | tee -a $ARCHIVO
psql -U $USUARIO -h localhost $BASE -c \
    "SELECT id, nombre, ip, tipo, activo FROM dispositivos ORDER BY id;" \
    | tee -a $ARCHIVO

echo ""                                             | tee -a $ARCHIVO
echo "── Paquetes por protocolo ──"                 | tee -a $ARCHIVO
psql -U $USUARIO -h localhost $BASE -c \
    "SELECT protocolo, COUNT(*) as total FROM trafico GROUP BY protocolo ORDER BY total DESC;" \
    | tee -a $ARCHIVO

echo ""                                             | tee -a $ARCHIVO
echo "── Top 5 IPs origen ──"                       | tee -a $ARCHIVO
psql -U $USUARIO -h localhost $BASE -c \
    "SELECT ip_origen, COUNT(*) as paquetes FROM trafico GROUP BY ip_origen ORDER BY paquetes DESC LIMIT 5;" \
    | tee -a $ARCHIVO

echo ""                                             | tee -a $ARCHIVO
echo "── Top 5 IPs destino ──"                      | tee -a $ARCHIVO
psql -U $USUARIO -h localhost $BASE -c \
    "SELECT ip_destino, COUNT(*) as paquetes FROM trafico GROUP BY ip_destino ORDER BY paquetes DESC LIMIT 5;" \
    | tee -a $ARCHIVO

echo ""                                             | tee -a $ARCHIVO
echo "── Actividad por hora ──"                     | tee -a $ARCHIVO
psql -U $USUARIO -h localhost $BASE -c \
    "SELECT DATE_TRUNC('hour', timestamp) as hora, COUNT(*) as paquetes FROM trafico GROUP BY hora ORDER BY hora DESC LIMIT 10;" \
    | tee -a $ARCHIVO

echo ""                                             | tee -a $ARCHIVO
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" | tee -a $ARCHIVO
echo "  Reporte guardado en: $ARCHIVO"              | tee -a $ARCHIVO
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" | tee -a $ARCHIVO
