#include <stdio.h>

/*!
 * @brief Concatinates n number of strings and prints it to the specified file.
 * @param file The file to print to.
 * @param num_strs The number of strings that should be concatinated.
 * @param ...str A list of comma-seperated strings to concatinate.
 */
void fprintf_strings(FILE* file, int num_strs, ...);

/*!
 * @brief Concatinates n number of strings and returns the buffer.
 * @note The buffer needs to be freed manually when it is no longer required.
 * @param num_strs The number of strings that should be concatinated.
 * @param ...str A list of comma-seperated strings to concatinate.
 * @return The new buffer containing all concatinated strings.
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