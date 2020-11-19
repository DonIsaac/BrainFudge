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

/**
 * Increments the program counter, updating the line/column number accordingly.
 */
void pc_next();

/**
 * Decrements the program counter, updating the line/column number accordingly.
 */
void pc_prev();

/* ==========================================================================
   ================================= GLOBALS ================================
   ========================================================================== */

/**
 * Data pointer (or read/write head) of the interpreter. This points to a cell
 * on the memory tape, the value of which is the target for `+` and `-`
 * instructions.
 */
char *di;

/**
 * Program counter. Points to the currently executing instruction in the program.
 * Modifyable with '[' and ']' instructions.
 */
char *pc;

/**
 * General purpose register. Used for storing temporary values.
 */
char *ax;

/**
 * A string containing the Brainfuck program's source code
 */
const char *program = NULL;

/**
 * Line number of the currently executing instruction.
 */
int lineno;
int lineno2; // lineno backup

/**
 * Column number of the currently executing instruction.
 */
int colno;
int colno2; // colno backup

/* ==========================================================================
   ============================= IMPLEMENTATIONS ============================
   ========================================================================== */

int interpret(const char *source)
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
    lineno = lineno2 = colno = colno2 = 0;

    // Execute the program
    status = bf_exec(memory, MEMORY_TAPE_SIZE);

    // Clean up. Not sure if this is needed, but it's here just in case.
    di = pc = ax = program = NULL;
    lineno = lineno2 = colno = colno2 = 0;

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
                ax = pc,
                lineno2 = lineno,
                colno2 = colno;
                pc_next();

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

                    pc_next();
                }

                // pc++;
            }

            break;

        case ']':

            if (*di)
            {
                depth = 0;
                // store the start of the loop for error output
                ax = pc,
                lineno2 = lineno,
                colno2 = colno;

                pc_prev();

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
                        pc_prev();
                    }
                }

                // TODO: is this correct?
                // pc++;
            }
            break;

        default:
            // NOOP
            break;
        } // !switch

        pc_next();
    }
    return 0;
}

void pc_next()
{
    if (*pc == '\n')
    {
        colno = 0;
        lineno++;
    }
    else
    {
        colno++;
    }

    pc++;
}

void pc_prev()
{
    char *tmp;
    int len;

    // I think this needs to be before the lineno/colno update?
    pc--;
    if (*pc == '\n')
    {
        len = 0;
        tmp = pc - 1;

        while (*tmp != '\n' && tmp > program)
        {
            len++;
            tmp--;
        }

        colno = tmp;
        lineno--;
    }
    else
    {
        colno--;
    }
}

int bf_err_runtime(char *msg)
{
    fprintf(stderr, "Runtime Error ('%c', %ld:%ld): %s\n", *pc, lineno, colno, msg);
#if DEBUG
    bf_print_trace();
#endif
    return BF_ERR_RUNTIME;
}

int bf_err_syntax(char *msg)
{
    fprintf(stderr, "Syntax error ('%c', %ld:%ld): %s\n", *ax, lineno2, colno2, msg);
#if DEBUG
    bf_print_trace();
#endif
    return BF_ERR_SYNTAX;
}
