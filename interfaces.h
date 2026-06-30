#ifndef INTERFACES_H
#define INTERFACES_H

#include <libpq-fe.h>

void listar_interfaces(PGconn *conn, int dispositivo_id);
void insertar_interfaz(PGconn *conn, int dispositivo_id, const char *nombre,
                       const char *ip, const char *mac, const char *tipo);
void editar_interfaz(PGconn *conn, int id, const char *nueva_ip, const char *nueva_mac);
void eliminar_interfaz(PGconn *conn, int id);

#endif
