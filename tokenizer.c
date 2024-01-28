#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer.h"

#define MAX_TOKEN_SIZE 256

char *dup_token(char *token) {
  char *copy = malloc(strlen(token) + 1);
  if (copy) {
    strcpy(copy, token);
  }
  return copy;
}


char** tokenize(char *line) {
  if (line == NULL) {
    return NULL;
  }

  char **tokens = (char **) malloc(MAX_TOKEN_SIZE * sizeof(char *));
  char *specialChar = "()<>;|\" \t\n";
  char *whitespaceChar = " \t\n";
  int j = 0;

  for (int i = 0; line[i] != '\0'; i++) {
    if (strchr(whitespaceChar, line[i]) != NULL) {
      continue;
    } else if (line[i] == '\"') {
      i++;
      char str[strlen(line)];
      int k = 0;
      while (line[i] != '\0' && line[i] != '\"') {
        str[k++] = line[i++];
      }
      str[k] = '\0';
      tokens[j++] = dup_token(str);
    } else if (strchr(specialChar, line[i]) != NULL) {
      char str[2];
      str[0] = line[i];
      str[1] = '\0';
      tokens[j++] = dup_token(str);
    } else {
      char str[strlen(line)];
      int k = 0;
      while(line[i] != '\0') {
        if (strchr(specialChar, line[i]) == NULL) {
          str[k++] = line[i++];
        } else {
          i--;
          break;
        }
      }
      str[k] = '\0';
      tokens[j++] = dup_token(str);
    }
  }

  tokens[j] = NULL;
  return tokens;
}

char** copy_tokens(char** tokens) {
  if (tokens == NULL) {
    return NULL;
  }
  
  char **copy = (char **) malloc(MAX_TOKEN_SIZE * sizeof(char *));
  int i = 0;

  while (tokens[i] != NULL) {
    copy[i] = dup_token(tokens[i]);
    i++;
  }

  copy[i] = NULL;
  return copy;
}

void free_tokens(char** tokens) {
  if (tokens == NULL) {
    return;
  }

  for (int i = 0; tokens[i] != NULL; i++) {
    free(tokens[i]);
  }
  free(tokens);
}

void remove_first_token(char **tokens) {
  if (tokens == NULL || tokens[0] == NULL) {
    return;
  }
  
  free(tokens[0]);
  int i = 0;
  
  while (tokens[i + 1] != NULL) {
    tokens[i] = tokens[i + 1];
    i++;
  }

  tokens[i] = NULL;
}

char** split_tokens(char **tokens, char *delimiters) {
  if (tokens == NULL || delimiters == NULL) {
    return NULL;
  }

  char **new_tokens = (char**) malloc(sizeof(char*) * MAX_TOKEN_SIZE);
  int i = 0;

  while (tokens[0] != NULL) {
    int is_delimiter = 0;
    if (new_tokens[0] != NULL) {
      for (int j = 0; delimiters[j] != '\0'; j++) {
        if (tokens[0][0] == delimiters[j]) {
          is_delimiter = 1;
	  break;
        }
      }
    }

    if (is_delimiter == 1) {
      break;
    }

    new_tokens[i] = dup_token(tokens[0]);
    remove_first_token(tokens);
    i++;
  }

  new_tokens[i] = NULL;
  return new_tokens;
}

char** join_tokens(char **tokens1, char **tokens2) {
  if (tokens1 == NULL || tokens2 == NULL) {
    return NULL;
  }

  char **new_tokens = (char**) malloc(sizeof(char*) * MAX_TOKEN_SIZE);
  int new_token_index = 0;
  for (int i = 0; tokens1[i] != NULL && new_token_index < MAX_TOKEN_SIZE; i++, new_token_index++) {
    new_tokens[new_token_index] = dup_token(tokens1[i]);
  }

  for (int i = 0; tokens2[i] != NULL && new_token_index < MAX_TOKEN_SIZE; i++, new_token_index++) {
    new_tokens[new_token_index] = dup_token(tokens2[i]);
  }

  free_tokens(tokens1);
  free_tokens(tokens2);

  new_tokens[new_token_index] = NULL;
  return new_tokens;
}
