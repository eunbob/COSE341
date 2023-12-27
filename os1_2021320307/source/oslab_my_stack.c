#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/linkage.h>
#define STACK_SIZE 100

int stack[STACK_SIZE]; 
int top = -1; //stack의 인덱스, 0번째 index부터 시작하기 위해 -1로 초기선언

SYSCALL_DEFINE1 (os2023_push, int, a){
	printk(KERN_INFO "[System Call] os2023_push() :\n");
	int i, already; 
	//i: for loop에서 활용하기 위한 변수
	//already: 이미 들어 있는 숫자인지 확인하기 위한 변수 

	//stack이 꽉 차있으면 알림 출력 후 종료
	if(top >= STACK_SIZE - 1){
		printk("Stack is full\n");
		return;
	}
	already = 0; 
	//stack에 push하려는 정수가 이미 있는지 확인
	for(i = top; i >= 0; i--){
		if(stack[i] == a){
			already = 1; //있다면 표시
			break;
		}  
	}

	//같은 숫자가 스택에 없다면 넣어줌
	if(!already){
		top = top + 1;
		stack[top] = a;
	}

	//커널에 stack 원소 출력 
	printk("Stack Top --------------------\n");
	for(i = top; i >= 0; i--){
		printk("%d\n", stack[i]);
	}
	printk("Stack Bottom --------------------\n");

	//push한 정수 반환하며 종료
	return a;
}
SYSCALL_DEFINE0 (os2023_pop){
	printk(KERN_INFO "[System Call] os2023_pop :\n");
	int i, j, temp, max;
	//i, j: for loop에서 활용하기 위한 변수
	//temp: 버블 정렬에서 활용할 변수
	//max: 스택의 최댓값을 담을 변수

	//스택이 비어있다면 알림 출력 후 종료
	if(top <= -1){
		printk("Stack is empty\n");
		return;
	}

	//버블정렬(오름차순)
	for(i = 0; i <= top; i++){
		for(j = 0; j <= top - 1 - i; j++){
			if(stack[j] > stack[j + 1]){
				temp = stack[j];
				stack[j] = stack[j + 1];
				stack[j + 1] = temp;	
			}
		}
	}

	//max에 최댓값 저장
	max = stack[top];
	
	//커널에 pop이후 스택 출력 
	printk("Stack Top --------------------\n");
	for(i = --top; i >= 0; i--){
		printk("%d\n", stack[i]);
	}
	printk("Stack Bottom --------------------\n");
	
	//최댓값(pop결과값) 반환하며 종료
	return max;	
}
