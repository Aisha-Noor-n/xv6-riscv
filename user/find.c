#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h" // Needed for MAXARG

// Globals to store the -exec command and its arguments
int use_exec = 0;
char *exec_argv[MAXARG];
int exec_argc = 0;

char* fmtname(char *path) {
    char *p;
    for(p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;
    return p;
}

// Replaces the basic printf with the fork/exec logic
void execute_or_print(char *path) {
    if (!use_exec) {
        printf("%s\n", path);
    } else {
        // The matched file is appended as the final argument
        exec_argv[exec_argc] = path;
        exec_argv[exec_argc + 1] = 0; // Null-terminate the array for exec
        
        if (fork() == 0) {
            // Child process executes the command
            exec(exec_argv[0], exec_argv);
            fprintf(2, "exec %s failed\n", exec_argv[0]);
            exit(1);
        }
        // Parent process waits for the child to finish[cite: 10]
        wait(0);
    }
}

void find(char *path, char *target) {
    char buf[512], *p;
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

    switch(st.type){
    case T_FILE:
        if(strcmp(fmtname(path), target) == 0){
            execute_or_print(path);
        }
        break;

    case T_DIR:
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0)
                continue;
            
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            
            if(stat(buf, &st) < 0){
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            
            if(strcmp(de.name, target) == 0) {
                execute_or_print(buf);
            }

            if(st.type == T_DIR && strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0){
                find(buf, target);
            }
        }
        break;
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if(argc < 3){
        fprintf(2, "Usage: find <path> <filename> [-exec cmd [args...]]\n");
        exit(1);
    }
    
    // Parse the -exec flag and command arguments
    if (argc > 3 && strcmp(argv[3], "-exec") == 0) {
        use_exec = 1;
        for (int i = 4; i < argc; i++) {
            if (exec_argc >= MAXARG - 2) {
                fprintf(2, "find: too many arguments for -exec\n");
                exit(1);
            }
            exec_argv[exec_argc++] = argv[i];
        }
        if (exec_argc == 0) {
            fprintf(2, "find: missing command for -exec\n");
            exit(1);
        }
    }
    
    find(argv[1], argv[2]);
    exit(0);
}