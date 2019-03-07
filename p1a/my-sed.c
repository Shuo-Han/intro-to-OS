#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void replace(char *fstr, char *rstr, char *buffer) {
  char *substr = strstr(buffer, fstr);
  int flen = strlen(fstr);
  int rlen = strlen(rstr);
  if (substr == NULL) {
    printf("%s", buffer);
  } else {
    int size_newstr = strlen(buffer) + rlen;
    char new_str[size_newstr];
    char latter[size_newstr];
    strcpy(latter, substr + flen);
    strncpy(new_str, buffer, substr - buffer);
    new_str[substr - buffer] = '\0';
    strcat(new_str, rstr);
    strcat(new_str, latter);
    printf("%s", new_str);
  }
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("my-sed: find_term replace_term [file ...]\n");
    exit(1);
  }

  if (argc == 3) {
    size_t len = 0;
    char *buffer = NULL;
    while(getline(&buffer, &len , stdin) != -1) {
      replace(argv[1], argv[2], buffer);
      free(buffer);
      buffer = NULL;
    }
    // printf("%s\n", buffer);

  }

  if (argc > 3) {
    int counter = 3;
      while(counter < argc) {
        ssize_t nread;

        size_t len = 0;
        FILE *fp = fopen(argv[counter], "r");
        if(fp == NULL) {
          printf("my-sed: cannot open file\n");
          exit(1);
        }
        char *buffer = NULL;
        while((nread = getline(&buffer, &len, fp)) != -1) {
          replace(argv[1], argv[2], buffer);
          // char *substr = strstr(buffer, argv[1]);
          // if (substr == NULL) {
          //   printf("%s", buffer);
          // } else {
          //   int size_newstr = strlen(buffer) + rlen;
          //   char new_str[size_newstr];
          //   char latter[size_newstr];
          //   strcpy(latter, substr + flen);
          //   strncpy(new_str, buffer, substr - buffer);
          //   new_str[substr - buffer] = '\0';
          //   strcat(new_str, rstr);
          //   strcat(new_str, latter);
          //   printf("%s", new_str);
          // }
        }
        free(buffer);
        fclose(fp);
        counter = counter + 1;
      }
  }

  return 0;
}
