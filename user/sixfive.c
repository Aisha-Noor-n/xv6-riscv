#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
sixfive(int fd)
{
  char c;
  int num = 0;
  int is_number = 1;      // Tracks if the current token is a pure sequence of digits
  int has_chars = 0;      // Tracks if we are inside a token
  char *separators = " -\r\t\n./,";

  while (read(fd, &c, 1) > 0) {
    if (strchr(separators, c)) {
      // End of token reached
      if (has_chars && is_number) {
        if (num % 5 == 0 || num % 6 == 0) {
          printf("%d\n", num);
        }
      }
      // Reset for the next token
      num = 0;
      is_number = 1;
      has_chars = 0;
    } else {
      has_chars = 1;
      if (c >= '0' && c <= '9') {
        if (is_number) {
          num = num * 10 + (c - '0');
        }
      } else {
        // Contains a non-digit character (e.g., letters), so it's not a valid number
        is_number = 0;
      }
    }
  }

  // End of file acts as an implicit separator
  if (has_chars && is_number) {
    if (num % 5 == 0 || num % 6 == 0) {
      printf("%d\n", num);
    }
  }
}

int
main(int argc, char *argv[])
{
  int fd;
  int i;

  if (argc <= 1) {
    // Read from standard input if no arguments are provided
    sixfive(0);
  } else {
    for (i = 1; i < argc; i++) {
      fd = open(argv[i], 0); // 0 corresponds to O_RDONLY
      if (fd < 0) {
        fprintf(2, "sixfive: cannot open %s\n", argv[i]);
        exit(1);
      }
      sixfive(fd);
      close(fd);
    }
  }
  exit(0);
}