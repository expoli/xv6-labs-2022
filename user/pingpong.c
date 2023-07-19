#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int p[2];

  char c;

  pipe(p);
  if(fork() == 0){
    //close(0);   // close std read
    //dup(p[0]);  // copy pipe read to std read
    read(p[0], &c, 1);
    fprintf(0, "%d: received %s", getpid(), "ping\n");
    close(p[0]);// close pipe read
    write(p[1], &c, 1);
    close(p[1]);
  } else {
    c = 'F';
    write(p[1], &c, 1);
    close(p[1]);
    read(p[0], &c, 1);
    fprintf(0, "%d: received %s", getpid(), "pong\n");
    close(p[0]);
    wait(0);
  }
  exit(0);
}
