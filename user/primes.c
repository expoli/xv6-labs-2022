#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
/**
 * `prime` 素数进程通过管道来合作，将数据从左传递到右边。下面是具体的每一步：
 * 1. 第一个进程（`feed`）将数字 2 到 35 输入到管道中。
 * 2. 第一个 `prime` 进程（`prime 2`）从管道中读取第一个数字 2，并将其打印出来。
 * 3. `prime 2` 进程创建一个新的管道，并 `fork` 出一个新的子进程（`prime 3`）。
 * 4. `prime 2` 进程继续从管道中读取数字，并检查它们是否能被 2 整除。
 * 5. 如果一个数字不能被 2 整除，则它会被写入到新的管道中，传递给 `prime 3` 进程。
 * 6. `prime 3` 进程从新的管道中读取第一个数字 3，并将其打印出来。
 * 7. `prime 3` 进程创建又一个新的管道，并 `fork` 出一个新的子进程（`prime 5`）。
 * 8. `prime 3` 进程继续从新的管道中读取数字，并检查它们是否能被 3 整除。
 * 9. 如果一个数字不能被 3 整除，则它会被写入到又一个新的管道中，传递给 `prime 5` 进程。
 * 10. 这个过程会一直持续下去，直到所有数字都被筛选完毕。
 *
 * 通过这种方式，每个 `prime` 素数进程都会从其左侧邻居那里接收数据，并将不能被整除的数字传递给其右侧邻居。这样，数据就能够从左边一直传递到右边。
*/
void
prime(int *fd)
{
  int p, n;
  int newfd[2];

  if (read(fd[0], &p, sizeof(p)) == sizeof(p)) {  // 主prime进程读到数据
    printf("prime %d\n", p);
    pipe(newfd);
    if (fork() == 0) {
      close(fd[0]);
      close(newfd[1]);
      prime(newfd);
      exit(0);
    } else {
      close(newfd[0]);
      while (read(fd[0], &n, sizeof(n)) == sizeof(n)) {
        if (n % p != 0) {
          write(newfd[1], &n, sizeof(n));
        }
      }
      close(fd[0]);
      close(newfd[1]);
      wait(0);
    }
  }
}

int
main(int argc, char *argv[])
{
  int i;
  int fd[2];  // 祖先管道

  pipe(fd);
  if (fork() == 0) {  // 子进程
    close(fd[1]);   // 关闭写端
    prime(fd);      // 开始处理
    exit(0);
  } else {            // 父进程
    close(fd[0]);   // 关闭读端
    for (i = 2; i <= 35; i++) { // 写入2-35
      write(fd[1], &i, sizeof(i));
    }
    close(fd[1]);   // 关闭写端
    wait(0);        // 等待子进程结束
  }
  exit(0);
}

