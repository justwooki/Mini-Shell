#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "tokenizer.h"

int main(int argc, char **argv) {
  char input[255];
  if (fgets(input, sizeof(input), stdin) != NULL) {
    char **tokens = tokenize(input);
    for (int i = 0; tokens[i] != NULL; i++) {
      printf("%s\n", tokens[i]);
    }
    free_tokens(tokens);
  } else {
    perror("Error reading input");
    exit(1);
  }

  return 0;
}
