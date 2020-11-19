
#define NUM_PROGS 3

// TODO we may not want to force debug output
#define DEBUG 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>

#include "../lib/file.h"
#include "../src/interp.h"
#include "types.h"
#include "unit.h"

/** Total number of test suites */
#define NUM_TESTS (NUM_PROGS + NUM_UNIT_TESTS)
#define TEST_FAIL (num_failed++, status[test_idx] = 'X')
#define TEST_PASS status[test_idx] = 'O'
#define TEST_SKIP status[test_idx] = '-'

/**
 * Status string, each character represents the status of a test suite.
 * '-' means the test wasn't run, 'O' means it passed, 'X' means it failed.
 */
char status[NUM_TESTS] = {'-'};
char progs[NUM_PROGS][STRLEN] = {
    "progs/hello_world.bf",
    "progs/hello_world2.bf",
    "progs/add.bf"};

int num_failed = 0;

int main()
{
    int test_result = 0, test_idx = 0;
    // test_suite_t *suite;
    // int tests_in_suite = 0;
    char *prog = NULL;

    // if(unit_get_suite(&suite, &tests_in_suite)) {
    //     fprintf(stderr, "Could not get test suite.\n");
    //     return 1;
    // }

    // Run unit tests
    for (int unit_idx = 0; unit_idx < NUM_UNIT_TESTS; unit_idx++, test_idx++)
    {
        test_result = (*test_suite_unit[unit_idx])();
        if (test_result)
        {
            TEST_FAIL;
        }
        else
        {
            TEST_PASS;
        }
    }

    // Run interpreter tests
    for (int prog_idx = 0; prog_idx < NUM_PROGS; prog_idx++, test_idx++)
    {
        printf("\nExecuting program \"%s\"...\n", progs[prog_idx]);

        if (loadf(progs[prog_idx], &prog))
        {
            TEST_FAIL;
        }
        else
        {
            test_result = interpret(prog);
            if (test_result)
            {
                TEST_FAIL;
            }
            else
            {
                TEST_PASS;
            }
        }

        free(prog);
    }

    printf("\n========== TEST RESULTS ==========\n");
    printf("%s\n", status);
    printf("%d/%d test suites failed.\n", num_failed, NUM_TESTS);
    printf("==================================\n");

    return num_failed;
}
