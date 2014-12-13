#include <stdio.h>

/*!
 * @brief Concatenates n number of strings and prints it to the specified file.
 * @param file The file to print to.
 * @param num_strs The number of strings that should be concatenated.
 * @param ...str A list of comma-seperated strings to concatenate.
 */
void fprintf_strings(FILE* file, int num_strs, ...);

/*!
 * @brief Concatenates n number of strings and returns the buffer.
 * @note The buffer needs to be freed manually when it is no longer required.
 * @param num_strs The number of strings that should be concatenated.
 * @param ...str A list of comma-seperated strings to concatenate.
 * @return The new buffer containing all concatenated strings.
 */
char* cat_strings(int num_strs, ...);

/*!
 * @brief Copies the specified into a new buffer and returns it.
 * @note The buffer needs to be freed manually when it is no longer required.
 * @param str The string to copy.
 * @return The new buffer containing a copy of the input string.
 */
char* malloc_string(const char* str);

/*!
 * @brief Checks if a character is a number or not.
 * @return Returns 1 if it is a number, 0 if otherwise.
 */
int is_number(const char chr);
