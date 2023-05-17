#ifndef PROCESSARG_H
#define PROCESSARG_H

struct ProcessArg {
  int pid;
  int argc;
  char **argv;
  ProcessArg(int pid = 0, int argc = 0, char **argv = 0) {
    this->pid = pid;
    this->argc = argc;
    this->argv = argv;
  }
};

#endif
