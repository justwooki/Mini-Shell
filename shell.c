#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>

#include "tokenizer.h"

#define MAX_INPUT_SIZE 255 // maximum input size
#define HOME_DIR "/home/user" // default home directory

char **prev_command = NULL; // keeps track of the previous command input for the built-in prev command
bool exit_called = false; // boolean to keep track of whether the user is trying to exit the program

void execute_commands(char **command);
void run_shell(char **tokens);

// exits the program with a bad status if there's an error
void exit_if_error(int status, char *msg) {
  if (status == -1) {
    perror(msg);
    exit(1);
  }
}

// removes first character in a given string
void remove_first_char(char *str) {
  if (str == NULL) {
    return;
  }

  for (int i = 0; i < strlen(str); i++) {
    str[i] = str[i + 1];
  }
}

// prints ascii art for the built-in help command
void print_ascii_art() {
  printf(" /\\_/\\  /\\_/\\  /\\_/\\  /\\_/\\  /\\_/\\  /\\_/\\  /\\_/\\  /\\_/\\  /\\_/\\  /\\_/\\  /\\_/\\  /\\_/\\  /\\_/\\  /\\_/\\ \n");
  printf("( o.o )( o.o )( o.o )( o.o )( o.o )( o.o )( o.o )( o.o )( o.o )( o.o )( o.o )( o.o )( o.o )( o.o )\n");
  printf(" > ^ <  > ^ <  > ^ <  > ^ <  > ^ <  > ^ <  > ^ <  > ^ <  > ^ <  > ^ <  > ^ <  > ^ <  > ^ <  > ^ < \n");
  printf(" /\\_/\\                  _                            _          _   _ _____ _     _         /\\_/\\ \n");
  printf("( o.o )   __      _____| | ___ ___  _ __ ___   ___  | |_ ___   | | | | ____| |   | |       ( o.o )\n");
  printf(" > ^ <    \\ \\ /\\ / / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\ | __/ _ \\  | |_| |  _| | |   | |        > ^ < \n");
  printf(" /\\_/\\     \\ V  V /  __/ | (_| (_) | | | | | |  __/ | || (_) | |  _  | |___| |___| |___     /\\_/\\ \n");
  printf("( o.o )     \\_/\\_/ \\___|_|\\___\\___/|_| |_| |_|\\___|  \\__\\___/  |_| |_|_____|_____|_____|   ( o.o )\n");
  printf(" > ^ <                                                                                      > ^ < \n");
  printf(" /\\_/\\  /\\_/\\  /\\_/\\  /\\_/\\  /\\_/\\  /\\_/\\  /\\_/\\  /\\_/\\  /\\_/\\  /\\_/\\  /\\_/\\  /\\_/\\  /\\_/\\  /\\_/\\ \n");
  printf("( o.o )( o.o )( o.o )( o.o )( o.o )( o.o )( o.o )( o.o )( o.o )( o.o )( o.o )( o.o )( o.o )( o.o )\n");
  printf(" > ^ <  > ^ <  > ^ <  > ^ <  > ^ <  > ^ <  > ^ <  > ^ <  > ^ <  > ^ <  > ^ <  > ^ <  > ^ <  > ^ < \n");
  printf("\n");
  printf("\n");
  printf("\n");
}

// displays the main message to be displayed for the built-in help command
void print_help_message() {
  printf("Welcome to my Mini-Shell!\n");
  printf("\n");
  printf("I died while making this.\n");
  printf("\n");
  printf("Built-in Commands:\n");
  printf("- cd [directory]: Change the current directory.\n");
  printf("- source [filename]: Execute a script.\n");
  printf("- prev: Executes previous command.\n");
  printf("- help: Display this help message.\n");
  printf("- exit: Exit the shell.\n");
  printf("\n");
  printf("Command Details:\n");
  printf("- cd [directory]: Change the current working directory to the specified path.\n");
  printf("- source [filename]: Takes a filename as an argument and processes each line of the file as a command.\n");
  printf("- prev: Prints the previous command line and executes it again.\n");
  printf("- help: Explains all the built-in commands available in the shell.\n");
  printf("- exit: Terminate the shell.\n");
  printf("\n");
  printf("Examples:\n");
  printf("- To change the current directory: cd /path/to/directory\n");
  printf("- To execute a script from a file named \"myscript.sh\": source myscript.sh\n");
  printf("To re-run and display the previous command: prev\n");
  printf("- To display the help message: help\n");
  printf("- To exit the shell: exit\n");
  printf("\n");
  printf("Well, that's all folks!\n");
  printf("Enjoy using my Mini-Shell!\n");
  printf("\n");
  printf("\n");
  printf("\n");
}

// function to deal with the built-in cd command
void change_directory(char **command) {
  // "cd" or "cd ~" will default to the home directory
  if (command[1] == NULL || strcmp(command[1], "~") == 0) {
    chdir(HOME_DIR);
  } else if (command[1][0] == '~') { // special case for navigating to a directory from the home directory
    char path[MAX_INPUT_SIZE];
    snprintf(path, MAX_INPUT_SIZE, "%s%s", HOME_DIR, command[1] + 1);
    if (chdir(path) == -1) {
      perror("cd");
    }
  } else {
    if (chdir(command[1]) == -1) {
      perror("cd");
    }
  }
}

// function to deal with the built-in source command
void source(char **command) {
  int child_pid = fork();
  exit_if_error(child_pid, "Error - fork failed");
  
  if (child_pid == 0) {
    char *file[2];
    if (strcmp(command[1], "~") == 0) {
      file[0] = HOME_DIR;
    } else if (command[1][0] == '~') {
      char path[MAX_INPUT_SIZE];
      snprintf(path, MAX_INPUT_SIZE, "%s%s", HOME_DIR, command[1] + 1);
      file[0] = path;
    } else {
      char path[MAX_INPUT_SIZE];
      snprintf(path, MAX_INPUT_SIZE, "/.%s", command[1]);
      file[0] = command[1];
    }
    file[1] = NULL;
    execvp(file[0], file);
    perror("execvp");
    exit(1);
  } else {
    wait(NULL);
  }
}

// function to deal with the built-in prev command
void prev() {
  // prev cannot be executed if no previous command took place before this
  if (prev_command == NULL) {
    printf("prev: command not found\n");
  } else {
    printf("%s\n", prev_command[0]);
    execute_commands(prev_command);
  }
}

// function for executing a specific command
void execute_commands(char **command) {
  if (strcmp(command[0], "exit") == 0 && command[1] == NULL) {
    printf("Bye bye.\n");
    exit_called = true;
  } else if (strcmp(command[0], "cd") == 0 && command[2] == NULL) {
    change_directory(command);
  } else if (strcmp(command[0], "source") == 0 && command[1] != NULL && command[2] == NULL) {
    source(command);
  } else if (strcmp(command[0], "help") == 0 && command[1] == NULL) {
    print_ascii_art();
    print_help_message();
  } else if (strcmp(command[0], "prev") == 0 && command[1] == NULL) {
    prev();
  } else {
    int child_pid = fork();
    exit_if_error(child_pid, "Error - fork failed");

    if (child_pid == 0) {
      if (execvp(command[0], command) == -1) {
        printf("%s: command not found\n", command[0]);
        exit(1);
      }
      
      exit(0);
    } else {
      wait(NULL);
    }
  }
}

// function to deal with input redirection
void input_redirect(char **cmd, char **tokens) {
  if (cmd == NULL || tokens == NULL) {
    return;
  }

  int child_pid = fork();
  exit_if_error(child_pid, "Error - fork failed");
  
  if (child_pid == 0) {
    close(0);
    int file_fd = open(tokens[0], O_RDONLY);
    exit_if_error(file_fd, "Error - open file failed");
    remove_first_token(tokens);
    char **new_tokens = join_tokens(cmd, tokens);
    printf("run\n");
    run_shell(new_tokens);
    exit(0);
  } else {
    wait(NULL);
  }
}

// function to deal with output redirection
void output_redirect(char **cmd, char **tokens) {
  if (cmd == NULL || tokens == NULL) {
    return;
  }

  int child_pid = fork();
  exit_if_error(child_pid, "Error -fork failed");
  
  if (child_pid == 0) {
    close(1);
    int file_fd = open(tokens[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    exit_if_error(file_fd, "Error - open file failed");
    remove_first_token(tokens);
    char **new_tokens = join_tokens(cmd, tokens);
    run_shell(new_tokens);
    exit(0);
  } else {
    wait(NULL);
  }
}

// function to run the main mini shell program
void run_shell(char **tokens) {
  if (tokens == NULL) {
    return;
  }

  // based on the list of delimiters, the program will act accordingly
  // to execute sequencing, redirection, and piping
  char *delimiters = ";<>";

  if (tokens[0] != NULL) {
    // separate the first command from the others split by a delimiter
    char **cmd = split_tokens(tokens, delimiters);

    if (strcmp(cmd[0], "prev") != 0 || cmd[1] != NULL) {
      free_tokens(prev_command);
    }

    if (tokens[0] == NULL) {
      execute_commands(cmd);
      free_tokens(cmd);
    } else if (strcmp(tokens[0], ";") == 0) {
      execute_commands(cmd);
      free_tokens(cmd);
      remove_first_token(tokens);
      run_shell(copy_tokens(tokens));
    } else if (strcmp(tokens[0], "<") == 0) {
      remove_first_token(tokens);
      input_redirect(cmd, tokens);
      free_tokens(cmd);
    } else if (strcmp(tokens[0], ">") == 0) {
      remove_first_token(tokens);
      output_redirect(cmd, tokens);
      free_tokens(cmd);
    }

    free_tokens(tokens);

    // Free remaining allocated memory if the program is asked to exit
    if (exit_called) {
      free_tokens(prev_command);
      exit(0);
    }
  }
}

int main(int argc, char **argv) {
  char input[MAX_INPUT_SIZE];
  char *delimiters = ";";
  printf("Welcome to mini-shell.\n");
  
  while (1) {
    printf("shell $ ");
    if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL || strcmp(input, "exit") == 0) {
      printf("Bye bye.\n");
      break;
    } else {
      char **tokens = tokenize(input);
      run_shell(tokens);
    }
  }

  return 0;
}
