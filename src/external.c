#include "external.h"
#include "tokenizer.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int handle_external(ArgV *argv) {
  // fork
  int pid = fork();
  int child_status;
  // check pid
  if (pid == -1) {
    perror("Fork error");
    return -1;
  } else if (pid == 0){
    execvp(argv->args[0],argv->args );
    if (errno == ENOENT){
      printf("%s: command not found\n", argv->args[0]);
    }
    // this is a lower level exit that doesn't use the atExit from the parent process
    _exit(127);
  }else{
    // the pid, the status, the options
    waitpid(pid,&child_status, 0);
  }
  return 0;
}
