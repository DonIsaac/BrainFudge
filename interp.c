#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>

#include "interp.h"

// Get location of address in memory tape
#define ADDR(X) (X - memory)
// Get location of instruction character in program string
#define INSTR(X) (X - program)

int interpret(char *program)
{
    // memory tape
    char memory[MEMORY_TAPE_SIZE] = {0};

    char *di = memory,  // data pointer
        *pc = program,  // program counter
            *ax = NULL; // general purpose pointer

    unsigned int depth = 0, i;
    /*     for (i = 0; i < MEMORY_TAPE_SIZE; i++)
    {
        memory[i] = '\0';
    } */

    while (*pc != '\0')
    {
        if (DEBUG)
        {
            printf("Executing '%c' at %ld.\n", *pc, INSTR(pc));
        }
        switch (*pc)
        {
        // Increment data pointer
        case '>':
            if (di == ((unsigned long int)memory) + MEMORY_TAPE_SIZE)
            {
                fprintf(stderr, "Runtime Error ('%c':%ld): Tape memory exceeded.\n", *pc, INSTR(pc));
                return 1;
            }
            else
            {
                ++di;
                if (DEBUG)
                    printf("Data pointer now points to: %ld.\n", ADDR(di));
            }
            break;

        // Decrement data pointer
        case '<':
            if (di == memory)
            {
                fprintf(stderr, "Runtime Error ('%c':%ld): Cannot decrement data pointer before start of memory.\n", *pc, INSTR(pc));
                return 1;
            }
            else
            {
                --di;
                if (DEBUG)
                    printf("Data pointer now points to: %ld.\n", ADDR(di));
            }
            break;

        // Incr cell data pointer is pointing to
        case '+':
            ++*di;
            break;

        // Decr cell data pointer is pointing to
        case '-':
            --*di;
            break;

        // Print memory cell to output stream
        case '.':
            putchar(*di);
            break;

        // Read byte from input stream
        case ',':
            *di = getchar();
            break;

        case '[':

            // Only jump if cell is zero
            if (!(*di))
            {
                depth = 0;
                ax = pc;
                ++pc;

                // [ + [ - ] + ]
                //             ^
                // pc
                // depth: 0

                // while matching brace hasn't been found, increment pc
                while (*pc != ']' || depth != 0)
                {
                    if (DEBUG)
                    {
                        printf("In '[' loop. pc: %ld, instr: '%c', depth: %d.\n", INSTR(pc), *pc, depth);
                    }

                    switch (*pc)
                    {
                    case '[':
                        ++depth;
                        break;
                    case ']':
                        --depth;
                        break;
                    case '\0':
                        fprintf(stderr, "Syntax error ('%c':%ld): expected closing ']', got EOF.\n", *pc, INSTR(ax));
                        return 1;
                    }

                    ++pc;
                }

                // pc++;
            }

            break;

        case ']':

            if (*di)
            {
                depth = 0;
                // store the start of the loop for error output
                ax = pc;
                pc--;

                while (*pc != '[' || depth != 0)
                {

                    if (DEBUG)
                    {
                        printf("In ']' loop. pc: %ld, instr: '%c', depth: %d.\n", INSTR(pc), *pc, depth);
                    }

                    switch (*pc)
                    {
                    case ']':
                        ++depth;
                        break;
                    case '[':
                        --depth;
                        break;
                    }

                    // program counter has decremented past the beginning of the program
                    if (pc == memory)
                    {
                        fprintf(stderr, "Syntax error ('%c':%ld): no matching '[' instruction.\n", *pc, INSTR(ax));
                        return 1;
                    }
                    else
                    {
                        --pc;
                    }
                }

                // TODO: is this correct?
                // pc++;
            }
            break;

        default:
            // if (isspace(*pc))
            // break;

            // fprintf(stderr, "Syntax error ('%c':%ld): invalid instruction '%c'.\n", *pc, INSTR(pc), *pc);
            // return 1;
            break;
        } // !switch

        ++pc;
    }

    return 0;
}

int bf_exec(char *program, char *memory, int memory_size)
{
    return 0;
}

/*
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
*/
