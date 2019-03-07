#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  int i = getopenedcount();
  printf(1, "Opened count now is %d\n", i);
  exit();
}
