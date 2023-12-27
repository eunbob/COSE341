#include<linux/sched.h>
#include<linux/syscalls.h>
#include<linux/kernel.h>
#include<linux/linkage.h>
#include<linux/slab.h>

#define MAX 100
#define IDLE -1
#define TIME_SLICE 10

typedef struct job{
        pid_t pid;
        int job;
}job_t;

job_t queue[MAX]; 

int rear = 0; // rear of queue

pid_t now = IDLE; // 현재 실행 중인 프로세스의 pid
int time = 0; // 현재 실행 중인 프로세스가 cpu를 점유하고 있는 시간

// queue가 비었는지 확인 후 비었으면 1, 아니면 0 반환
int ku_is_empty(void){
        return rear == 0;
}

// queue에 pid가 있는지 확인 후 있으면 0, 아니면 1 반환
int ku_is_new(pid_t pid){
        int i;
        for(i = 0; i < rear; i++){
                if(queue[i].pid == pid) return 0;
        }
        return 1;
}

// queue에 job을 push
void ku_push(job_t new_job){
        if(rear == MAX){
                printk("Queue is full\n");
                return;
        }
        queue[rear++] = new_job;
}

// queue에서 job을 pop
pid_t ku_pop(void){
        if(ku_is_empty()){
                printk("Queue is empty\n");
                return -1;
        }
        job_t out = queue[0];
        int i;
        for(i = 0; i < rear - 1; i++){
                queue[i] = queue[i+1];
        }
        rear--;
        return out.pid;
}

// queue를 job 순서대로 정렬
void sort_queue_by_job(void) {
        int i, j;
        for (i = 0; i < rear; i++) {
                for (j = 0; j < rear - i - 1; j++) {
                        if (queue[j].job > queue[j + 1].job) {
                                job_t temp = queue[j];
                                queue[j] = queue[j + 1];
                                queue[j + 1] = temp;
                        }
                }
        }
}
                
SYSCALL_DEFINE2(ku_cpu_FCFS, char*, name, int, job){
        job_t new_job = {current->pid, job}; // 새로운 프로세스의 pid와 job을 저장

        if(now == IDLE) now = new_job.pid; // 현재 cpu가 IDLE 상태이면 현재 프로세스를 실행
        
        if(now == new_job.pid){ // 현재 프로세스가 실행 중인 프로세스라면
                if(job == 0){ // job이 0이면 프로세스가 종료된 것
                        printk("Process Finish: %s\n", name); // 종료 메시지 출력

                        if(ku_is_empty()) now = IDLE; // queue가 비었으면 IDLE 상태로 전환
                        else now = ku_pop(); // queue에서 다음 프로세스를 pop
                }
                else{
                        printk("Working: %s\n", name); // job이 0이 아니면 현재 프로세스가 작업 중
                }
                return 0;
        }
        else{ // 현재 프로세스가 실행 중인 프로세스가 아니면
                if(ku_is_new(new_job.pid)) ku_push(new_job);  // 새로운 프로세스가 queue에 없으면 queue에 push
                printk("Working Denied: %s\n", name); // 현재 프로세스가 실행 중인 프로세스가 아니면 작업 거부 메시지 출력
        }
        return 1;
}

SYSCALL_DEFINE2(ku_cpu_SJF, char*, name, int, job){
        job_t new_job = {current->pid, job}; // 새로운 프로세스의 pid와 job을 저장

        if(now == IDLE) now = new_job.pid; // 현재 cpu가 IDLE 상태이면 현재 프로세스를 실행
        
        if(now == new_job.pid){ // 현재 프로세스가 실행 중인 프로세스라면
                if(job == 0){ // job이 0이면 프로세스가 종료된 것
                        printk("Process Finish: %s\n", name); // 종료 메시지 출력

                        if(ku_is_empty()) now = IDLE; // queue가 비었으면 IDLE 상태로 전환
                        else now = ku_pop(); // queue에서 다음 프로세스를 pop
                }
                else{
                        printk("Working: %s\n", name); // job이 0이 아니면 현재 프로세스가 작업 중
                }
                return 0;
        }
        else{ // 현재 프로세스가 실행 중인 프로세스가 아니면
                if(ku_is_new(new_job.pid)) { // 새로운 프로세스가 queue에 없으면
                        ku_push(new_job); // queue에 push
                        sort_queue_by_job(); // queue를 job 순서대로 정렬
                }
                printk("Working Denied: %s\n", name); // 현재 프로세스가 실행 중인 프로세스가 아니면 작업 거부 메시지 출력
        }
        return 1;
}

SYSCALL_DEFINE2(ku_cpu_SRTF, char*, name, int, job){ 
        job_t new_job = {current->pid, job}; // 새로운 프로세스의 pid와 job을 저장

        if(now == IDLE){ // 현재 cpu가 IDLE 상태이면 현재 프로세스를 실행
                now = new_job.pid;
                ku_push(new_job); //실행 중인 프로세스의 남은 job을 확인하기 위해 queue에 push
        }
        
        if(now == new_job.pid){ // 현재 프로세스가 실행 중인 프로세스라면
                if(job == 0){ // job이 0이면 프로세스가 종료된 것
                        printk("Process Finish: %s\n", name); // 종료 메시지 출력
                        
                        ku_pop(); // 작업이 종료된 프로세스를 queue에서 pop
                        if(ku_is_empty()) now = IDLE; // queue가 비었으면 IDLE 상태로 전환
                        else now = queue[0].pid; // queue의 첫 번째 프로세스를 실행
                }
                else{
                        printk("Working: %s\n", name); // job이 0이 아니면 현재 프로세스가 작업 중
                        queue[0].job = job; // 현재 실행중인 프로세스의 남은 job을 queue에 저장
                }
                return 0;
        } 
        else{ // 현재 프로세스가 실행 중인 프로세스가 아니면
                if(ku_is_new(new_job.pid)) { // 새로운 프로세스가 queue에 없으면
                        ku_push(new_job); // queue에 push
                        sort_queue_by_job(); // queue를 job 순서대로 정렬
                        if(queue[0].pid != now) now = queue[0].pid; // queue의 첫 번째 프로세스가 현재 실행 중인 프로세스가 아니면 queue의 첫 번째 프로세스를 실행
                        
                }
                printk("Working Denied: %s\n", name); // 현재 프로세스가 실행 중인 프로세스가 아니면 작업 거부 메시지 출력
        }
        return 1;
}


SYSCALL_DEFINE2(ku_cpu_RR, char*, name, int, job){
        job_t new_job = {current->pid, job}; // 새로운 프로세스의 pid와 job을 저장

        if(now == IDLE) { // 현재 cpu가 IDLE 상태이면 현재 프로세스를 실행
                now = new_job.pid;
        }
        
        if(now == new_job.pid){ // 현재 프로세스가 실행 중인 프로세스라면
                if(job == 0){ // job이 0이면 프로세스가 종료된 것
                        printk("Process Finish: %s\n", name); // 종료 메시지 출력
                        time = 0; // time 초기화
                        if(ku_is_empty()) now = IDLE; // queue가 비었으면 IDLE 상태로 전환
                        else now = ku_pop(); // queue에서 다음 프로세스를 pop
                }
                else{ // job이 0이 아니면 현재 프로세스가 작업 중
                        printk("Working: %s\n", name); // job이 0이 아니면 현재 프로세스가 작업 중
                        if(time == TIME_SLICE){ // time slice가 다 되었으면
                                time = 0; // time 초기화
                                if(ku_is_empty()) now = IDLE; // queue가 비었으면 IDLE 상태로 전환
                                else{ // queue가 비어있지 않으면
                                        now = ku_pop(); // queue에서 다음 프로세스를 pop
                                }
                                printk("——>Turn Over: %s\n", name); // time slice가 다 되었으므로 현재 프로세스가 종료되었다는 메시지 출력
				return 0;
                        }
			time++; // time slice가 다 되지 않았으면 time 증가
                }
                return 0;
        }
        else{// 현재 프로세스가 실행 중인 프로세스가 아니면
                if(ku_is_new(new_job.pid)) ku_push(new_job); // 새로운 프로세스가 queue에 없으면 queue에 push
        }
        return 1;
}

