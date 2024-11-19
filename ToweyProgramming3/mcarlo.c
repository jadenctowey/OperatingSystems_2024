#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

void *thread_function(void *arg);

long total_hits = 0;       //Global hit count
pthread_mutex_t mutex;     //Mutex for synchronizing access to the global hit count    

double random_double(){            //Generates a double precision random number
    return random() / ((double)RAND_MAX + 1);
}

void *thread_function(void *arg){
    long points = *(long *)arg;     //Get # of points for this thread
    long hit_count = 0;             //Local hit counts for this thread

    for (long i = 0; i < points; i++){       //Check for points inside circle
        double x = random_double() * 2.0 - 1.0;     //generate random numbers between -1.0 and +1.0 (exclusive)
        double y = random_double() * 2.0 - 1.0;

        if (sqrt(x*x + y*y) < 1.0)      //Chekc (x, y) is in cirlce then add hit if it is
            ++hit_count;
        
    }

    pthread_mutex_lock(&mutex);     //lock before updating global
    total_hits += hit_count;        //update global
    pthread_mutex_unlock(&mutex);   //unlock

    pthread_exit((void *)"The CPU time!!");    //exit
}

int main(int argc, char *argv[]){
    if (argc != 2){         //check that it is right # of args
        fprintf(stderr, "Usage: %s <number of points>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    long points = atol(argv[1]);        //convert to long int
    if (points <= 0){               //make sure it is postive 
        fprintf(stderr, "Number of points must be positive integer\n");
        exit(EXIT_FAILURE);
    }

    pthread_t thread1, thread2;
    void *thread_result1;       //return thread 1
    void *thread_result2;       //return thread 2
    pthread_mutex_init(&mutex, NULL);

    long points_per_thread = points / 2;        //divide btwn two threads

    int res1 = pthread_create(&thread1, NULL, thread_function, &points_per_thread);     //FIRST THREAD
    if (res1 != 0){     //did it create thread?
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    int res2 = pthread_create(&thread2, NULL, thread_function, &points_per_thread);  //SECOND THREAD
    if (res2 != 0){     //did it create?
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
}
printf("----- waiting for threads to finish -----\n");       //waiting for threads to finish to keep going

res1 = pthread_join(thread1, &thread_result1);      //wait for thread 1
if (res1 != 0){         //errors?
    perror("thread join fail");
    exit(EXIT_FAILURE);
}

res2 = pthread_join(thread2, &thread_result2);      //wait for thead 2
if (res2 != 0){         //erorrs?
    perror("thread join fail");
    exit(EXIT_FAILURE);
}

double pi_estimate = 4.0 * total_hits / points;         //get pi using #s generated 
printf("estimated Pi = %f\n", pi_estimate);
printf("Thread 1 returned: %s\n", (char *)thread_result1);
printf("Thread 2 returned: %s\n", (char *)thread_result2);
pthread_mutex_destroy(&mutex);      //kill mutex (frees resources)

return 0;   //exit

}