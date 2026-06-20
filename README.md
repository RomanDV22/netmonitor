# NetMonitor

Sistema de monitoreo de red en C con PostgreSQL, diseñado para capturar tráfico en tiempo real, detectar dispositivos activos en tu red local, y demostrar (de forma educativa) vulnerabilidades de seguridad y cómo prevenirlas.

## Características

- **Captura de paquetes en tiempo real** con libpcap (protocolo, IPs, puertos, tamaño)
- **Scanner de red** que detecta automáticamente dispositivos activos (ping sweep)
- **Menú interactivo** para gestionar dispositivos (CRUD completo)
- **Servidor HTTP en C** con autenticación básica
- **Demostración de SQL injection** — cómo funciona y cómo prevenirla
- **Base de datos PostgreSQL** con esquema relacional
- **Scripts de automatización** — backup, reportes, inicio del sistema

## Requisitos

### Sistema
- WSL2 con Ubuntu 24.04 (o Linux directo)
- PostgreSQL 16+
- gcc 13+
- git

### Librerías
```bash
sudo apt update
sudo apt install build-essential libpq-dev libpcap-dev
```

### Credenciales PostgreSQL
```bash
# Usuario: claude
# Contraseña: (la que configuraste)
# Base de datos: netmonitor_dev
```

## Instalación y compilación

### 1. Clonar el repositorio
```bash
git clone https://github.com/RomanDV22/netmonitor.git
cd netmonitor
```

### 2. Configurar PostgreSQL
```bash
# Iniciar el servidor
sudo service postgresql start

# Verificar que está corriendo
pg_isready -h localhost
```

### 3. Compilar todo
```bash
make
```

Genera los binarios: `netmonitor`, `captura`, `scanner`, `servidor`

## Uso

### Opción 1 — Iniciar todo de una vez (recomendado)
```bash
# Setear credenciales para el servidor HTTP
export NETMONITOR_USER=roman
export NETMONITOR_PASS=tu_password

# Ejecutar el script de inicio
./scripts/iniciar.sh
```

Te presenta un menú para elegir qué ejecutar.

### Opción 2 — Ejecutar módulos individuales

#### Menú principal (CRUD de dispositivos)
```bash
./netmonitor
```

Opciones:
- 1: Listar dispositivos
- 2: Agregar dispositivo
- 3: Editar IP y MAC
- 4: Eliminar dispositivo
- 5: Ver tráfico capturado (últimos N paquetes)
- 6: Salir

#### Capturador de paquetes (tiempo real)
```bash
# Necesita permisos de superusuario
sudo ./captura

# Genera entrada en tabla "trafico" automáticamente
# Ctrl+C para detener
```

#### Scanner de red
```bash
./scanner

# Escanea 192.168.0.1 - 192.168.0.254
# Detecta dispositivos activos
# Agrega automáticamente los nuevos a la DB
```

#### Servidor HTTP (con autenticación)
```bash
# Variables de entorno requeridas
export NETMONITOR_USER=roman
export NETMONITOR_PASS=tu_password

./servidor

# Escucha en puerto 8080
# Desde la PC: curl -u roman:password http://localhost:8080/vulnerable?nombre=Router-Casa
# Desde la red: curl -u roman:password http://192.168.0.139:8080/vulnerable?nombre=Router-Casa
```

#### Scripts de automatización
```bash
./scripts/backup.sh      # Backup de la DB
./scripts/reporte.sh     # Reporte de tráfico
./scripts/iniciar.sh     # Menú de inicio (interactivo)
```

## Arquitectura

## Seguridad educativa — SQL Injection

### Endpoint vulnerable
```bash
curl -u roman:password "http://192.168.0.139:8080/vulnerable?nombre=' OR 1=1--"
```
Extrae **todos** los dispositivos (sin filtrar por nombre).

### Endpoint seguro (prevenido)
```bash
curl -u roman:password "http://192.168.0.139:8080/seguro?nombre=' OR 1=1--"
```
Devuelve 0 resultados — el input se trata como texto literal, nunca como SQL.

**Diferencia técnica:**
- Vulnerable: concatena strings → `WHERE nombre = '' OR 1=1--`
- Seguro: parámetros separados → `WHERE nombre = $1` (PostgreSQL lo valida)

## Estructura de directorios

## Configuración inicial (primera vez)

### 1. Crear archivo .pgpass para no pedir contraseña
```bash
echo "localhost:5432:netmonitor_dev:claude:tu_password" >> ~/.pgpass
chmod 600 ~/.pgpass
```

### 2. Agregar variables de entorno al ~/.bashrc (opcional pero recomendado)
```bash
echo 'export NETMONITOR_USER=roman' >> ~/.bashrc
echo 'export NETMONITOR_PASS=tu_password' >> ~/.bashrc
source ~/.bashrc
```

### 3. Verificar la DB
```bash
psql -U claude -h localhost netmonitor_dev -c "\dt"
```

Deberías ver las 3 tablas: dispositivos, interfaces, trafico

## Ejemplos de uso

### Capturar y ver tráfico en tiempo real
```bash
# Terminal 1 — capturar
sudo ./captura

# Terminal 2 — enviar datos
curl https://example.com

# Terminal 1 mostrará los paquetes TCP en tiempo real
```

### Identificar dispositivos en tu red
```bash
./scanner

# Muestra un mapa completo de IPs activas
# Las agrega automáticamente a la DB si son nuevas
```

### Analizar el tráfico desde el menú
```bash
./netmonitor
# Opción 5: Ver tráfico capturado
# Elige cuántos últimos registros ver
```

### Generar reportes
```bash
./scripts/reporte.sh

# Genera archivo en reportes/reporte_YYYYMMDD_HHMMSS.txt
# Incluye resumen por protocolo, IPs más frecuentes, actividad horaria
```

## Autenticación — Basic Auth

El servidor HTTP protege sus endpoints con autenticación básica HTTP:

```bash
# Sin credenciales → 401 Unauthorized
curl http://192.168.0.139:8080/vulnerable?nombre=X

# Con credenciales → ejecuta la consulta
curl -u roman:password http://192.168.0.139:8080/vulnerable?nombre=X
```

Las credenciales se definen mediante variables de entorno:
```bash
export NETMONITOR_USER=tu_usuario
export NETMONITOR_PASS=tu_contraseña
```

## Próximos pasos (mejoras futuras)

- Reemplazar Basic Auth con JWT para mayor seguridad
- Agregar tabla de logs para auditar accesos
- Interfaz web en lugar de CLI
- Gráficos de tráfico en tiempo real
- Alertas automáticas por comportamiento inusual
- Soporte para IPv6

## Notas técnicas

- **Lenguaje:** C (POSIX compliant)
- **DB:** PostgreSQL 16 (relacional, consultas parametrizadas)
- **Librerías:** libpq (PostgreSQL), libpcap (captura de paquetes)
- **Red:** Capa 3 (IP) y Capa 4 (TCP/UDP) del modelo OSI
- **Autenticación:** HTTP Basic Auth (Base64 encoded)

## Licencia

Proyecto educativo — sin licencia restrictiva.

## Autor

Román — desarrollo e implementación

## Contacto / Recursos

- Repositorio: https://github.com/RomanDV22/netmonitor
- Commits: git log --oneline

---

## Guía técnica detallada por módulo

### db.c / db.h — Conexión a PostgreSQL

Maneja toda la comunicación con PostgreSQL usando libpq.

**Funciones:**
- `PGconn *conectar()` — Conecta usando credenciales del #define CONNINFO
- `void desconectar(PGconn *conn)` — Cierra la conexión y libera memoria

**Importante:** Las credenciales se leen del #define, no del entorno. Para cambiar usuario/contraseña, editar `db.c`:
```c
#define CONNINFO "host=localhost dbname=netmonitor_dev user=claude password=tu_password"
```

### dispositivos.c / dispositivos.h — Gestión de dispositivos

CRUD completo sobre la tabla `dispositivos`.

**Funciones:**
- `listar_dispositivos(conn)` — SELECT * con formato tabular
- `insertar_dispositivo(conn, nombre, ip, mac, tipo)` — INSERT parametrizado
- `eliminar_dispositivo(conn, id)` — DELETE por ID

Todas usan `PQexecParams()` para prevenir SQL injection.

**Ejemplo:**
```c
insertar_dispositivo(conn, "Celular", "192.168.0.50", "AA:BB:CC:DD:EE:FF", "celular");
```

### captura.c — Capturador de paquetes (libpcap)

Captura tráfico en tiempo real en la interfaz eth0 (WSL).

**Cómo funciona:**
1. `pcap_open_live()` — abre eth0 para captura
2. `pcap_compile()` — compila un filtro BPF (Basic Packet Filter)
3. `pcap_loop()` — escucha indefinidamente, llama callback por cada paquete
4. Callback `procesar_paquete()` — extrae headers, identifica protocolo, inserta en DB

**Protocolos soportados:**
- ICMP (ping) — puerto 0
- TCP — lee puerto origen y destino del header TCP
- UDP — lee puerto origen y destino del header UDP
- Otros — marca como "OTRO"

**Datos capturados en tabla trafico:**

**Limitación actual:**
- No captura el payload (contenido de los datos)
- eth0 es una interfaz virtual — no ve WiFi directamente
- Para capturar tráfico real WiFi, necesitaría VM con Linux nativa en modo bridge

### scanner.c — Detector de dispositivos

Escanea el rango 192.168.0.1 - 192.168.0.254 enviando pings.

**Algoritmo:**

**Timing:**
- Timeout de 1 segundo por ping
- 254 IPs × 1s = ~4 minutos de escaneo

**Salida:**

**Nota:** Los dispositivos "desconocidos" se pueden editar después con `netmonitor` opción 3.

### servidor.c — Servidor HTTP + SQL Injection

Servidor TCP en puerto 8080 que responde a requests HTTP.

**Endpoints:**

| Endpoint | Método | Parámetros | Comportamiento |
|----------|--------|-----------|-----------------|
| `/` | GET | - | Lista endpoints disponibles |
| `/vulnerable` | GET | `nombre=X` | Busca dispositivo — **SQL vulnerable** |
| `/seguro` | GET | `nombre=X` | Busca dispositivo — **SQL parametrizado** |

**Autenticación:**
- Tipo: HTTP Basic Auth (RFC 7617)
- Header: `Authorization: Basic base64(user:pass)`
- Credenciales: variables de entorno `NETMONITOR_USER` y `NETMONITOR_PASS`
- Si falta auth → responde 401 Unauthorized

**Ejemplo vulnerable (NO USAR EN PRODUCCIÓN):**
```c
snprintf(query, sizeof(query),
    "SELECT ... WHERE nombre = '%s'", nombre);
PQexec(conn, query);
```

Con `nombre = "' OR 1=1--"` la query se convierte en:
```sql
WHERE nombre = '' OR 1=1--'
```
Devuelve todos los registros, sin filtrar por nombre.

**Ejemplo seguro (USO RECOMENDADO):**
```c
const char *params[1] = {nombre};
PQexecParams(conn,
    "SELECT ... WHERE nombre = $1",
    1, NULL, params, NULL, NULL, 0);
```

PostgreSQL garantiza que `$1` nunca se interpreta como código SQL.

### netmonitor.c — Menú CRUD interactivo

Interfaz terminal para gestionar dispositivos.

**Flujo:**

**Función editar_dispositivo():**
- Muestra lista de dispositivos
- Pide ID a editar
- Permite cambiar IP y/o MAC (dejar en blanco para no modificar)
- Updates parametrizados (no vulnerable)

**Función ver_trafico():**
- Pide cuántos registros ver (1-50)
- Muestra: protocolo, IPs, puertos, tamaño, timestamp
- Resumen por protocolo al final

### Scripts Bash

#### backup.sh
```bash
pg_dump -U claude -h localhost netmonitor_dev > backups/netmonitor_YYYYMMDD_HHMMSS.sql
```
- Crea backup completo de la DB
- Guarda en carpeta `backups/`
- Mantiene solo los últimos 5 backups (elimina automáticamente los viejos)

#### reporte.sh
```bash
psql -U claude -h localhost netmonitor_dev -c "[queries]"
```
- Ejecuta 5 consultas SQL distintas
- Muestra: dispositivos, protocolo, IPs frecuentes, actividad horaria
- Guarda salida en `reportes/reporte_YYYYMMDD_HHMMSS.txt`

#### iniciar.sh
```bash
sudo service postgresql start
make [si no existen binarios]
./scripts/backup.sh
[menú interactivo]
```
- Arranca PostgreSQL
- Verifica binarios compilados (recompila si falta algo)
- Ejecuta backup automático
- Presenta menú para elegir qué módulo ejecutar

## Consultas SQL útiles

### Ver todos los dispositivos con sus interfaces
```sql
SELECT d.nombre, d.ip, i.nombre as interfaz, i.ip as ip_interfaz
FROM dispositivos d
LEFT JOIN interfaces i ON d.id = i.dispositivo_id
ORDER BY d.id;
```

### Tráfico por protocolo (últimas 24 horas)
```sql
SELECT protocolo, COUNT(*) as total,
       DATE_TRUNC('hour', timestamp) as hora
FROM trafico
WHERE timestamp > NOW() - INTERVAL '24 hours'
GROUP BY protocolo, hora
ORDER BY hora DESC;
```

### Top 10 IPs más activas (origen)
```sql
SELECT ip_origen, COUNT(*) as paquetes
FROM trafico
GROUP BY ip_origen
ORDER BY paquetes DESC
LIMIT 10;
```

### Dispositivos sin registrar (IPs huérfanas en tráfico)
```sql
SELECT DISTINCT ip_origen
FROM trafico
WHERE ip_origen NOT IN (
    SELECT ip FROM dispositivos UNION SELECT ip FROM interfaces
)
ORDER BY ip_origen;
```

## Troubleshooting

### Error: "connection to server failed — FATAL: password authentication failed"
**Causa:** Contraseña de PostgreSQL incorrecta en `db.c` o `~/.pgpass`
**Solución:** Verificá credenciales y actualiza ambos lugares

### Error: "UNION types integer and character varying cannot be matched"
**Causa:** SQL injection mal formulado en servidor.c (tipos de datos no coinciden)
**Solución:** Castea todos los campos de ambos SELECT al mismo tipo

### `captura` no muestra paquetes
**Causa:** 
- No hay tráfico en eth0 en ese momento
- eth0 es interfaz virtual — no captura todo el tráfico
**Solución:** Generá tráfico con `ping`, `curl`, `nslookup` desde otra terminal

### `scanner` tarda mucho
**Causa:** Escanea 254 IPs × 1s timeout = ~4 minutos
**Solución:** Es esperado. O acotá el rango manualmente editando scanner.c

### `servidor` no responde desde la notebook
**Causa:** 
- Servidor no levantado
- Variables de entorno no seteadas
- Firewall de Windows bloqueando puerto 8080
**Solución:** 
1. Verificá que `./servidor` esté corriendo en la PC
2. Desde la PC probá: `curl localhost:8080`
3. Si no funciona desde la notebook, abrí puerto 8080 en Windows con netsh

### "Presioná Enter para continuar..." queda pegado
**Causa:** Buffer de entrada de scanf() sin limpiar
**Solución:** Ya está corregido con `getchar()` después de `scanf()`

## Performance y limitaciones

| Aspecto | Capacidad | Nota |
|---------|-----------|------|
| Paquetes capturados/seg | ~100-1000 | Depende del tráfico real |
| Dispositivos en DB | Ilimitado | Teóricamente, prácticamente limitado por RAM/DB |
| Escaneo de red | 254 IPs | Tiempo: ~4 minutos |
| Conexiones simultáneas servidor | 1 | Servidor secuencial, no concurrente |
| Histórico de trafico | Ilimitado | Crecer indefinidamente; considerar archivar |

**Mejora futura:** Usar pthreads en servidor.c para manejar múltiples clientes simultáneamente.

## Referencia educativa — Modelo OSI

NetMonitor trabaja en las **Capas 3 y 4**:

**Por qué no capturamos payload (Capa 7)?**
- Requerría parsear HTTP, DNS, etc. — complejo
- HTTPS/TLS está encriptado — imposible leerlo sin certificados
- El header (Capas 3-4) es suficiente para análisis de tráfico

## Control de versiones — Git workflow

```bash
# Ver historial de commits
git log --oneline

# Revertir a un commit anterior
git checkout [commit_hash]

# Ver cambios antes de commitear
git diff

# Stash (guardar cambios sin commitear)
git stash
git stash pop
```

## Testing manual

### Test 1 — Flujo completo en 5 minutos
```bash
# Terminal 1
./scripts/iniciar.sh
# Elegir opción 3: Scanner

# Terminal 2 (mientras scanea)
./netmonitor
# Opción 1: Listar dispositivos
# Verifica que aparezcan nuevos

# Terminal 1 (cuando termine scanner)
# Elegir opción 4: Servidor
```

### Test 2 — SQL Injection educativa
```bash
# Terminal 2 (mientras servidor corre en Terminal 1)
# Sin autenticación → debe fallar
curl "http://localhost:8080/vulnerable?nombre=test"

# Con autenticación → vulnerable a SQLi
curl -u roman:password "http://localhost:8080/vulnerable?nombre=' OR 1=1--"
# Devuelve todos los dispositivos

# Mismo ataque en endpoint seguro → bloquea
curl -u roman:password "http://localhost:8080/seguro?nombre=' OR 1=1--"
# Devuelve 0 resultados
```

### Test 3 — Captura de paquetes
```bash
# Terminal 1
sudo ./captura

# Terminal 2
curl https://example.com

# Terminal 1 debe mostrar paquetes TCP
```

## Contacto y soporte

- **Repositorio:** https://github.com/RomanDV22/netmonitor
- **Issues:** Reportar bugs directamente en GitHub
- **Commits:** `git log` para ver historial completo

---

**Última actualización:** 31 de mayo de 2026  
**Estado:** Proyecto consolidado y documentado  
**Próxima fase:** Agregar interfaz web o JWT

## Arquitectura de datos — Modelo relacional

### Tabla dispositivos

**Índices:** PRIMARY KEY sobre `id`  
**Referencias:** `interfaces.dispositivo_id` → `dispositivos.id` (relación 1:N)

### Tabla interfaces

**Relación:** Un dispositivo puede tener múltiples interfaces  
**Ejemplo:**

### Tabla trafico

**Relación:** Una interfaz puede generar múltiples paquetes  
**Nota actual:** `interfaz_id` siempre NULL porque captura.c no implementó esa lógica aún

### Diagrama de relaciones

## Compilación avanzada — Makefile customizado

El Makefile actual es funcional pero básico. Para proyectos más grandes:

```makefile
# Versión mejorada (no necesaria ahora, pero bueno saber)

CC        = gcc
CFLAGS    = -I/usr/include/postgresql -Wall -Wextra -O2 -g
LDFLAGS   = -lpq -lpcap
TARGETS   = netmonitor captura scanner servidor
SRCS      = *.c
DEPS      = db.h dispositivos.h

.PHONY: all clean install debug

all: $(TARGETS)

$(TARGETS): %: %.c db.c $(DEPS)
	$(CC) $< db.c -o $@ $(CFLAGS) $(LDFLAGS)

debug: CFLAGS += -g -O0 -DDEBUG
debug: clean all

clean:
	rm -f $(TARGETS) *.o core

install: all
	mkdir -p ~/bin
	cp $(TARGETS) ~/bin/

help:
	@echo "Targets: all clean debug install help"
```

## Debugging — Técnicas prácticas

### GDB (GNU Debugger)

**Compilar con símbolos de debug:**
```bash
gcc -g captura.c db.c -o captura -I/usr/include/postgresql -lpq -lpcap
```

**Ejecutar bajo GDB:**
```bash
gdb ./captura
(gdb) break procesar_paquete    # punto de quiebre
(gdb) run                       # ejecuta
(gdb) next                      # siguiente línea
(gdb) print ip_header->ip_src   # inspecciona variable
(gdb) backtrace                 # stack trace si crashea
(gdb) quit
```

### Valgrind — Detector de memory leaks

```bash
valgrind --leak-check=full ./netmonitor
```

Reporta cualquier memoria no liberada. NetMonitor ya usa `PQclear()` y `free()` correctamente, pero siempre bueno verificar.

### Printf debugging

Método clásico — agregar prints estratégicos:

```c
printf("[DEBUG] ip_origen: %s, ip_destino: %s\n", ip_origen, ip_destino);
printf("[DEBUG] paquete %d bytes, protocolo: %s\n", header->len, protocolo);
```

Compilar y ejecutar con salida redirigida:
```bash
make && ./captura > debug.log 2>&1
cat debug.log | grep DEBUG
```

## Extensiones futuras — Roadmap

### Fase 2 — Mejoras de seguridad

**Estimado:** 2-3 días de trabajo

### Fase 3 — Frontend web

**Estimado:** 1-2 semanas

### Fase 4 — Concurrencia y escalabilidad

**Estimado:** 1 semana

### Fase 5 — ML / Detección de anomalías

**Estimado:** 2-3 semanas

## Casos de uso reales

### 1. Monitoreo del hogar

### 2. Educativo — Clase de redes

### 3. Detección de intrusiones (IDS light)

### 4. Auditoría de cumplimiento

## Benchmarking — Performance esperado

### Captura de paquetes

### Scanner de red

### Servidor HTTP

## Seguridad — Checklist

## Integración con herramientas externas

### tcpdump — alternativa a captura.c
```bash
# Capturar en archivo
tcpdump -i eth0 -w traffic.pcap

# Analizar después
tcpdump -r traffic.pcap -n | head -20
```

Ventaja: más robusto y usado en producción  
Desventaja: no guarda en BD automáticamente

### Wireshark — visualización gráfica
```bash
# Cargar el .pcap en Wireshark
wireshark traffic.pcap
```

NetMonitor es más simple, Wireshark es más poderoso.

### nmap — escaneo avanzado de puertos
```bash
# Alternativa a scanner.c (más detallado)
nmap -sn 192.168.0.0/24      # ping scan
nmap -sS 192.168.0.1         # SYN stealth scan
```

NetMonitor usa ping (ICMP), nmap es más versatile.

## Comparativa con herramientas existentes

| Aspecto | NetMonitor | Wireshark | Nagios |
|---------|-----------|-----------|--------|
| **Lenguaje** | C | C | Perl/Python |
| **Tamaño** | ~800 líneas | ~500K líneas | ~50K líneas |
| **Aprendizaje** | Fácil | Medio | Difícil |
| **UI** | Terminal (CLI) | GUI | Web |
| **DB Integrada** | Sí (PostgreSQL) | No | Sí (embedded) |
| **SQL Injection Demo** | Sí | No | No |
| **Opensource** | Sí | Sí | Sí |
| **Producción-ready** | No | Sí | Sí |

**NetMonitor es para:** aprender redes + seguridad  
**Wireshark es para:** análisis profundo de tráfico  
**Nagios es para:** monitoreo empresarial 24/7

## FAQ — Preguntas frecuentes

**P: ¿Por qué C y no Python?**  
R: C es más rápido, usa menos recursos, y es lo que usan herramientas reales (tcpdump, libpcap, nginx). Además, aprendés memory management.

**P: ¿Por qué PostgreSQL y no SQLite?**  
R: PostgreSQL soporta usuarios y permisos (buen para multi-tenant). SQLite es más simple pero no escala.

**P: ¿Funciona en macOS?**  
R: Parcialmente. libpcap funciona, pero las IPs del rango 192.168.0.x dependen de tu red WiFi. En WSL es más sencillo.

**P: ¿Puedo capturar HTTPS?**  
R: No sin el certificado privado. HTTPS está encriptado end-to-end. Solo ves IPs, puertos, tamaño de paquetes — no contenido.

**P: ¿Cuánto tráfico puedo guardar?**  
R: Teóricamente ilimitado, pero PostgreSQL eventual ralentiza. Considerar archiving después de 1 mes.

**P: ¿Es legal capturar tráfico de otros?**  
R: Legal en tu propia red. Ilegal en red ajena sin consentimiento. Este proyecto es educativo solo.

**P: ¿Cómo veo quién accedió al servidor?**  
R: Hoy solo imprime en consola. Próximo paso: agregar tabla `logs` con IP, timestamp, usuario, acción.

## Recursos de aprendizaje

### Libros
- "TCP/IP Illustrated Vol. 1" — Stevens, Fenner (CLÁSICO)
- "The C Programming Language" — Kernighan & Ritchie

### Online
- PostgreSQL docs: https://www.postgresql.org/docs/
- libpcap tutorial: https://www.tcpdump.org/papers/sniffing-faq.html
- RFC 7617 (HTTP Basic Auth): https://tools.ietf.org/html/rfc7617
- OWASP SQL Injection: https://owasp.org/www-community/attacks/SQL_Injection

### Herramientas relacionadas
- Wireshark: https://www.wireshark.org/
- tcpdump: https://www.tcpdump.org/
- nmap: https://nmap.org/
- Suricata (IDS): https://suricata.io/

## Changelog

## Cómo contribuir (si lo extendés)

1. Fork el repo en GitHub
2. Crea rama para tu feature: `git checkout -b feature/jwt-auth`
3. Commitea cambios: `git commit -am "Add JWT authentication"`
4. Push a la rama: `git push origin feature/jwt-auth`
5. Abrí Pull Request

## Licencia y disclaimer

**Proyecto educativo.** Úsalo para aprender redes y seguridad. Úsalo responsablemente.
## Agradecimientos

- PostgreSQL team
- libpcap team
- Comunidad de C/Linux
- Claude AI 
---

**Última actualización:** 31 de mayo de 2026  
**Mantenedor:** Román  
**Estado:** Proyecto educativo consolidado  
**Próximo paso:** Interfaz web o JWT

