#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path, char* filename)
{
  static char buf[DIRSIZ+1];
  char * p;

  memmove(buf, path, strlen(path));
  p = buf + strlen(path);
  *p++ = '/';
  strcpy(p, filename);
  return buf;
}

void
find(char *path, char* filename)
{
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

  if (st.type != T_DIR) {
    fprintf(2, "find: %s is not a directory\n", path);
    close(fd);
    return;
  }

  if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
    printf("find: path too long\n");
    return;
  }
  strcpy(buf, path);
  p = buf+strlen(buf);
  *p++ = '/';

  while(read(fd, &de, sizeof(de)) == sizeof(de)) {
    if(de.inum == 0)
        continue;
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;
    if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
    }
    switch(st.type){
        case T_DEVICE:
        case T_FILE:
            if (strcmp(filename, de.name) == 0) {
                printf("%s\n", fmtname(path, filename));
            }
            break;

        case T_DIR:
            if (strcmp(".", de.name) == 0 || strcmp("..", de.name) == 0) {
                continue;
            }
            find(buf, filename);
            break;
    }
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 3){
    find(".", argv[1]);
    exit(0);
  }
  find(argv[1], argv[2]);
  exit(0);
}
