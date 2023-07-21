#ifndef XV6_LABS_2022_SYSTRACE_H
#define XV6_LABS_2022_SYSTRACE_H

struct syscall_info {
  uint64 (*func)();  // 系统调用实现的函数指针
  int num_args;   // 系统调用所需参数的数量
};

#endif //XV6_LABS_2022_SYSTRACE_H
