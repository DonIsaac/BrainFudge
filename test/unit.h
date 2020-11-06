#ifndef __TEST_UNIT_H__
#define __TEST_UNIT_H__

#define NUM_UNIT_TESTS 2
#include "types.h"
#include "../lib/err.h"
#include "../lib/file.h"

test_suite_t test_suite_unit[NUM_UNIT_TESTS];
int unit_get_suite(test_suite_t **suite_out, int *num_tests);

#endif
