#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define my_stack_push 335
#define my_stack_pop 336

int main(void){
	srand(time(NULL)); 

	int cnt, n, random, before; 
		//cnt: 3 - 스택에 담긴 수의 개수
		//n: 스택에 담긴 정수(syscall에서 반환됨)
		//random: stack에 push하기 위해 랜덤으로 생성한 정수(0~99)
		//before: 스택에 서로 다른 정수를 최소 세개 담기 위해 이전에 생성된 random변수 값을 저장하기 위핸 변수

       	cnt = 3;

	random = rand() % 100;
	while(cnt > 0){
		n = syscall(my_stack_push, random);
		printf("Push : %d\n", n);

		before = random;
		random = rand() % 100;
		while(random == before){
			n = syscall(my_stack_push, before);
			printf("Push : %d\n", n);
			random = rand() % 100;
		}

		cnt--;
	}

	for(cnt = 3; cnt > 0; cnt--){
		n = syscall(my_stack_pop);
		printf("Pop : %d\n", n);
	}

	return 0;
}
