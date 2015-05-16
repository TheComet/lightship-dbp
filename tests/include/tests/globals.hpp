#include "util/config.h" /* use util's config file for determining platform */

/*
 * The regular expression to use for death testing assertions, based on
 * platform and build configuration.
 */
#ifdef _DEBUG
#   ifdef LIGHTSHIP_UTIL_PLATFORM_MACOSX
#       define ASSERTION_REGEX ".*Assertion failed.*"
#   elif defined(LIGHTSHIP_UTIL_PLATFORM_LINUX)
#       define ASSERTION_REGEX "\\bAssertion `\\w+' failed."
#   else
#       define ASSERTION_REGEX ""
#   endif
#else
#   define ASSERTION_REGEX ""
#endif
