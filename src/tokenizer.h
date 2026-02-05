#include <stddef.h>
#pragma once

// Just a dynamic array implementation for now
typedef struct  {
  char **argv;
  int argc;
  int cap;
} ArgV ;



int tokenize(ArgV* argv, char* input);

void push_argv(ArgV* argv, char* elem);

ArgV* new_argv();






