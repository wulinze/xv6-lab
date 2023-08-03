#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void prime(int read_fd, int write_fd) {
    close(write_fd);
    int p;
    if (read(read_fd, &p, sizeof(p)) != 4) {
        return;
    } else {
        int fd[2];
        pipe(fd);
        printf("prime %d\n", p);
        int pid = fork();
        if (pid > 0) {
            int num;
            while(read(read_fd, &num, sizeof(num))) {
                if (num % p != 0) {
                    write(fd[1], &num, sizeof(num));
                }
            }
            close(fd[0]);
            close(fd[1]);
            close(read_fd);
            wait(0);
        } else if(pid == 0) {
            prime(fd[0], fd[1]);
        } else {
            fprintf(2, "Error in fork sys_call!\n");
            exit(1);
        }
    }
    
}

int 
main(int argc, char* argv[])
{
    int fd[2];
    int i;

    if (pipe(fd) != 0) {
        fprintf(2, "Error in pipe sys_call!\n");
        exit(1);
    }

    for (i=2; i<=35; i++) {
        write(fd[1], &i, sizeof(i));
    }
    prime(fd[0], fd[1]);

    exit(0);
}