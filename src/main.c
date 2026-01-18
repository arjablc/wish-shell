#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);
  while (1==1) {
  printf("$ ");
  char input[1024];
  fgets(input, sizeof(char) * 1024, stdin);
  if(input != NULL){
    input[strcspn(input, "\n")] = '\0';
    printf("%s: command not found", input);
    printf("\n");
  }
  }
  return 0;
}
