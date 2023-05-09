#ifndef PROCESSARG_H
#define PROCESSARG_H

struct ProcessArg {
  int pid;
  int argc;
  char **argv;
  ProcessArg(int pid, int argc, char **argv) {
    this->pid = pid;
    this->argc = argc;
    this->argv = argv;
  }
};

#endif
