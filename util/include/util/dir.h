struct list_t;

/*!
 * @brief Scans a specified directory and saves the listings in the specified
 * list.
 * @note If the list is not empty, then the data is simply appended.
 * @param [in] list The list to store the directory listings in.
 * @param [in] dir The directory to scan (relative or absolute).
 */
void get_directory_listing(struct list_t* list, const char* dir);
