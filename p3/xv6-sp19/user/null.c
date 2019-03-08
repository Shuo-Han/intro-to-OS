#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  char i[4097];

  printf(1, "%d\n", i);
  // printf(1, "The value of i is : %d", *i);

  exit();
}
