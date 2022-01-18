#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/sem.h>
#include <pthread.h>

int sold=0;//num of tickets have been sold
int total;//num of all tickets
int semid;//semaphore id
int n;
void thread(void);
void V(int semid, int index);
void P(int semid, int index);

union ARG {
    int val;
};

int main(void) {
    srand(time(0));
	semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	union ARG arg;
	arg.val = 1;
	semctl(semid, 0, SETVAL, arg);
    printf("Input the num of tickets: ");
    scanf("%d",&total);
    printf("Input the num of thread: ");
    scanf("%d",&n);
	pthread_t *id_array =(pthread_t *)malloc(n*sizeof(pthread_t));
    //create thread
    int err;
    for(int i=0 ; i<n ; i++){
	    err = pthread_create(id_array+i, NULL, (void*)thread, NULL);
        if (err != 0) {
            printf("thread created failed");
            exit(0);
        }
	}
    //destroy thread
    for(int i=0 ; i<n ; i++){
        pthread_join(id_array[i],NULL);
	}
    free(id_array);

	return 0;
}

void thread(void) {
    pthread_t id = pthread_self();
	printf("thread %lu is created\n",id);
	int sold_self = 1;
	while (1) {
		P(semid, 0);
		if (sold == total) {
			V(semid, 0);
			break;
		}
		sold_self++;
		sold++;
		V(semid, 0);
        sleep(0.01*(rand()%10));
	}
	printf("thread %lu has sold %d tickets\n",id, sold_self - 1);
	pthread_exit(0);
}

void P(int semid, int index)
{
	struct sembuf sem;
	sem.sem_num = index;
	sem.sem_op = -1;
	sem.sem_flg = 0;
	semop(semid, &sem, 1);
	return;
}

void V(int semid, int index)
{
	struct sembuf sem;
	sem.sem_num = index;
	sem.sem_op = 1;
	sem.sem_flg = 0;
	semop(semid, &sem, 1);
	return;
}