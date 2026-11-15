#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  // Check if the user passed the number of ticks argument
  if (argc < 2) {
    fprintf(2, "Usage: sleep ticks\n");
    exit(1);
  }

  // Convert the argument string to an integer and call the system call
  int ticks = atoi(argv[1]);
  pause(ticks);  // Use sleep(ticks) if your specific xv6 version uses the 'sleep' system call instead

  exit(0);
}