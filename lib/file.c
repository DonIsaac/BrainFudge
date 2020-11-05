
#include "file.h"

int loadf(char *file, char **out)
{
    FILE *fp;
    int status, file_len, i, ch;
    struct stat st;

    if (!file || !out)
    {
        return 1;
    }
    else if ((status = stat(file, &st)))
    {
        fprintf(stderr, "Could not read file %s: stat exited with code %d.\n", file, status);
        return status;
    }
    else if (!(st.st_mode & R_OK))
    {
        fprintf(stderr, "Unable to read file %s: file is unreadable. Check your read permissions.\n", file);
        return 1;
    }

    file_len = st.st_size;

    // Allocate memory for program
    if (NULL == (*out = malloc(file_len + 2)))
    {
        fprintf(stderr, "Could not allocate memory for program source code.\n");
        return 1;
    }

    // Load in program from source file
    if (!(fp = fopen(file, "r")))
    {
        fprintf(stderr, "Could not open file %s.\n", file);
        return 1;
    }
    for (i = 0; (ch = fgetc(fp)) != EOF; i++)
    {
        (*out)[i] = (char)ch;
    }

    (*out)[i] = '\0'; // Add null termination to program string
    fclose(fp);

    return 0;
}
