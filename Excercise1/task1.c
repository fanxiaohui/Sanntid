// gcc 4.7.2 +
// gcc -std=gnu99 -Wall -g -o helloworld_c helloworld_c.c -lpthread

#include <pthread.h>
#include <stdio.h>

int i = 0;

// Note the return type: void*
void* thread1_func(){
  for(int j=0; j<1000000;j++){
  		i++;
  	}
    return NULL;
}

void* thread2_func(){
   for(int j=0; j<1000000;j++){
  		i--;
	}
    return NULL;
}


int main(){
    pthread_t thread1;
    pthread_t thread2;
    pthread_create(&thread1, NULL, thread1_func, NULL);
    pthread_create(&thread2, NULL, thread2_func, NULL);
    // Arguments to a thread would be passed here ---------^
    
   if(!pthread_join(thread1, NULL))
   		printf("Thread1_join sucsess\n");
    if(!pthread_join(thread2, NULL))
    	printf("Thread2_join sucsess\n");
    printf("Hello from main!\n");
    printf("i = %d\n",i);
    printf("Hello from Big Jonny");
    return 0;
    
}

OUTPUT
	Success	time: 0 memory: 18632 signal:0
	Thread1_join sucsess
	Thread2_join sucsess
	Hello from main!
	i = 0


