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



/* ==========================================================================
   ================================ FUNCTIONS ===============================
   ========================================================================== */

int bf_exec(char *memory, int memory_size);

/**
 * Throws a runtime error. `msg` is formatted and printed to `stderr`.
 *
 * @param msg the error message to print
 * @returns   `BF_ERR_RUNTIME`
 */
int bf_err_runtime(char *msg);

/**
 * Throws a syntax error. `msg` is formatted and printed to `stderr`.
 *
 * @param msg the error message to print
 * @returns   `BF_ERR_SYNTAX`
 */
int bf_err_syntax(char *msg);



/* ==========================================================================
   ================================ REGISTERS ===============================
   ========================================================================== */

// data pointer, program counter, and a GP register, respectively
char *di, *pc, *ax;

/**
 * A string containing the Brainfuck program's source code
 */
char *program = NULL;



/* ==========================================================================
   ============================= IMPLEMENTATIONS ============================
   ========================================================================== */

int interpret(char *source)
{
    // memory tape that the program reads/writes from.
    char memory[MEMORY_TAPE_SIZE] = {0};
    int status;

    // Check if program is NULL
    if (!source)
    {
        return 1;
    }

    // Initialize registers before executing the program
    program = source;
    di = memory;
    pc = program;
    ax = NULL;

    // Execute the program
    status = bf_exec(memory, MEMORY_TAPE_SIZE);

    di = pc = ax = program = NULL;

    return status;
}

int bf_exec(char *memory, int memory_size)
{
    unsigned int depth = 0, i;

#ifdef DEBUG
    if (!pc)
    {
        bf_err("Pointer counter is NULL. Aborting.");
        return 1;
    }
#endif

    while (*pc != '\0')
    {
#ifdef DEBUG
        printf("Executing '%c' at %ld.\n", *pc, INSTR(pc));
#endif

        switch (*pc)
        {
        // Increment data pointer
        case '>':
            if (di == ((unsigned long int)memory) + MEMORY_TAPE_SIZE)
            {
                return bf_err_runtime("Tape memory exceeded.");
            }
            else
            {
                ++di;
#ifdef DEBUG
                printf("Data pointer now points to: %ld.\n", ADDR(di));
#endif
            }
            break;

        // Decrement data pointer
        case '<':
            if (di == memory)
            {
                return bf_err_runtime("Cannot decrement data pointer below the start of the memory tape.");
            }
            else
            {
                --di;
#ifdef DEBUG
                printf("Data pointer now points to: %ld.\n", ADDR(di));
#endif
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
#ifdef DEBUG
                    printf("In '[' loop. pc: %ld, instr: '%c', depth: %d.\n", INSTR(pc), *pc, depth);
#endif

                    switch (*pc)
                    {
                    case '[':
                        ++depth;
                        break;
                    case ']':
                        --depth;
                        break;
                    case '\0':
                        return bf_err_syntax("Expected closing ']', got EOF.");
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

#ifdef DEBUG
                    printf("In ']' loop. pc: %ld, instr: '%c', depth: %d.\n", INSTR(pc), *pc, depth);
#endif

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
                        return bf_err_syntax("No matching '[' instruction.");
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

int bf_err_runtime(char *msg)
{
    fprintf(stderr, "Runtime Error ('%c':%ld): %s\n", msg, *pc, INSTR(pc));
#if DEBUG
    bf_print_trace();
#endif
    return 1;
}

int bf_err_syntax(char *msg)
{
    fprintf(stderr, "Syntax error ('%c':%ld): %s\n", msg, *pc, INSTR(ax));
#if DEBUG
    bf_print_trace();
#endif
    return 1;
}
