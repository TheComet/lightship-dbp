#include "util/net.h"
#include <winsock2.h>

/* ------------------------------------------------------------------------- */
char
net_init(void)
{
    WSADATA wsaData;
    return (WSAStartup(MAKEWORD(2, 2), &wsaData) == NO_ERROR);
}

/* ------------------------------------------------------------------------- */
void
net_deinit(void)
{
    WSACleanup();
}

/* ------------------------------------------------------------------------- */
