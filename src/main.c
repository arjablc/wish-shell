#include <stdio.h>
#include <string.h>

#define INPUT_SIZE 1024

static void handle_echo(const char *input) {
    const char *msg = strchr(input, ' ');
    if (msg != NULL) {
        printf("%s\n", msg + 1);
    } else {
        printf("\n");
    }
}

int main(void) {
    setbuf(stdout, NULL);

    char input[INPUT_SIZE];

    while (1) {
        printf("$ ");

        if (fgets(input, INPUT_SIZE, stdin) == NULL) {
            break; /* EOF or error */
        }

        /* remove trailing newline */
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0) {
            break;
        }

        char *command = strtok(input, " ");
        if (command == NULL) {
            continue;
        }

        if (strcmp(command, "echo") == 0) {
            handle_echo(input);
        } else {
            printf("%s: command not found\n", command);
        }
    }

    return 0;
}
