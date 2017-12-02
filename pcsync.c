#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct student{
    int sid;
    char firstname[64];
    char lastname[64];
    double cgpa;
};

typedef struct {
    int bufSize; 
    pthread_mutex_t mutex;
    pthread_cond_t can_produce;
    pthread_cond_t can_consume;
} buffer_t;

void* producer(void *arg) {
    buffer_t *buffer = (buffer_t *) arg;

    

    while (1) {

    }
}

void* consumer(void *arg) {
    
}

int main(int argc, char **argv) {

    // check argument count
    if(argc < 4) {
        printf("Insufficient amount of arguments provided...");
        exit(1);
    }
    // initialize parameters
    int buffersize = atoi(argv[1]);
    char *inFileName = argv[2];
    char *outFileName = argv[3];

    // check the limits of the buffersize and if illegal set it to the default
    if (buffersize > 1000 || buffersize < 10) {
        buffersize = 100;
    }

    // read file to initialize producer count
    FILE *fp;
    fp = fopen(inFileName, "r");

    int maxID = 0;
    int N = 0;
    int ta1;
    char *ta2;
    char *ta3;
    double *ta4;

    // initial pass to find the number of producers
    while(fscanf(fp, "%d %d %s %s %lf", &maxID, &ta1, ta2, ta3, &ta4)) {
        if (maxID > N) {
            N = maxID;
        }
    }
    fclose(fp);

    pthread_t threads[N];
    pthread_t cons;

    for (int i = 0; i < N; i++) {
        buffer_t buffer =  {
            .bufSize = buffersize,
            .mutex = PTHREAD_MUTEX_INITIALIZER,
            .can_produce = PTHREAD_COND_INITIALIZER,
            .can_consume = PTHREAD_COND_INITIALIZER
        };
        pthread_create(&threads[i], NULL, producer, (void*)&buffer);
    }

    buffer_t buffer =  {
            .bufSize = buffersize,
            .mutex = PTHREAD_MUTEX_INITIALIZER,
            .can_produce = PTHREAD_COND_INITIALIZER,
            .can_consume = PTHREAD_COND_INITIALIZER
    };

    pthread_create(&cons, NULL, consumer, (void*)&buffer);

    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_join(cons, NULL);

    return 0;
}