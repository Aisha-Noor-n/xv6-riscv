#include "kernel/types.h"
#include "user/user.h"
int main(int argc, char *argv[]) {
    if(argc > 3) {
        interpose(atoi(argv[1]), "-");
        exec(argv[3], &argv[3]);
    }
    exit(0);
}
