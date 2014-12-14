/*!
 * @brief Tries to load the specified module.
 * @param [in] filename The path and filename of the module to load.
 * @return Returns NULL on failure, returns the handle to the module on
 * success.
 */
void* module_open(const char* filename);

/*!
 * @brief Retrieves a symbol from the module.
 * @param [in] handle The handle of a loaded module to search.
 * @param [in] symbol The string identifying the symbol to search for.
 * @return Returns NULL on failure, returns the address of the symbol on
 * success.
 */
void* module_sym(void* handle, const char* symbol);

/*!
 * @brief Closes the specified module.
 * @param [in] handle The handle of the module to close.
 */
void module_close(void* handle);
