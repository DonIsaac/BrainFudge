#include <assert.h>
#include <sysexits.h>
#include <limits.h>
#include <sys/wait.h>

#include "unit.h"

int test_loadf();
int test_cli_program();

test_suite_t test_suite_unit[NUM_UNIT_TESTS] = {
    &test_loadf,
    &test_cli_program};

int unit_get_suite(test_suite_t **suite_out, int *num_tests)
{
    (*suite_out) = &test_suite_unit;
    (*num_tests) = NUM_UNIT_TESTS;
}

int test_loadf()
{
    char *actual;
    char filename[STRLEN] = "test/data/loadf1.txt";
    char expected[STRLEN] = "this file has 28 characters\n";
    int status;

    status = loadf(filename, &actual);

    if (status)
        return status;

    if (strcmp(actual, expected))
    {
        fprintf(stderr, "Assertion failed: test_loadf.\n\tExpected: \"%s\"\n\tActual: \"%s\"\n", expected, actual);
        bf_print_trace();
        status = 1;
        return 1;
    }
    else
    {
        status = 0;
    }

    free(actual);
    return status;
}

int test_cli_program()
{
    /**
     * "progs/add.bf" program with whitespace/comments removed
     */
    char program[STRLEN] = "++>+++++[<+>-]++++++++[<++++++>-]<.";
    char expected = '7';
    // Output received from interpreter child process
    char actual[1] = {'\0'};

    pid_t child;
    int status, to_child, from_child;
    // Parent writes, child reads via stdin (after piping)
    int parent_to_child[2];
    // Parent reads, child writes via stdout (after piping)
    int child_to_parent[2];
    // Absolute path to interpreter executable
    char interp_bin[2048] = {'\0'};

    if (
        !getcwd(interp_bin, PATH_MAX) || // Parent process' CWD needed to find interp binary
        pipe(parent_to_child) < 0 ||
        pipe(child_to_parent))
    {
        bf_err("Child process setup failed.");
        return EX_OSERR;
    }
    assert(interp_bin[strlen(interp_bin) - 1] == '/');
    strcat("bin/interp", interp_bin);

    if ((child = fork()) < 0)
    {
        bf_err("Fork failed.");
        return EX_OSERR;
    }

    if (child) // Parent code
    {
        to_child = parent_to_child[1]; // Parent writes to child's input from here
        close(parent_to_child[0]);     // Child reads from here, parent can close their end

        from_child = child_to_parent[0]; // Parent reads child's output from here
        close(child_to_parent[1]);       // Parent isn't reading this, can close its side

        read(from_child, actual, 1);

        waitpid(child, &status, 0); // Wait for child to finish executing
        if (!WIFEXITED(status))
            return 1;

        if (WEXITSTATUS(status))
        {
            bf_err("Interpreter exited with non-zero code.");
            return WEXITSTATUS(status);
        }

        if (actual[0] != expected)
        {
            fprintf(stderr, "Assertion failed: test_cli_program.\n\tExpected: \'%c\'\n\tActual: \'%c\'\n", expected, actual[0]);
            return 1;
        }

        return 0;
    }
    else // Child code
    {
        dup2(parent_to_child[0], STDIN_FILENO);
        close(parent_to_child[1]);

        dup2(child_to_parent[1], STDOUT_FILENO);
        close(child_to_parent[0]);

        exit(execl(interp_bin, program, NULL));

        return 1; // Only reached if exit fails
    }
}
