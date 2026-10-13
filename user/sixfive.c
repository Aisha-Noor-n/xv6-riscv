#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Define the separators as requested
const char *separators = " -\r\t\n./,";

void process(int fd) {
    char c;
    char buf[128]; 
    int i = 0;
    
    // States: 0 = searching, 1 = reading valid number, 2 = reading invalid token
    int state = 0; 

    // Read the file one character at a time
    while (read(fd, &c, 1) > 0) {
        if (strchr(separators, c) != 0) { 
            // We hit a separator
            if (state == 1) {
                buf[i] = '\0';
                int num = atoi(buf);
                if (num % 5 == 0 || num % 6 == 0) {
                    printf("%d\n", num);
                }
            }
            // Reset state for the next token
            state = 0;
            i = 0;
        } else if (c >= '0' && c <= '9') { 
            // We hit a digit
            if (state == 0) {
                state = 1;
                i = 0;
                buf[i++] = c;
            } else if (state == 1) {
                if (i < sizeof(buf) - 1) {
                    buf[i++] = c;
                }
            }
        } else { 
            // Not a separator and not a digit (e.g., 'x' or 'v' in "xv6")
            state = 2; 
        }
    }
    
    // The end of the file is an implicit separator
    if (state == 1) {
        buf[i] = '\0';
        int num = atoi(buf);
        if (num % 5 == 0 || num % 6 == 0) {
            printf("%d\n", num);
        }
    }
}

int main(int argc, char *argv[]) {
    int fd;

    // If no arguments, read from standard input (file descriptor 0)
    if (argc <= 1) {
        process(0);
    } else {
        // Otherwise, process each file argument
        for (int i = 1; i < argc; i++) {
            if ((fd = open(argv[i], 0)) < 0) {
                fprintf(2, "sixfive: cannot open %s\n", argv[i]);
                continue;
            }
            process(fd);
            close(fd);
        }
    }
    
    exit(0);
}