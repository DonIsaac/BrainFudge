#ifndef __INTERP_H__
#define __INTERP_H__

#include "types.h"


#ifdef DEBUG
#include "lib/err.h"
#endif

/**
 * Error code for a syntax error.
 */
#define BF_ERR_SYNTAX 2
/**
 * Error code for a runtime error.
 */
#define BF_ERR_RUNTIME 4

/**
 * Reads and executes a Brainfuck program. The executing program may read/write
 * from `stdin`/`stdout`.
 *
 * If an error occurs during execution, execution will stop and an error code is
 * returned.
 *
 * - Syntax errors (e.g. '[' has no matching ']' instruction) use the
 *   error code `BF_ERR_SYNTAX`.
 * - Runtime errors (e.g. read head falls off the memory tape) use the error
 *   code `BF_ERR_RUNTIME`.
 *
 * @param program The program's source code as a string.
 *
 * @return        An int representing the exit status. A successful exit results
 *                in 0, 1 if `program` is `NULL`, or an error code if an error
 *                occurs during execution.
 */
int interpret(char *program);

#endif
