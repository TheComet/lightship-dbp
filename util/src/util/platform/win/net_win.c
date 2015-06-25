#include "util/net.h"
#include <winsock2.h>

/* ------------------------------------------------------------------------- */
char
net_init(void)
{
	/*WSADATA wsaData;
	return (WSAStartup(MAKEWORD(2, 2), &wsaData) == NO_ERROR);*/
	return 1;
}

/* ------------------------------------------------------------------------- */
void
net_deinit(void)
{
	/*WSACleanup();*/
}

/* ------------------------------------------------------------------------- */
struct net_connection_t*
net_host_udp(const char* port, uint32_t max_connections)
{
	return 1;
}

/* ------------------------------------------------------------------------- */
struct net_connection_t*
net_join_udp(const char* node, const char* port)
{
	return NULL;
}

/* ------------------------------------------------------------------------- */
void
net_disconnect(struct net_connection_t* connection)
{
}
