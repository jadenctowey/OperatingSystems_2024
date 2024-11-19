#include <stdio.h>     //printf()
#include <stdint.h>    //int sizes
#include <stdlib.h>	   //rand()
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include "buffer.h"

BUFFER_ITEM buffer[BUFFER_SIZE];
int in = 0;
int out = 0;
sem_t *empty;
sem_t *full;
pthread_mutex_t mutex;

volatile int keep_running = 1;

uint16_t checksum(uint8_t *addr, uint32_t count)
{
    register uint32_t sum = 0; //Change from char * to uint8_t * 

    uint16_t *buf = (uint16_t *) addr;

    while(count > 1){           //sum loop
        sum = sum + *(buf)++;
        count = count - 2;
    }

    if (count > 0)     //left-over byte (if)
        sum = sum + *addr;

    while (sum>>16)   // 32-bit sum to 16 
        sum = (sum & 0xFFFF) + (sum >> 16);

    return(~sum);
}

//insert buffer
int insert_item(BUFFER_ITEM item){
    sem_wait(empty);//wait for empty slot
    pthread_mutex_lock(&mutex);//lock buffer

    buffer[in] = item;
    in = (in + 1) % BUFFER_SIZE;

    pthread_mutex_unlock(&mutex);//unlock buffer
    sem_post(full);//signal new item present

    return 0;
}

//remove bufffer
int remove_item(BUFFER_ITEM *item){
    sem_wait(full);//wait for filled slot
    pthread_mutex_lock(&mutex);//lock buffer

    *item = buffer[out];
    out = (out + 1) % BUFFER_SIZE;

    pthread_mutex_unlock(&mutex);
    sem_post(empty);

    return 0;
}

//producer 
void *producer(void *param){
    BUFFER_ITEM item;
    while(keep_running){
        for (int i = 0; i < 30; i++){
            item.data[i] = rand() % 256;//get random
        }
        item.cksum = checksum(item.data, 30);

        printf("producer: generated item with checksum %x\n", item.cksum);

        if (insert_item(item) == -1){
            fprintf(stderr, "error inserting item\n");
        }

        sleep(rand() % 3);//sleep for short rand time
    }
    return NULL; //ensure function return NULL
}

//consumer
void *consumer(void *param){
    BUFFER_ITEM item;
    while(keep_running){
        if (remove_item(&item) == -1){
            fprintf(stderr, "error removing item\n");
        }

        //verify chksum
        uint16_t cksum = checksum(item.data, 30);
        if (cksum != item.cksum){
            fprintf(stderr, "checksum error: expected %x, got %x\n", item.cksum, cksum);
            exit(EXIT_FAILURE);
        }
        printf("consumer: verified item with checksum %x\n", item.cksum);

        sleep(rand() % 3);
    }
    return NULL;
}

//main
int main(int argc, char *argv[]){
    if (argc != 4){
        fprintf(stderr, "Usage: %s <delay> <num_producers <num_consumers>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int delay = atoi(argv[1]);
    int num_producers = atoi(argv[2]);
    int num_consumers = atoi(argv[3]);

    empty = sem_open("/empty", O_CREAT, 0644, BUFFER_SIZE);     //semaphores and mutex
    if(empty == SEM_FAILED){
        perror("failed to open semaphore empty");
        exit(EXIT_FAILURE);
    }
    full = sem_open("/full", O_CREAT, 0644, 0);
    if (full == SEM_FAILED){
        perror("failed to open semaphore full");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_init(&mutex, NULL);

    pthread_t producers[num_producers], consumers[num_consumers];

    for (int i = 0; i < num_producers; i++){    //create producer threads
        if (pthread_create(&producers[i], NULL, producer,NULL) != 0){
        perror("failed to create producer thread");
        exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_consumers; i++){     //consumer threads
        if (pthread_create(&consumers[i], NULL, consumer,NULL) != 0){
        perror("failed to create consumer thread");
        exit(EXIT_FAILURE);
        }
    }

    sleep(delay);//sleep for specified delay

    keep_running = 0;//signal threads to stop

    for (int i = 0; i < num_producers; i++){     //join threads to terminate
        pthread_join(producers[i], NULL);
    }
    for (int i = 0; i < num_consumers; i++){
        pthread_join(consumers[i], NULL);
    }

    sem_unlink("/empty");    //clean
    sem_unlink("/full");
    sem_close(empty);
    sem_close(full);

    return EXIT_SUCCESS;
}