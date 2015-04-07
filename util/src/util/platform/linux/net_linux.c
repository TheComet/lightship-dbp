#include "util/net.h"
#include "util/log.h"
#include "util/memory.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

/* ------------------------------------------------------------------------- */
char
net_init(void)
{
    return 1;
}

/* ------------------------------------------------------------------------- */
void
net_deinit(void)
{
}

/* ------------------------------------------------------------------------- */
struct net_connection_t*
net_host_udp(const char* node, const char* port, uint32_t max_connections)
{
    struct addrinfo hints, *res, *p, *addr;
    struct net_connection_t* connection;
    
    /* fill in hints */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;    /* allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* UDP */
    hints.ai_flags = AI_PASSIVE;    /* fill in my IP for me */
    
    /* get address info matching above hints */
    if(getaddrinfo(NULL, port, &hints, &res) != 0)
    {
        llog(LOG_ERROR, NULL, 1, "getaddrinfo() failed in net_host_udp()");
        return NULL;
    }
    
    /* Filter through list and find a suitable address. Prefer IPv6 over IPv4 */
    addr = NULL;
    for(p = res; p != NULL; ++p)
    {
        /* Found IPv4. If address not yet determined, set to IPv4 */
        if(!addr && p->ai_family == AF_INET)
            addr = p;
        
        /* Found IPv6. If address not yet determined, set to IPv6. If address
         * already determined, only set to IPv6 if the determined address is
         * not IPv6 */
        if(p->ai_family == AF_INET6)
            if(!addr || addr->ai_family != AF_INET6)
                addr = p;
    }
    
    /* create connection object */
    connection = (struct net_connection_t*)MALLOC(sizeof connection);
    if(!connection)
        OUT_OF_MEMORY("net_host_udp()", NULL);
    memset(connection, 0, sizeof *connection);
    connection->max_connections = max_connections;
    
    /* create socket */
    connection->sockfd = socket(addr->ai_family,
                                addr->ai_socktype,
                                addr->ai_protocol);
    if(connection->sockfd < 0)
    {
        llog(LOG_ERROR, NULL, 1, "Failed to create socket()");
        net_disconnect(connection);
        return NULL;
    }
    
    /* bind the socket */
    if(bind(connection->sockfd, addr->ai_addr, addr->ai_addrlen) < 0)
    {
        llog(LOG_ERROR, NULL, 1, "Failed to bind() socket.");
        net_disconnect(connection);
        return NULL;
    }
    
    /* done with address info */
    freeaddrinfo(res);
    
    return connection;
}

/* ------------------------------------------------------------------------- */
struct net_connection_t*
net_join_udp(const char* node, const char* port)
{
    struct addrinfo hints, *res, *p, *addr;
}

/* ------------------------------------------------------------------------- */
void
net_disconnect(struct net_connection_t* connection)
{
    if(connection->sockfd)
        close(connection->sockfd);
    FREE(connection);
}
