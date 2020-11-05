#ifndef __FILE_H__
#define __FILE_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>

/**
 * Reads a file and stores its contents in memory as a pre-malloc'd,
 * null-terminated string. Make sure to free `out` before program termination.
 *
 * @param file The path to the file to read.
 * @param out  A pointer to the string where the file contents will be written.
 *
 * @returns 0 on success, an error code otherwise.
 */
int loadf(char *file, char **out);

#endif
