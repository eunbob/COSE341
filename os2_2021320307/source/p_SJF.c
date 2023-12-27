#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define SJF 336

int main(int argc, char **argv){
    int job; // job time
    int delay; // delay time
    char name[4]; // process name
    int wait = 0; // wait time

    if(argc < 4){ // argument가 부족하다면
        printf("\nInsufficient Arguments..\n"); // argument 부족 메시지 출력
        return 1;
    }

    job = atoi(argv[1]); // argument를 int형으로 변환
    delay = atoi(argv[2]); // argument를 int형으로 변환
    strcpy(name, argv[3]); // argument를 name에 복사

    sleep(delay); // delay만큼 sleep
    printf("\nProcess %s : I will use CPU by %ds.\n", name, job); // process name과 job time 출력
    job *= 10; // job time을 0.1초 단위로 변환 (1초 = 10)

    while(job){ // job time이 0이 될 때까지
        if(!syscall(SJF, name, job)) job--; //프로세스가 cpu를 점유하고 있다면 job time 감소

        else{
            wait++; // 프로세스가 cpu를 점유하지 않는다면 wait time 증가
        }
        usleep(100000); // 0.1초 sleep
    }

    syscall(SJF, name, 0); // 프로세스가 cpu를 점유하지 않도록 함
    printf("\nProcess %s : Finish! My total wait time is %ds. \n", name, (wait + 5)/10); // wait time 출력
    return 0;
}

