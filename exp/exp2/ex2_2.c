#include <signal.h>
#include <unistd.h>
#include <sys/sem.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int a;
int i;//the number which is being added
int semid;
void thread1(void);
void thread2(void);
void thread3(void);
void V(int semid, int index);
void P(int semid, int index);

union ARG {
    int val;
};

int main() {
	semid = semget(IPC_PRIVATE, 3, IPC_CREAT | 0666);//cread 3 semaphore
	union ARG arg;
	arg.val = 1;
	semctl(semid, 0, SETVAL, arg);//No.0 semaphore for writing , which is available at the begging.
	arg.val = 0;
	semctl(semid, 1, SETVAL, arg);//No.1 semaphore for reading odds , which is unavailable at the begging
	arg.val = 0;
	semctl(semid, 2, SETVAL, arg);//No.2 semaphore for reading even , which is unavailable at the begging

	a = 0;
	i = 1;
	pthread_t id1, id2, id3;

	int err1, err2, err3;

	err1 = pthread_create(&id1, NULL, (void*)thread1, NULL);
	err2 = pthread_create(&id2, NULL, (void*)thread2, NULL);
	err3 = pthread_create(&id3, NULL, (void*)thread3, NULL);

	if (err1 != 0 || err2 != 0 || err3 != 0) {
		printf("thread created failed");
		exit(0);
	}
	pthread_join(id1, NULL);
	pthread_join(id2, NULL);
	pthread_join(id3, NULL);

	return 0;
}

void thread1(void) {
	printf("calculate thread is created\n");
	while (1) {
		P(semid, 0);
		if (i > 100) {
			printf("calculate finish\n");
			V(semid, 1);
			break;
		}
		a += i;
		i++;
		if (a % 2 == 1) {
			V(semid, 1);
		}
		else {
			V(semid, 2);
		}
	}
	printf("calculate thread exited\n");
	pthread_exit(0);
}

void thread2(void) {
	printf("print odd thread is created\n");
	while (1) {
		P(semid, 1);
		if (i>100) {
			break;
		}
		printf("odd : %d\n", a);
		V(semid, 0);
	}
	printf("print odd thread exited\n");
	pthread_exit(0);
}

void thread3(void) {
	printf("print even thread is created\n");
	while (1) {
		P(semid, 2);
		if (i>100) {
			printf("even : %d\n", a);
			V(semid, 0);
			break;
		}
		printf("even : %d\n", a);
		V(semid, 0);
	}
	printf("print even thread exited\n");
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