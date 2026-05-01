#pragma once

#include "command.h"

int redirections_apply(Command *cmd, int saved_fds[3]);
void redirections_restore(int saved_fds[3]);
