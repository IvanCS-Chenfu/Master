/*
 *
 */

#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"

#define ITER 1000000

//-- low level data layer
typedef struct {
    long value;			// the actual low level var. for this example
						// here the rest of fields of the real data type
} ll_data_t;

void inc_data(ll_data_t *p)
{
	p->value++;			// low level access to the shared resource
}

void dec_data(ll_data_t *p)
{
	p->value--;			// low level access to the shared resource
}
// end of low level data layer


//-- functions accesing shared data
void go_up(int n, ll_data_t *p)
{
    for (int i = 0; i < n; i++) {
        inc_data(p); 	// <- use of shared data (critical section)
						// <- here the rest of non critical section
    }
}

void go_down(int n, ll_data_t *p)
{
    for (int i = 0; i < n; i++) {
        dec_data(p); 	// <- use of shared data (critical section)
						// <- here the rest of non critical section
    }
}
//--

//--

ll_data_t ll_data;

int main(int argc, char *argv[])
{
    /* sequential access */
    go_up(ITER, &ll_data);
    go_down(ITER, &ll_data);

    printf("Last value after secuential access: %ld\n", ll_data.value);

    return 0;
}
