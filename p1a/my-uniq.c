#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  char *buffer1 = NULL;
  char *buffer2 = NULL;
  size_t len = 0;
  if(argc == 1) {
    if(getline(&buffer1, &len, stdin) == -1) {
      exit(1);
    }
    printf("%s", buffer1);
    while(getline(&buffer2, &len, stdin) != -1) {
      // printf("%s\n", buffer1);
      // printf("%s\n", buffer1);
      if(strcmp(buffer1, buffer2) != 0) {
        printf("%s", buffer2);
      }
      strcpy(buffer1, buffer2);
      free(buffer2);
      buffer2 = NULL;
    }
    free(buffer1);
  }

  int counter = 1;
    while(counter < argc) {
      FILE *fp = fopen(argv[counter], "r");
      if(fp == NULL) {
        printf("my-uniq: cannot open file\n");
        exit(1);
      }
      // int indi = 0;
      if(getline(&buffer1, &len, fp) == -1) {
        exit(0);
      }
      printf("%s", buffer1);
      while(getline(&buffer2, &len, fp) != -1) {
        // if (strcmp(buffer1, buffer2) == 0) {
        //   indi = 2;
        // } else {
        //   if (indi > 0) {
        //     indi = indi - 1;
        //   }
        //   if(indi == 0) {
        //     printf("%s", buffer1);
        //   }
        // }
        if (strcmp(buffer1, buffer2) != 0) {
          printf("%s", buffer2);
        }
        strcpy(buffer1, buffer2);
        free(buffer2);
        buffer2 = NULL;
      }
      free(buffer1);
      buffer1 = NULL;
      fclose(fp);
      counter = counter + 1;
    }
  return 0;
}
