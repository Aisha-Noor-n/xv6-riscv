#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void sleep(int ticks);


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(2, "Usage: sleep <ticks>\n");
        exit(1);
    }

    int ticks = atoi(argv[1]);

    if (ticks < 0)
    {
        fprintf(2, "Error: ticks must be a non-negative integer\n");
        exit(1);
    }

    sleep(ticks);
}



void sleep(int ticks)
{
  pause(ticks);
}

