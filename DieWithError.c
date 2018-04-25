#include <stdio.h>  /* for perror() */
#include <stdlib.h> /* for exit() */
#include "DieWithError.h"

void DieWithError(const char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}
