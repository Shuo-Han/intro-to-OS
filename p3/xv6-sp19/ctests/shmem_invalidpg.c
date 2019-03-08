#include "types.h"
#include "stat.h"
#include "user.h"

void
test_failed()
{
	printf(1, "TEST FAILED\n");
	exit();
}

void
test_passed()
{
 printf(1, "TEST PASSED\n");
 exit();
}

int
main(int argc, char *argv[])
{
  void *ptr;
	
	ptr = shmget(-1);
	if (ptr != NULL) {
		test_failed();
	}

	ptr = shmget(-100);
	if (ptr != NULL) {
		test_failed();
	}

	ptr = shmget(4);
	if (ptr != NULL) {
		test_failed();
	}

	ptr = shmget(100);
	if (ptr != NULL) {
		test_failed();
	}

	test_passed();
	exit();
}
