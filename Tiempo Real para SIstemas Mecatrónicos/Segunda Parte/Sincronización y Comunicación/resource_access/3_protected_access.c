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


// protection layer
typedef struct {
	ll_data_t ll_data;
	pthread_spinlock_t lock;
} protected_data_t;

void protected_inc_data(protected_data_t *p)
{
	pthread_spin_lock(&p->lock);	// critical section entry
	inc_data(&p->ll_data);	// <- use of shared data (critical section)
	pthread_spin_unlock(&p->lock);	// critical section exit
}

void protected_dec_data(protected_data_t *p)
{
	pthread_spin_lock(&p->lock);	// critical section entry
	dec_data(&p->ll_data);	// <- use of shared data (critical section)
	pthread_spin_unlock(&p->lock);	// critical section exit
}
// end of protection layer

//-- functions accesing shared data
void go_up(int n, protected_data_t *p)
{
    for (int i = 0; i < n; i++) {
        protected_inc_data(p);	// <- use of protected shared data (critical section)
						// <- here the rest of non critical section
    }
}

void go_down(int n, protected_data_t *p)
{
    for (int i = 0; i < n; i++) {
        protected_dec_data(p);	// <- use of prtected shared data (critical section)
						// <- here the rest of non critical section
    }
}
//--


//-- worker threads and work description
struct work_descr{
    void (*function)(int, protected_data_t*);	// target function
    int iterations;					// first argument of target function
    protected_data_t *pr_data_p;	// second argument
};

typedef struct work_descr *work_descr_t;

void* worker(void *arg)
{
    work_descr_t descr_p = (work_descr_t)arg;
    descr_p->function(descr_p->iterations, descr_p->pr_data_p);
    return NULL;
}
//--

protected_data_t pr_data;

int main(int argc, char *argv[])
{
    /* protected concurrent access */
    pthread_spin_init(&pr_data.lock, 0);
    struct work_descr thr_up, thr_down;
    thr_up.function = go_up;
    thr_up.iterations = ITER;
    thr_up.pr_data_p = &pr_data;
    thr_down.function = go_down;
    thr_down.iterations = ITER;
    thr_down.pr_data_p = &pr_data;
    pthread_t th0, th1;
    pthread_create(&th0, NULL, worker, &thr_up);
    pthread_create(&th1, NULL, worker, &thr_down);
    pthread_join(th0, NULL);
    pthread_join(th1, NULL);
    printf("Last value after protected concurrent access: %ld\n", pr_data.ll_data.value);
    pthread_spin_destroy(&pr_data.lock);
    return 0;
}
