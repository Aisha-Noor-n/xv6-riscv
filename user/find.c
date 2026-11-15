#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/param.h"

// cmd points at the command's argv (argv[4] from main), ncmd is how many
// entries it has. If cmd == 0, we're in plain "print the path" mode.
void
find(char *path, char *target, char **cmd, int ncmd)
{
  char buf[512];
  char *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if(st.type == T_FILE){
    close(fd);

    p = path + strlen(path);
    while(p > path && *(p - 1) != '/')
      p--;

    if(strcmp(p, target) == 0){
      if(cmd == 0){
        printf("%s\n", path);
      } else {
        int pid = fork();
        if(pid < 0){
          fprintf(2, "find: fork failed\n");
        } else if(pid == 0){
          // child: build argv = cmd args + matched path + terminating 0
          char *args[MAXARG];
          int i;
          for(i = 0; i < ncmd; i++)
            args[i] = cmd[i];
          args[ncmd] = path;
          args[ncmd + 1] = 0;
          exec(args[0], args);
          // only reached if exec failed
          fprintf(2, "find: exec %s failed\n", args[0]);
          exit(1);
        } else {
          wait(0);
        }
      }
    }
    return;
  }

  if(st.type != T_DIR){
    close(fd);
    return;
  }

  if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)){
    fprintf(2, "find: path too long\n");
    close(fd);
    return;
  }

  strcpy(buf, path);
  p = buf + strlen(buf);
  *p++ = '/';

  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if(de.inum == 0)
      continue;

    if(strcmp(de.name, ".") == 0 ||
       strcmp(de.name, "..") == 0)
      continue;

    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;

    find(buf, target, cmd, ncmd);
  }

  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc == 3){
    find(argv[1], argv[2], 0, 0);
  } else if(argc >= 5 && strcmp(argv[3], "-exec") == 0){
    int ncmd = argc - 4;
    if(ncmd > MAXARG - 2){   // leave room for the path and the 0
      fprintf(2, "find: too many command arguments\n");
      exit(1);
    }
    find(argv[1], argv[2], &argv[4], ncmd);
  } else {
    fprintf(2, "usage: find path name [-exec cmd ...]\n");
    exit(1);
  }

  exit(0);
}