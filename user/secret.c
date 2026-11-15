
#include "kernel/types.h"
#include "kernel/riscv.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc != 2) {
    printf("Usage: secret <string>\n");
    exit(1);
  }

  char *mem = sbrk(4096);
  if (mem == (char *)-1) {
    printf("secret: sbrk failed\n");
    exit(1);
  }

  // Store the secret 32 bytes into the allocated page.
  volatile char *location = mem + 32;

  int i = 0;
  while (argv[1][i] != '\0') {
    location[i] = argv[1][i];
    i++;
  }
  location[i] = '\0';

  printf("Secret stored.\n");
  exit(0);
}
