#ifndef __COMPILE_H__
#define __COMPILE_H__

#include <stdio.h>

/**
 * @brief Compiles a Brainfuck program to assembly.
 *
 * @param program The program source code to compile
 * @param out     The file the assembly will be written to
 * @return int    Status code, 0 for success, non-zero otherwise.
 */
int compile(const char *program, FILE *out);

#endif
