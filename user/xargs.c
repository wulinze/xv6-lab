#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"


void
xargs_exec(char* program, char** paraments)
{
    if (fork() > 0) {
        wait(0);
    } else {
        if (exec(program, paraments) == -1) {
            fprintf(2, "xargs: Error exec %s\n", program);
        }
    }
}

void
xargs(char* param[], int size, char* cmd)
{
    char buf[1024];
    char *arg[MAXARG];
    int m = 0;

    int i;
    for (i=0; i<size; i++) {
        arg[i] = (char*)malloc(sizeof(param[i]));
        memcpy(arg[i], param[i], sizeof(param[i]));
    }
    while (read(0, buf+m, 1) == 1) {
        if (m >= 1024) {
            fprintf(2, "xargs: arguments too long.\n");
            exit(1);
        }
        if (buf[m] == '\n') {
            buf[m] = '\0';
            arg[i] = (char*)malloc(m+1);
            memmove(arg[i], buf, m+1);
            arg[i+1] = 0;
            // exec(char*, char** paraments): paraments ending with zero
            xargs_exec(cmd, arg);
            free(arg[i]);
            m = 0;
        } else {
            m++;
        }
    }
}

int
main(int argc, char* argv[])
{
    char *command = "echo";
    if (argc >= 2) {
        command = argv[1];
    }

    xargs(argv + 1, argc - 1, command);
    exit(0);
}
// echo '1\n2' | xargs echo line