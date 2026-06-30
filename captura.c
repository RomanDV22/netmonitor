#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcap.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include "db.h"

// ─────────────────────────────────────
//  Callback — se ejecuta por cada paquete capturado
// ─────────────────────────────────────
void procesar_paquete(u_char *args,
                      const struct pcap_pkthdr *header,
                      const u_char *packet) {

    PGconn *conn = (PGconn *)args;

    // Saltar header Ethernet (14 bytes)
    struct ip *ip_header = (struct ip *)(packet + 14);

    char ip_origen[16];
    char ip_destino[16];
    char protocolo[10];
    char tamanio[10];
    int  puerto_origen  = 0;
    int  puerto_destino = 0;

    // Extraer IPs
    strncpy(ip_origen,  inet_ntoa(ip_header->ip_src), 15);
    strncpy(ip_destino, inet_ntoa(ip_header->ip_dst), 15);
    snprintf(tamanio, sizeof(tamanio), "%d", header->len);

    // Identificar protocolo y puertos
    switch (ip_header->ip_p) {
        case IPPROTO_TCP: {
            struct tcphdr *tcp = (struct tcphdr *)
                (packet + 14 + ip_header->ip_hl * 4);
            puerto_origen  = ntohs(tcp->source);
            puerto_destino = ntohs(tcp->dest);
            snprintf(protocolo, sizeof(protocolo), "TCP");
            break;
        }
        case IPPROTO_UDP: {
            struct udphdr *udp = (struct udphdr *)
                (packet + 14 + ip_header->ip_hl * 4);
            puerto_origen  = ntohs(udp->source);
            puerto_destino = ntohs(udp->dest);
            snprintf(protocolo, sizeof(protocolo), "UDP");
            break;
        }
        case IPPROTO_ICMP:
            snprintf(protocolo, sizeof(protocolo), "ICMP");
            break;
        default:
            snprintf(protocolo, sizeof(protocolo), "OTRO");
            break;
    }

    // Mostrar en pantalla
    printf("[%s] %s:%d → %s:%d (%s bytes)\n",
           protocolo,
           ip_origen,  puerto_origen,
           ip_destino, puerto_destino,
           tamanio);

    // Guardar en la DB
    char p_origen_str[10], p_destino_str[10];
    snprintf(p_origen_str,  sizeof(p_origen_str),  "%d", puerto_origen);
    snprintf(p_destino_str, sizeof(p_destino_str), "%d", puerto_destino);

    const char *params[6] = {
        protocolo,
        ip_origen,
        ip_destino,
        p_origen_str,
        p_destino_str,
	tamanio
    };

    PGresult *res = PQexecParams(conn,
        "INSERT INTO trafico "
        "(protocolo, ip_origen, ip_destino, puerto_origen, puerto_destino, tamanio) "
        "VALUES ($1, $2, $3, $4, $5, $6)",
        6, NULL, params, NULL, NULL, 0
    );

    PQclear(res);
}

// ─────────────────────────────────────
//  Main
// ─────────────────────────────────────
int main() {
    char errbuf[PCAP_ERRBUF_SIZE];

    // Conectar a la DB
    PGconn *conn = conectar();
    printf("✓ Conectado a la DB\n");

    // Abrir interfaz eth0 para captura
    pcap_t *handle = pcap_open_live("eth0", BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        printf("Error al abrir interfaz: %s\n", errbuf);
        return 1;
    }
    printf("✓ Capturando en eth0...\n");
    printf("  Presioná Ctrl+C para detener\n\n");

    // Filtrar solo tráfico IP
    struct bpf_program fp;
    pcap_compile(handle, &fp, "ip", 0, PCAP_NETMASK_UNKNOWN);
    pcap_setfilter(handle, &fp);

    // Capturar indefinidamente
    pcap_loop(handle, 0, procesar_paquete, (u_char *)conn);

    pcap_close(handle);
    desconectar(conn);
    return 0;
}
