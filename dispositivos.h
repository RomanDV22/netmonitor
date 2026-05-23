#ifndef DISPOSITIVOS_H
#define DISPOSITIVOS_H

#include <libpq-fe.h>

void listar_dispositivos(PGconn *conn);
void insertar_dispositivo(PGconn *conn, const char *nombre,
                          const char *ip, const char *mac,
                          const char *tipo);
void eliminar_dispositivo(PGconn *conn, int id);

#endif
