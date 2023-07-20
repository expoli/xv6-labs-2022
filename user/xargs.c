#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

int
main(int argc, char *argv[])
{
  int i;
  char *cmd[MAXARG];
  char line[1024];

  if(argc < 2){
    fprintf(2, "Usage: xargs command arguments...\n");
  }

  for(i = 1; i < argc && i < MAXARG - 1; i++)
    cmd[i - 1] = argv[i];

  cmd[i] = 0;
  while (1) {
    int x = argc - 1;
    gets(line, sizeof(line));

    if (line[0] == 0)
      break;

    cmd[x++] = line;    // 有新输入的时候，刷新这一行

    for(char *p = line; *p; p++) {
      if(*p == ' ') {  // 进行参数分割
        *p = 0;
        cmd[x++] = p + 1;
      } else if (*p == '\n') {
        *p = 0;
      }
    }
    if(fork() == 0) {
      exec(cmd[0], cmd);
    }
  }
  wait(0);
  exit(0);

  return 0;
}
 
