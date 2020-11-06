#ifndef __LIB_ERR_H__
#define __LIB_ERR_H__

#include <stdlib.h>
#include <stdio.h>
#include <execinfo.h>

#define ERR_TRACE_COUNT 10

void bf_err(char *msg);
void bf_print_trace();

#endif
