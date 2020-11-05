#include "unit.h"
#include "../lib/err.h"

int test_loadf();

test_suite_t test_suite_unit[NUM_UNIT_TESTS] = {
    &test_loadf
};

int unit_get_suite(test_suite_t **suite_out, int *num_tests)
{
    (*suite_out) = &test_suite_unit;
    (*num_tests) = NUM_UNIT_TESTS;
}

int test_loadf() {
    char* actual;
    char filename[STRLEN] = "test/data/loadf1.txt";
    char expected[STRLEN] = "this file has 28 characters\n";
    int status;

    status = loadf(filename, &actual);

    if (status) return status;

    if(strcmp(actual, expected))
    {
        fprintf(stderr, "Assertion failed: test_loadf.\n\tExpected: \"%s\"\n\tActual: \"%s\"\n", expected, actual);
        bf_print_trace();
        status = 1;
        return 1;
    } else {
        status = 0;
    }

    free(actual);
    return status;

}
