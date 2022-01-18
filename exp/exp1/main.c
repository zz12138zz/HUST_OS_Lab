#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#define MAX_TIMES 10
#define BUFFER_LENGTH 128
pid_t p1, p2;
int pipefd[2];
void kill_parent(int sig_no);
void kill_child1(int sig_no);
void kill_child2(int sig_no);
void child1();
void child2();

void main()
{
    pipe(pipefd);//initial the pipe
    signal(SIGINT, kill_parent); //set what will parent do when receiving ctrl+c
    p1= fork();
    if (p1== 0){//when child 1 run
        child1();
    }
    else{
        p2 = fork();
        if (p2 == 0){//when child 2 run
            child2();
        }
        else
        {//when parent run
            int count = 1;
            close(pipefd[0]);//close read port;
            char buffer[BUFFER_LENGTH];
            while (count <= MAX_TIMES)//loop send message
            {
                sprintf(buffer, "I send you %d times.", count);
                write(pipefd[1], buffer, sizeof(buffer));
                count++;
                sleep(1);//send one message per second
            }
            //when parent run here,the process will exit naturally
            close(pipefd[1]);
            //wait the two child process to exit
            wait(NULL);
            wait(NULL);
            printf("Parent Process exit!\n");
            exit(0);//0 means exit naturally
        }
    }
}

//what child1 will do when it's running
void child1(){
    signal(SIGINT, SIG_IGN);//ignore the ctrl+c and wait for the SIGUSR1 from parent
    signal(SIGUSR1, kill_child1);
    close(pipefd[1]);//close the write port
    char buffer[BUFFER_LENGTH];
    while (read(pipefd[0], buffer, sizeof(buffer))>0){
        //if no message in pipe ,the process will be blocked at the read function
        //if the parrent close write port,the read function will return 0,and the loop will break
        printf("child 1 receive : %s\n", buffer);
    }
    //when child 1 run here,the child 1 will exit naturally
    close(pipefd[0]);
    printf("Child 1 Process exit!\n");
    exit(0);
}
//what child2 will do when it's running
void child2(){
    signal(SIGINT, SIG_IGN);//ignore the ctrl+c and wait for the SIGUSR1 from parent
    signal(SIGUSR1, kill_child2);
    close(pipefd[1]);//close the write port
    char buffer[BUFFER_LENGTH];
    while (read(pipefd[0], buffer, sizeof(buffer))>0){
        //if no message in pipe ,the process will be blocked at the read function
        //if the parrent close write port,the read function will return 0,and the loop will break
        printf("child 2 receive : %s\n", buffer);
    }
    //when child 2 run here,the child 2 will exit naturally
    close(pipefd[0]);
    printf("Child 2 Process exit!\n");
    exit(0);
}

//when parent receive ctrl+c
void kill_parent(int sig_no){
    //send SIGUSR1 to child 1 and child 2
    kill(p1, SIGUSR1);
    kill(p2, SIGUSR1);
    //wait the two child process to exit
    wait(NULL);
    wait(NULL);
    close(pipefd[1]);
    printf("\nParent Process is Killed!\n");
    exit(1);

}
void kill_child1(int sig_no){
    close(pipefd[0]);
    printf("\nChild Process 1 is Killed by Parent!\n");
    exit(1);
}
void kill_child2(int sig_no){
    close(pipefd[0]);
    printf("\nChild Process 2 is Killed by Parent!\n");
    exit(1);
}