#include "util/net.h"
#include "util/memory.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

static char*
get_sockaddr_ip_str(const struct sockaddr* addr);

static void
log_addrinfo_error(const char* message, const struct addrinfo* addr);

static struct net_connection_t*
net_udp(const char* node, const char* port, uint32_t max_connections, char is_host);

/* ------------------------------------------------------------------------- */
/* EXPORTED FUNCTIONS */
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
net_host_udp(const char* port, uint32_t max_connections)
{
	return net_udp(NULL, port, max_connections, 1);
}

/* ------------------------------------------------------------------------- */
struct net_connection_t*
net_join_udp(const char* node, const char* port)
{
	return net_udp(node, port, 1, 0);
}

/* ------------------------------------------------------------------------- */
void
net_disconnect(struct net_connection_t* connection)
{
	if(connection->sockfd != -1)
		close(connection->sockfd);
	FREE(connection);
}

/* ------------------------------------------------------------------------- */
/* STATIC FUNCTIONS */
/* ------------------------------------------------------------------------- */
static char*
get_sockaddr_ip_str(const struct sockaddr* addr)
{
	char* s = NULL;
	
	switch(addr->sa_family)
	{
		case AF_INET:
		{
			struct sockaddr_in *addr_in = (struct sockaddr_in *)addr;
			s = (char*)MALLOC(INET_ADDRSTRLEN);
			inet_ntop(AF_INET, &(addr_in->sin_addr), s, INET_ADDRSTRLEN);
			break;
		}
		
		case AF_INET6:
		{
			struct sockaddr_in6 *addr_in6 = (struct sockaddr_in6 *)addr;
			s = (char*)MALLOC(INET6_ADDRSTRLEN);
			inet_ntop(AF_INET6, &(addr_in6->sin6_addr), s, INET6_ADDRSTRLEN);
			break;
		}
		
		default:
			s = (char*)MALLOC(sizeof(char) * 8);
			strcpy(s, "unknown");
			break;
	}
	
	return s;
}

/* ------------------------------------------------------------------------- */
static void
log_addrinfo_error(const char* message, const struct addrinfo* addr)
{
	/* get ip address from address info struct */
	char* ip_str = get_sockaddr_ip_str(addr->ai_addr);
	
	/* output to stderr */
	fprintf(stderr, "socket info:\n"
					"  family:        %d\n"
					"  type:          %d\n"
					"  protocol:      %d\n"
					"  address:       %s\n\n"
					"constants:\n"
					"  families\n"
					"    AF_UNSPEC:   %d\n"
					"    AF_INET:     %d\n"
					"    AF_INET6:    %d\n"
					"  types\n"
					"    SOCK_STREAM: %d\n"
					"    SOCK_DGRAM:  %d\n",
			addr->ai_family, addr->ai_socktype, addr->ai_protocol, ip_str,
			AF_UNSPEC, AF_INET, AF_INET6,
			SOCK_STREAM, SOCK_DGRAM);
	
	FREE(ip_str);
}

/* ------------------------------------------------------------------------- */
static struct net_connection_t*
net_udp(const char* node, const char* port, uint32_t max_connections, char is_host)
{
	struct addrinfo hints, *res, *p;
	struct net_connection_t* connection;
	
	/* create connection object */
	connection = (struct net_connection_t*)MALLOC(sizeof *connection);
	if(!connection)
	{
		fprintf(stderr, "malloc() failed in net_host_udp() -- not enough memory\n");
		return NULL;
	}
	
	/* init connection */
	memset(connection, 0, sizeof *connection);
	connection->sockfd = -1; /* set to invalid sockfd */
	connection->max_connections = max_connections;
	
	/* fill in hints */
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;    /* allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* UDP */
	hints.ai_flags = AI_PASSIVE;    /* fill in my IP for me */
	
	/* get address info matching above hints */
	if(getaddrinfo(NULL, port, &hints, &res) != 0)
	{
		fprintf(stderr, "getaddrinfo() failed in net_host_udp()\n");
		net_disconnect(connection);
		return NULL;
	}
	
	/* Filter through list and create and bind the first socket we can */
	for(p = res; p != NULL; p = p->ai_next)
	{
		/* create socket */
		connection->sockfd = socket(p->ai_family,
									p->ai_socktype,
									p->ai_protocol);
		if(connection->sockfd == -1)
		{
			log_addrinfo_error("Failed to create socket()\n", p);
			continue;
		}
		
		/* bind socket if host */
		if(is_host)
		{
			if(bind(connection->sockfd, p->ai_addr, p->ai_addrlen) == -1)
			{
				close(connection->sockfd);
				connection->sockfd = -1;
				log_addrinfo_error("Failed to bind() socket\n", p);
				continue;
			}
		}
		
		/* success! */
		break;
	}
	
	/* done with address info */
	freeaddrinfo(res);
	
	/* make sure bind was successful */
	if(p == NULL)
	{
		fprintf(stderr, "Failed to set up host\n");
		net_disconnect(connection);
		return NULL;
	}
	
	return connection;
}
