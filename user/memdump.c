#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
memdump(char *fmt, char *data, int len)
{
  int i;

  for(i = 0; fmt[i] != '\0'; i++){
    switch(fmt[i]){
    case 'i':
      if(len < 4){
        printf("memdump: not enough data for 'i'\n");
        return;
      }
      printf("%d\n", *(int *)data);
      data += 4;
      len -= 4;
      break;

    case 'p':
      if(len < 8){
        printf("memdump: not enough data for 'p'\n");
        return;
      }
      printf("%lx\n", *(uint64 *)data);
      data += 8;
      len -= 8;
      break;

    case 'h':
      if(len < 2){
        printf("memdump: not enough data for 'h'\n");
        return;
      }
      printf("%d\n", *(short *)data);
      data += 2;
      len -= 2;
      break;

    case 'c':
      if(len < 1){
        printf("memdump: not enough data for 'c'\n");
        return;
      }
      printf("%c\n", *data);
      data++;
      len--;
      break;

    case 's':
      if(len < 8){
        printf("memdump: not enough data for 's'\n");
        return;
      }
      printf("%s\n", *(char **)data);
      data += 8;
      len -= 8;
      break;

    case 'S':
      while(len > 0 && *data != '\0'){
        printf("%c", *data);
        data++;
        len--;
      }
      printf("\n");
      return;
    }
  }
}

int
main(int argc, char *argv[])
{
  char data[1024];
  int len;
  int n;

  if(argc == 1){
    exit(0);
  }

  len = 0;

  while(len < sizeof(data)){
    n = read(0, data + len, sizeof(data) - len);

    if(n < 0){
      fprintf(2, "memdump: read error\n");
      exit(1);
    }

    if(n == 0)
      break;

    len += n;
  }

  memdump(argv[1], data, len);

  exit(0);
}

