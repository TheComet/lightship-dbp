#include "util/pstdint.h"
#include "util/config.h"

struct net_connection_t
{
	int sockfd;
	uint32_t max_connections;
};

/*!
 * @brief Initialises networking.
 * @return Returns 1 if successful, 0 if otherwise.
 */
LIGHTSHIP_UTIL_PUBLIC_API char
net_init(void);

/*!
 * @brief De-initialises networking.
 */
LIGHTSHIP_UTIL_PUBLIC_API void
net_deinit(void);

/*!
 * @brief Initialises a host connection.
 * @param[in] port The port to bind to.
 * @param[in] max_connections The maximum amount of clients allowed to connect
 * to this address.
 */
LIGHTSHIP_UTIL_PUBLIC_API struct net_connection_t*
net_host_udp(const char* port, uint32_t max_connections);

/*!
 */
LIGHTSHIP_UTIL_PUBLIC_API struct net_connection_t*
net_join_udp(const char* node, const char* port);

/*!
 */
LIGHTSHIP_UTIL_PUBLIC_API void
net_disconnect(struct net_connection_t* connection);
