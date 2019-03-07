#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"

int
main(int argc, char *argv[])
{
    struct pstat st;

    if(argc != 2){
        printf(1, "usage: mytest counter");
        exit();
    }
    int i, x, l, j;
    int mypid = getpid();
    // printf(1, "222222222222222222\n");
    for(i = 1; i < atoi(argv[1]); i++){
        x = x + i;
    }
    // printf(1, "3333333333333333\n");
    getpinfo(&st);

    for (j = 0; j < NPROC; j++) {
      // printf(1, "!!!!!!!!!!!!!::: %d\n", st.inuse[j]);
        if (st.inuse[j] && st.pid[j] >= 3 && st.pid[j] == mypid) {
            for (l = 3; l >= 0; l--) {
                printf(1, "level:%d \t ticks-used:%d\n", l, st.ticks[j][l]);
            }
        }
    }

    exit();
    return 0;
}
