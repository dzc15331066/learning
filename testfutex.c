#define _GNU_SOURCE
#include<stdlib.h>
#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<linux/futex.h>
#include<sys/syscall.h>
#include<sys/time.h>
#include<pthread.h>


#define likely(x)       __builtin_expect((x),1)

#define errExit(msg) do {perror(msg); exit(EXIT_FAILURE);\
                     } while(0)

//#define FUTEX_WAITERS 0x80000000
//#define FUTEX_TID_MASK 0x3fffffff

static int futex(int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3)
{
    return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}

static void lock(int *futexp, unsigned int id)
{
    int s;
    int oldval = *futexp;
    unsigned int assume_other_waiters = 0;
    
    while(1) {
        if(oldval==0){
            if(__sync_bool_compare_and_swap(futexp,0,id|assume_other_waiters)){
                printf("%u,lock\n",id);
                break;
            }
                
        }
        
        if((oldval & FUTEX_WAITERS)==0){
            if(__sync_val_compare_and_swap(futexp,oldval,oldval|FUTEX_WAITERS) != oldval){
                oldval = *futexp;
                continue;
            }
            oldval |= FUTEX_WAITERS;
        }
        assume_other_waiters |= FUTEX_WAITERS;
        
        s = futex(futexp, FUTEX_WAIT, oldval, NULL, NULL, 0);
        //printf("%d\n",oldval);
        if(s == -1 && errno != EAGAIN){
            errExit("futex-FUTEX_WAIT");
        }
        oldval = *futexp;
    }
    //printf("%u,lock,oldval: 0x%x\n",id,oldval);
}

static void unlock(int *futexp, unsigned int id)
{
    int s;
    int oldval = *futexp;
    if((int)(oldval & FUTEX_TID_MASK) != id){
        printf("not eq\n");
        return;
    }
    if(__sync_lock_test_and_set(futexp, 0) & FUTEX_WAITERS){
        //printf("%u,unlock, oldval: 0x%x\n",id, oldval);
        s = futex(futexp, FUTEX_WAKE, 1, NULL, NULL, 0);
        if(s == -1){
            errExit("futex-FUTEX_WAKE");
        }
        
    }
}

#define N 10
static int num, *futex1;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void *task(void *arg){
    int i = 0;
    unsigned int id = *(unsigned int *)(arg);
    while(i++ < 10000){
        lock(futex1,id);
        //pthread_mutex_lock(&mtx);
        num++;
        //pthread_mutex_unlock(&mtx);
        //printf("%d\n",num);
        unlock(futex1,id);
    }
    
}

int main(int argc, char const *argv[])
{
    pthread_t tids[N];
    unsigned int ids[N];
    futex1 = (int*)malloc(sizeof(int));
    *futex1 = 0; 
    int i = 0;
    time_t t1, t2;
    time(&t1);
    for(i = 0; i < N; i++){
        ids[i]=i+1;
        pthread_create(&tids[i],NULL,task,&ids[i]);
    }
    for(i = 0; i < N; i++){
        pthread_join(tids[i],NULL);
    }
    time(&t2);
    printf("num: %d cost: %lds\n",num, t2-t1);
    return 0;
}
