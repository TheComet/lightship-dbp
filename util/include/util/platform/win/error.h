/*
 * @brief Returns the last Win32 error as a char array.
 * @note The allocated string is allocated with malloc() and MUST be freed
 * manually.
 */
char* get_last_error_string(void);