#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "interp.h"
#include "lib/err.h"

int main(int argc, char **argv)
{
    FILE *program_file = NULL;
    char *program = NULL;
    unsigned long program_length = 0L;
    struct stat st;
    int status, i;

    if (argc <= 1)
    {
        fprintf(stderr, "No program provided.");
        return 1;
    }

    // Program passed via command line arguments
    if (!strcmp(argv[1], "-e") || !strcmp(argv[1], "--eval"))
    {
        if (argc < 3)
        {
            fprintf(stderr, "No program provided. Aborting.");
            return 1;
        }

        program = argv[2];
        status = interpret(program);
        return status;
    }

    // Get the size of the program while simultaneously checking the source file
    if ((status = stat(argv[1], &st)))
    {
        fprintf(stderr, "Could not read file %s: stat exited with code %d.", argv[1], status);
        exit(status);
    }

    if (!(st.st_mode & R_OK))
    {
        fprintf(stderr, "Unable to read file %s: file is unreadable. Check your read permissions.", argv[1]);
        return 1;
    }
    program_length = st.st_size;

    // Allocate memory for program
    if (NULL == (program = malloc(program_length + 2)))
    {
        fprintf(stderr, "Could not allocate memory for program source code.");
        return 1;
    }

    // Load in program from source file
    program_file = fopen(argv[1], "r");
    if (!program_file)
    {
        fprintf(stderr, "Could not open file %s.", argv[1]);
        return 1;
    }
    for (i = 0; !feof(program_file); i++)
    {
        program[i] = fgetc(program_file);
    }
    program[i] = '\0'; // Add null termination to program string
    fclose(program_file);

    // Execute program
    status = interpret(program);
    free(program);
    return status;
}
