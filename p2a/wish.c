#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <math.h>
#define PATH_LIMIT 64
#define PATH_MAX_LEN 128
#define PARA_NUM 64
#define PARA_LENGTH 64
#define HIST_LENGTH 128

char **paths;
char **hist;
int path_count;
int hist_count = 0;
int is_batch;

void print_error(int code) {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
    if(code){
      exit(code);
    }
}

char **parser(char *cmd, int *count) {
  char *newline = malloc(sizeof(char) * strlen(cmd) + 3);
  char **output = malloc(sizeof(char*) * PARA_NUM);
  for(int i = 0; i < PARA_NUM; i++) {
    output[i] = malloc(sizeof(char) * PARA_LENGTH);
  }
  int j = 0;

  for(int i = 0; i < strlen(cmd); i++) {
    int indicator = 0;
    char loc = cmd[i];
    if((loc == '>' || loc == '|') && indicator == 0) {
      newline[j++] = ' ';
      newline[j++] = loc;
      newline[j++] = ' ';
      indicator = 1;
    } else if (loc == '>' || loc == '|'){
      print_error(0);
    } else {
      newline[j++] = loc;
    }
  }
  newline[j] = '\0';

  char *current = strtok(newline, " \t\n");
  *count = 0;
  while(current) {
      strcpy(output[*count], current);
      *count += 1;
      current = strtok(NULL, " \t\n");
  }
  free(newline);
  newline = NULL;
  return output;
}

void run(char **cmd, int is_redirect, int file_num, int count) {
  if(is_redirect == 1) {
    dup2(file_num, 1);
    dup2(file_num, 2);
  }

  char * path_exec = malloc(sizeof(char) * 1024);
  int found = 0;
  for(int i = 0; i < path_count; i++) {
      memset(path_exec, 0, 256);
      strcpy(path_exec, paths[i]);
      strcat(path_exec, "/");
      strcat(path_exec, cmd[0]);
      if(!access(path_exec, X_OK)) {
          found = 1;
          // free(path_exec);
          char **new_argv = malloc(sizeof(char*) * PARA_NUM);
          memset(new_argv, 0, sizeof(char*) * PARA_NUM);
          for(int i = 0; i < count; i++) {
            new_argv[i] = cmd[i];
          }
          execv(path_exec, new_argv);
      }
    }
    free(path_exec);
    if(!found) {
        print_error(0);
    }
}

void run_pip(char **cmd, int pos, int count) {
  int status;
  int pipefd[2];
  if (0 != pipe(pipefd)) {
    print_error(1);
  }

  pid_t pid_r = fork();
  if(pid_r == -1) {
    print_error(0);
    exit(1);
  } else if (pid_r == 0) {
    pid_t pid_l = fork();
    if(pid_l == -1) {
      print_error(0);
      exit(1);
    } else if (pid_l == 0) {
      char **p1 = malloc(sizeof(char*) * PARA_NUM);
      memset(p1, 0, sizeof(char*) * PARA_NUM);
      for(int i = 0; i < pos; i++) {
        p1[i] = cmd[i];
      }
      dup2(pipefd[1], 2);
      dup2(pipefd[1], 1);
      run(p1, 0, 0, pos);
      exit(1);
    } else {
      waitpid(pid_l, &status, WUNTRACED);
      char **p2 = malloc(sizeof(char*) * PARA_NUM);
      memset(p2, 0, sizeof(char*) * PARA_NUM);
      for(int i = pos + 1; i < count; i++) {
        p2[i - pos - 1] = cmd[i];
      }
      dup2(pipefd[0], 0);
      close(pipefd[1]);
      run(p2, 0, 0, count - pos -1);
      exit(1);
    }
  } else {
    close(pipefd[1]);
    waitpid(pid_r, &status, WUNTRACED);
    return;
  }
}

void execute(char **cmd, int count) {
  int is_redirect = 0;
  int pos = -1;
  for(int i = 0; i < count; i++) {
    if(strcmp(cmd[i], ">") == 0) {
      pos = i;
      if(pos == 0 || pos != count - 2) {
        print_error(0);
      }
      is_redirect = 1;
      count -= 2;
    } else if (strcmp(cmd[i], "|") == 0) {
      pos = i;
      if(pos == 0 || pos == count - 1) {
        print_error(0);
      }
      run_pip(cmd, pos, count);
      return;
    }
  }

  pid_t pid = fork();
  if(pid == -1) {
    print_error(0);
    exit(1);
  } else if (pid == 0) {
    int file_num = -1;
    if(is_redirect) {
      file_num = open(cmd[pos+1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
      if (file_num < 0) {
        print_error(0);
      }
    }
    run(cmd, is_redirect, file_num, count);
    exit(1);
  } else {
    wait(NULL);
    return;
  }
}

void cd(char **cmd, int count) {
  if(count != 2) {
      print_error(0);
      return;
  }
  char * dir = cmd[1];
  int err = chdir(dir);
  if(err) {
      print_error(0);
  }
  return;
}

void path(char **cmd, int count) {
  int cur = 0;
  while(cur < count - 1) {
      memset(paths[path_count], 0, PATH_MAX_LEN);
      char *str = cmd[cur + 1];
      if(str[strlen(str) - 1] == '/') {
        str[strlen(str) - 1] = '\0';
      }
      strcpy(paths[path_count], str);
      path_count += 1;
      cur += 1;
  }
  return;
}

void history(char **cmd, int count) {
  if (count == 1) {
    for(int i = 0; i < hist_count; i++) {
      printf("%s", hist[i]);
    }
  } else if (count == 2) {
    int amt = 0;
    float amt_f = 0;
    if (sscanf(cmd[1], "%f", &amt_f) == 1) {
      amt = ceil(amt_f);
    } else {
      print_error(0);
    }
    if(amt > hist_count) {
      amt = hist_count;
    }
    for(int i = hist_count - amt; i < hist_count; i++) {
      printf("%s", hist[i]);
    }
  } else {
    print_error(0);
  }
}

int main(int argc, char *argv[]) {
    paths = malloc(sizeof(char*) * PATH_LIMIT);
    for(int i = 0; i < PATH_LIMIT; i++) {
        paths[i] = malloc(sizeof(char) * PATH_MAX_LEN);
    }
    hist = malloc(sizeof(char*) * HIST_LENGTH);
    for(int i = 0; i < HIST_LENGTH; i++) {
        hist[i] = malloc(sizeof(char) * PARA_LENGTH);
    }
    strcpy(paths[0], "/bin");
    size_t len = 1024;
    path_count = 1;
    char *line = (char*) malloc(sizeof(char) * len);
    int *ur_argc = malloc(sizeof(int));
    FILE *file;

    if(argc > 1) {
      if(argc > 2) {
        print_error(1);
      }
      is_batch = 1;
      file = fopen(argv[1], "r");
      if(file == NULL) {
        print_error(1);
      }
    }
    while(1) {
      if(!is_batch) {
        printf("wish> ");
        fflush(stdout);
        if(getline(&line, &len, stdin) == -1) continue;
      } else {
        if(getline(&line, &len, file) == -1) break;
      }
      if(line[0] != '\n') {
        strcpy(hist[hist_count], line);
        hist_count += 1;
      } else {
        continue;
      }
      char **command = parser(line, ur_argc);
      if(ur_argc == 0) {
        continue;
      }

      if(!strcmp(command[0], "exit")) {
        for(int i = 0; i < PATH_LIMIT; i++){
            free(paths[i]);
        }
        free(paths);
        for(int i = 0; i < PARA_NUM; i++) {
            free(command[i]);
        }
        free(command);
        free(line);
        if(*ur_argc != 1) {
          print_error(0);
        }
        free(ur_argc);
        exit(0);
      } else if (!strcmp(command[0], "cd")) {
        cd(command, *ur_argc);
      } else if (!strcmp(command[0], "path")) {
        path(command, *ur_argc);
      } else if (!strcmp(command[0], "history")) {
        history(command, *ur_argc);
      } else {
        execute(command, *ur_argc);
      }
    }
}
