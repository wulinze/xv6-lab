#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int 
main(int argc, char* argv[])
{
    int fd1[2], fd2[2];
    char msg[100];
    int err;
    if((err = pipe(fd1)) != 0) {
        fprintf(2, "Error in pipe sys_call!\n");
        exit(1);
    }
    if((err = pipe(fd2)) != 0) {
        fprintf(2, "Error in pipe sys_call!\n");
        exit(1);
    }

    int pid = fork();
    msg[0]  = '\0'; 
    if (pid > 0) {
        if (write(fd1[1], "ping", 4) == 0) {
            fprintf(2, "father Error in write sys_call!\n");
            exit(1);
        }
        if (read(fd2[0], msg, 4) < 0) {
            fprintf(2, "Error in fork sys_call!\n");
            exit(1);
        } else {
            printf("%d: received %s\n", getpid(), msg);
        }
    } else if (pid == 0){
        if (read(fd1[0], msg, 4) < 0) {
            fprintf(2, "Error in fork sys_call!\n");
            exit(1);
        } else {
            printf("%d: received %s\n", getpid(), msg);
            if (write(fd2[1], "pong", 4) == 0) {
                fprintf(2, "Error in write sys_call!\n");
                exit(1);
            }
        }
    } else {
        fprintf(2, "Error in fork sys_call!\n");
        exit(1);
    }
    
    exit(0);
}