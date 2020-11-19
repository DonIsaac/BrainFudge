#include <stdlib.h>

#include "../lib/err.h"
#include "compile.h"

int compile_incr(FILE *out);
int compile_decr(FILE *out);
int compile_add(FILE *out);
int compile_sub(FILE *out);
int compile_read(FILE *out);
int compile_write(FILE *out);
int compile_loop_begin(FILE *out);
int compile_loop_end(FILE *out);
size_t swrite(const char *str, FILE *out);

char instructions[8] = {
    '>', // incr PC
    '<', // decr PC
    '+', // incr val at PC
    '-', // decr val at PC
    '.', // output byte at PC
    ',', // input byte, write to PC
    '[', // jeq to matching ']'
    ']'  // jne to matching '['
};

char *pc;

int compile(const char *program, FILE *out)
{

    int status = 0;

    if (!program || !out)
        return 1;

    for (pc = program; *pc != '\0'; pc++)
    {
        switch (*pc)
        {
        case '>':
            status = compile_incr(out);
            break;
        case '<':
            status = compile_decr(out);
            break;
        case '+':
            status = compile_add(out);
            break;
        case '-':
            status = compile_sub(out);
            break;
        case '.':
            status = compile_write(out);
            break;
        case ',':
            status = compile_read(out);
            break;
        case '[':
            status = compile_loop_begin(out);
            break;
        case ']':
            status = compile_loop_end(out);
            break;
        default:
            // NOOP
            break;
        }

        if (status) {
            bf_err("Error: An error ocurred during compilation.");
            return status;
        }
    }

    return 0;
}

int compile_incr(FILE *out)
{
    char instr[] = "inc ax\n";
    swrite(instr, out);

    return 0;
}

int compile_decr(FILE *out)
{
    char instr[] = "dec\tax\n";
    swrite(instr, out);

    return 0;
}

int compile_add(FILE *out)
{
    char instr[] = "inc\t[ax]\n";
    swrite(instr, out);

    return 0;
}

int compile_sub(FILE *out)
{
    char instr[] = "dec\t[ax]\n";
    swrite(instr, out);

    return 0;
}

int compile_read(FILE *out)
{
    return 0;
}

int compile_write(FILE *out)
{
    return 0;
}

size_t swrite(const char *str, FILE *out) {
    return fwrite(str, sizeof(char), strlen(str), out);
}
