#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void memdump(char *fmt, char *data, int len) {
    char *p = data;
    char *end = data + len;

    for (int i = 0; fmt[i] != '\0'; i++) {
        char f = fmt[i];

        // i: 32-bit Integer, decimal (4 bytes)
        if (f == 'i') {
            if (p + 4 > end) {
                printf("memdump: not enough data for '%c'\n", f);
                return;
            }
            printf("%d\n", *(int *)p);
            p += 4;
        } 
        // p: 64-bit integer, hex (8 bytes)
        else if (f == 'p') {
            if (p + 8 > end) {
                printf("memdump: not enough data for '%c'\n", f);
                return;
            }
            printf("%p\n", (void *)*(uint64 *)p);
            p += 8;
        } 
        // h: 16-bit integer, decimal (2 bytes)
        else if (f == 'h') {
            if (p + 2 > end) {
                printf("memdump: not enough data for '%c'\n", f);
                return;
            }
            printf("%d\n", *(short *)p);
            p += 2;
        } 
        // c: 8-bit ASCII character (1 byte)
        else if (f == 'c') {
            if (p + 1 > end) {
                printf("memdump: not enough data for '%c'\n", f);
                return;
            }
            printf("%c\n", *p);
            p += 1;
        } 
        // s: 64-bit pointer to a C string (8 bytes)
        else if (f == 's') {
            if (p + 8 > end) {
                printf("memdump: not enough data for '%c'\n", f);
                return;
            }
            char *str = *(char **)p;
            printf("%s\n", str);
            p += 8;
        } 
        // S: print string up to null byte or end of valid data
        else if (f == 'S') {
            while (p < end && *p != '\0') {
                printf("%c", *p);
                p++;
            }
            printf("\n");
            // Consume the null byte if we stopped because of it
            if (p < end && *p == '\0') {
                p++;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    // If invoked with an argument, read standard input to EOF
    if (argc > 1) {
        char buf[1024];
        int len = 0;
        int n;
        
        while ((n = read(0, buf + len, sizeof(buf) - len)) > 0) {
            len += n;
        }
        
        memdump(argv[1], buf, len);
    } else {
        printf("Error: Missing test harness for zero arguments.\n");
        printf("Run with an argument (e.g., echo a | memdump i) to use standard input.\n");
    }
    
    exit(0);
}