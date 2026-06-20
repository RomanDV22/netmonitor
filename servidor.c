#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <libpq-fe.h>
#include "db.h"

#define PUERTO   8080
#define BUFFER   4096

// ─────────────────────────────────────
//  Decodificar Base64
// ─────────────────────────────────────
int base64_decode(const char *input, char *output, int max_len) {
    const char tabla[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int len = strlen(input);
    int out_i = 0;

    int vals[4];
    for (int i = 0; i < len; i += 4) {
        for (int j = 0; j < 4; j++) {
            char c = (i + j < len) ? input[i + j] : 'A';
            if (c == '=') { vals[j] = 0; continue; }
            char *p = strchr(tabla, c);
            vals[j] = p ? (int)(p - tabla) : 0;
        }
        if (out_i < max_len - 1)
            output[out_i++] = (vals[0] << 2) | (vals[1] >> 4);
        if (out_i < max_len - 1)
            output[out_i++] = ((vals[1] & 0xF) << 4) | (vals[2] >> 2);
        if (out_i < max_len - 1)
            output[out_i++] = ((vals[2] & 0x3) << 6) | vals[3];
    }
    output[out_i] = '\0';
    return out_i;
}

// ─────────────────────────────────────
//  Validar credenciales del header Authorization
// ─────────────────────────────────────
int autenticado(char *request) {
    char *env_user = getenv("NETMONITOR_USER");
    char *env_pass = getenv("NETMONITOR_PASS");

    if (!env_user || !env_pass) {
        printf("[AUTH] Variables de entorno no configuradas\n");
        return 0;
    }

    char *auth_header = strstr(request, "Authorization: Basic ");
    if (!auth_header) return 0;

    auth_header += strlen("Authorization: Basic ");

    char b64_valor[256] = {0};
    int i = 0;
    while (auth_header[i] && auth_header[i] != '\r' && auth_header[i] != '\n' && i < 255) {
        b64_valor[i] = auth_header[i];
        i++;
    }
    b64_valor[i] = '\0';

    char decodificado[256] = {0};
    base64_decode(b64_valor, decodificado, sizeof(decodificado));

    char esperado[256];
    snprintf(esperado, sizeof(esperado), "%s:%s", env_user, env_pass);

    return strcmp(decodificado, esperado) == 0;
}

// ─────────────────────────────────────
//  Decodificar URL (%27 → ', %20 → ' ')
// ─────────────────────────────────────
void url_decode(char *dst, const char *src) {
    char a, b;
    while (*src) {
        if (*src == '%' && (a = src[1]) && (b = src[2])) {
            if (a >= '0' && a <= '9') a -= '0';
            else if (a >= 'a' && a <= 'f') a -= 'a' - 10;
            else if (a >= 'A' && a <= 'F') a -= 'A' - 10;
            if (b >= '0' && b <= '9') b -= '0';
            else if (b >= 'a' && b <= 'f') b -= 'a' - 10;
            else if (b >= 'A' && b <= 'F') b -= 'A' - 10;
            *dst++ = 16 * a + b;
            src += 3;
        } else if (*src == '+') {
            *dst++ = ' ';
            src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

// ─────────────────────────────────────
//  Extraer parámetro de la URL
//  Ej: /buscar?nombre=Router → "Router"
// ─────────────────────────────────────
char *extraer_parametro(char *request, const char *param) {
    char *pos = strstr(request, param);
    if (!pos) return NULL;

    pos += strlen(param);
    char *valor = malloc(256);
    int i = 0;

    while (*pos && *pos != ' ' && *pos != '&' && *pos != '\n') {
        // Decodificar %20 → espacio
        if (*pos == '%' && *(pos+1) == '2' && *(pos+2) == '0') {
            valor[i++] = ' ';
            pos += 3;
        } else {
            valor[i++] = *pos++;
        }
    }
    valor[i] = '\0';

// Decodificar URL encoding
    char *decoded = malloc(256);
    url_decode(decoded, valor);
    free(valor);
    
    return decoded;
}

// ─────────────────────────────────────
//  ⚠️  CONSULTA VULNERABLE — SQL Injection
//  NUNCA hacer esto en producción
// ─────────────────────────────────────
void buscar_vulnerable(PGconn *conn, int socket, char *nombre) {
    char query[512];

    // ⚠️ Concatenación directa — PELIGROSO
    snprintf(query, sizeof(query),
        "SELECT id, nombre, ip, tipo FROM dispositivos WHERE nombre = '%s'",
        nombre);

    printf("[VULNERABLE] Query ejecutada: %s\n", query);

    PGresult *res = PQexec(conn, query);
    char respuesta[4096] = "";

    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        int filas = PQntuples(res);
        snprintf(respuesta, sizeof(respuesta),
                 "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");

        char linea[256];
        snprintf(linea, sizeof(linea), "Resultados: %d\n\n", filas);
        strcat(respuesta, linea);

        for (int i = 0; i < filas; i++) {
            snprintf(linea, sizeof(linea), "ID: %s | Nombre: %s | IP: %s | Tipo: %s\n",
                PQgetvalue(res, i, 0),
                PQgetvalue(res, i, 1),
                PQgetvalue(res, i, 2),
                PQgetvalue(res, i, 3));
            strcat(respuesta, linea);
        }
    } else {
        snprintf(respuesta, sizeof(respuesta),
                 "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nError: %s",
                 PQerrorMessage(conn));
    }

    send(socket, respuesta, strlen(respuesta), 0);
    PQclear(res);
}

// ─────────────────────────────────────
//  ✅  CONSULTA SEGURA — Parametrizada
// ─────────────────────────────────────
void buscar_seguro(PGconn *conn, int socket, char *nombre) {
    const char *params[1] = {nombre};

    // ✅ Parámetro separado — SEGURO
    PGresult *res = PQexecParams(conn,
        "SELECT id, nombre, ip, tipo FROM dispositivos WHERE nombre = $1",
        1, NULL, params, NULL, NULL, 0
    );

    printf("[SEGURO]     Query ejecutada con parámetro: '%s'\n", nombre);

    char respuesta[4096] = "";

    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        int filas = PQntuples(res);
        snprintf(respuesta, sizeof(respuesta),
                 "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");

        char linea[256];
        snprintf(linea, sizeof(linea), "Resultados: %d\n\n", filas);
        strcat(respuesta, linea);

        for (int i = 0; i < filas; i++) {
            snprintf(linea, sizeof(linea), "ID: %s | Nombre: %s | IP: %s | Tipo: %s\n",
                PQgetvalue(res, i, 0),
                PQgetvalue(res, i, 1),
                PQgetvalue(res, i, 2),
                PQgetvalue(res, i, 3));
            strcat(respuesta, linea);
        }
    } else {
        snprintf(respuesta, sizeof(respuesta),
                 "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nError: %s",
                 PQerrorMessage(conn));
    }

    send(socket, respuesta, strlen(respuesta), 0);
    PQclear(res);
}

// ─────────────────────────────────────
//  Manejar cada request HTTP
// ─────────────────────────────────────
void manejar_request(PGconn *conn, int socket, char *request) {
    printf("\n[REQUEST] %s\n", request);

    if (!autenticado(request)) {
        char *resp = "HTTP/1.1 401 Unauthorized\r\n"
                     "WWW-Authenticate: Basic realm=\"NetMonitor\"\r\n"
                     "Content-Type: text/plain\r\n\r\n"
                     "Acceso denegado. Credenciales requeridas.\n";
        send(socket, resp, strlen(resp), 0);
        printf("[AUTH] Acceso denegado\n");
        return;
    }

    if (strstr(request, "GET /vulnerable")) {
        char *nombre = extraer_parametro(request, "nombre=");
        if (nombre) {
            buscar_vulnerable(conn, socket, nombre);
            free(nombre);
        }
    } else if (strstr(request, "GET /seguro")) {
        char *nombre = extraer_parametro(request, "nombre=");
        if (nombre) {
            buscar_seguro(conn, socket, nombre);
            free(nombre);
        }
    } else {
        char *resp = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"
                     "NetMonitor API\n"
                     "Endpoints:\n"
                     "  /vulnerable?nombre=X  (inseguro)\n"
                     "  /seguro?nombre=X      (seguro)\n";
        send(socket, resp, strlen(resp), 0);
    }
}

// ─────────────────────────────────────
//  Main — servidor TCP
// ─────────────────────────────────────
int main() {
    PGconn *conn = conectar();
    printf("✓ Conectado a la DB\n");

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(PUERTO);

    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 5);

    printf("✓ Servidor escuchando en puerto %d\n", PUERTO);
    printf("  Desde la notebook probá:\n");
    printf("  curl http://192.168.0.139:8080/vulnerable?nombre=Router-Casa\n\n");

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        char buffer[BUFFER] = {0};
        read(client_fd, buffer, BUFFER);
        manejar_request(conn, client_fd, buffer);
        close(client_fd);
    }

    desconectar(conn);
    return 0;
}
