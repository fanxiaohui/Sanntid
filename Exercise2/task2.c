//
//  main.c
//  SanntidEx2
//
//  Created by Vegard Brekke Løvvig on 19.01.2016.
//  Copyright © 2016 Vegard. All rights reserved.
//

// gcc 4.7.2 +
// gcc -std=gnu99 -Wall -g -o helloworld_c helloworld_c.c -lpthread

#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

int i = 0;

pthread_mutex_t mutex;

// Note the return type: void*
void* thread1_func(){
    
    for(int j=0; j<1000000;j++){
        pthread_mutex_lock(&mutex);
        i++;
        pthread_mutex_unlock(&mutex);
    }
    
    return NULL;
}

void* thread2_func(){
    
    for(int j=0; j<1000000;j++){
        pthread_mutex_lock(&mutex);
        i--;
        pthread_mutex_unlock(&mutex);
    }
   
    return NULL;
}


int main(){
    
    pthread_mutex_init(&mutex,NULL);
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
    pthread_mutex_destroy(&mutex);
    return 0;
    
}



