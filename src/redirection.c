#include "redirection.h"

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int redirections_apply(Command *cmd, int saved_fds[3]) {
  for (int i = 0; i < 3; i++) {
    saved_fds[i] = -1;
  }

  for (int i = 0; i < cmd->redirc; i++) {
    Redir *redir = &cmd->redirs[i];
    int flags = O_WRONLY | O_CREAT;

    if (redir->mode == REDIR_APPEND) {
      flags |= O_APPEND;
    } else {
      flags |= O_TRUNC;
    }

    if (redir->target_fd < 0 || redir->target_fd > 2) {
      continue;
    }

    if (saved_fds[redir->target_fd] == -1) {
      saved_fds[redir->target_fd] = dup(redir->target_fd);
      if (saved_fds[redir->target_fd] == -1) {
        perror("dup");
        redirections_restore(saved_fds);
        return -1;
      }
    }

    int fd = open(redir->filename, flags, 0644);
    if (fd == -1) {
      perror(redir->filename);
      redirections_restore(saved_fds);
      return -1;
    }

    if (dup2(fd, redir->target_fd) == -1) {
      perror("dup2");
      close(fd);
      redirections_restore(saved_fds);
      return -1;
    }

    close(fd);
  }

  return 0;
}

void redirections_restore(int saved_fds[3]) {
  for (int i = 0; i < 3; i++) {
    if (saved_fds[i] == -1) {
      continue;
    }

    dup2(saved_fds[i], i);
    close(saved_fds[i]);
    saved_fds[i] = -1;
  }
}
