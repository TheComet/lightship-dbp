#include "util/pstdint.h"
#include "util/export.h"

/*!
 * @brief Jenkins one at a time hash.
 * @param[in] key The data to hash.
 * @param[in] len The lenght of the data in bytes.
 * @return Returns a 32-bit hash of the string.
 */
LIGHTSHIP_UTIL_PUBLIC_API uint32_t
hash_jenkins_oaat(const char* key, uint32_t len);
