#include "util/pstdint.h"

struct net_connection_t
{
    int sockfd;
    uint32_t max_connections;
};

/*!
 * @brief Initialises networking.
 * @return Returns 1 if successful, 0 if otherwise.
 */
char
net_init(void);

/*!
 * @brief De-initialises networking.
 */
void
net_deinit(void);

/*!
 * @brief Initialises a host connection.
 * @param[in] node (optional) The address to host on. Set to NULL for auto.
 * @param[in] port The port to bind to.
 * @param[in] max_connections The maximum amount of clients allowed to connect
 * to this address.
 */
struct net_connection_t*
net_host_udp(const char* node, const char* port, uint32_t max_connections);

/*!
 */
struct net_connection_t*
net_join_udp(const char* node, const char* port);

/*!
 */
void
net_disconnect(struct net_connection_t* connection);
