#ifndef CONEXIONES_H
#define CONEXIONES_H

#include <libpq-fe.h>

void listar_conexiones(PGconn *conn);
void insertar_conexion(PGconn *conn, int interfaz_a_id, int interfaz_b_id,
                       const char *tipo_cable);
void eliminar_conexion(PGconn *conn, int id);

#endif
