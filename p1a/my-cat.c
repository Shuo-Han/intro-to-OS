#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
int counter = 1;
  while(counter < argc) {
    FILE *fp = fopen(argv[counter], "r");
    if(fp == NULL) {
      printf("my-cat: cannot open file\n");
      exit(1);
    }
    char buffer[4096];
    while(fgets(buffer, 4097, fp) != NULL) {
      printf("%s", buffer);
    }
    fclose(fp);
    counter = counter + 1;
  }
  return 0;
}
