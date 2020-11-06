#include "err.h"

void bf_err(char *msg)
{
    void *array[ERR_TRACE_COUNT];
    char **frames;
    int num_frames, i;

    num_frames = backtrace(array, 10);
    frames = backtrace_symbols(array, num_frames);

    if (msg)
    {
        fprintf(stderr, "Error: %s\n", msg);
    }
    else
    {
        fprintf(stderr, "Error: An error occurred during execution.\n");
    }

    if (frames)
    {
        // start at 1 to skip current fn
        for (i = 1; i < num_frames; i++)
            fprintf(stderr, "\tat %s\n", frames[i]);

    }

    free(frames);
}

void bf_print_trace() {

    void *array[ERR_TRACE_COUNT];
    char **frames;
    int num_frames, i;

    num_frames = backtrace(array, 10);
    frames = backtrace_symbols(array, num_frames);


    if (frames)
    {
        // start at 1 to skip current fn
        for (i = 1; i < num_frames; i++)
            fprintf(stderr, "\tat %s\n", frames[i]);

    }

    free(frames);
}
