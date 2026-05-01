#include "external.h"
#include "redirection.h"

#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int handle_external(Command *cmd) {
  if (command_is_empty(cmd)) {
    return 0;
  }

  // fork
  int pid = fork();
  int child_status;
  // check pid
  if (pid == -1) {
    perror("Fork error");
    return -1;
  } else if (pid == 0){
    int saved_fds[3];
    if (redirections_apply(cmd, saved_fds) != 0) {
      _exit(1);
    }

    execvp(cmd->args[0], cmd->args);
    if (errno == ENOENT){
      fprintf(stderr, "%s: command not found\n", cmd->args[0]);
    }
    // this is a lower level exit that doesn't use the atExit from the parent process
    _exit(127);
  }else{
    // the pid, the status, the options
    waitpid(pid,&child_status, 0);
  }
  return 0;
}
