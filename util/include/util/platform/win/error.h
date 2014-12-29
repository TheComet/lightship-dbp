/*!
 * @brief Returns the last Win32 error as a char array.
 * @note The returned string is allocated with MALLOC() and MUST be FREEd
 * manually.
 */
char*
get_last_error_string(void);
