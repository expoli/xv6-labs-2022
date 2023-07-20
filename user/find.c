#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
find(char *path, const char *target)
{
  int fd;
  struct dirent de;
  struct stat st;
  char buf[512], *file_name, *p;

  //printf("path: %s, target: %s\n", path, target);

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
    file_name = fmtname(path);
    if(strcmp(file_name, target) == 0){
      printf("%s\n", path);
      return;
    }
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){ // 再放一个 /path/
      printf("ls: path too long\n");
      break;
    }
    //printf("T_DIR: before cpy %s\n", buf);
    strcpy(buf, path);
    //printf("T_DIR: after cpy %s\n", buf);
    p = buf+strlen(buf);
    *p++ = '/'; // 在目录后面添加/
    //printf("T_DIR: after / %s\n", buf);
    while(read(fd, &de, sizeof(de)) == sizeof(de)){ // 读取一个目录项
      if(de.inum == 0)
        continue;
      if(!strcmp(de.name, ".") || !strcmp(de.name, ".."))
        continue;
      memmove(p, de.name, strlen(de.name));
      p[strlen(de.name)] = 0;
      //printf("before stat %s:strlen name:%d\n", buf, strlen(de.name));
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      if(strcmp(de.name, target) == 0)
          printf("%s\n", buf);
      if(st.type == T_DIR){
        //printf("-----find: %s-----\n", buf);
        find(buf, target);
      }
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 3){
    printf("Usage: find dir target\n");
    exit(0);
  }

  find(argv[1], argv[2]);

  exit(0);
}

