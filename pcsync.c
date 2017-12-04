#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct student{
    long sid;
    char firstname[64];
    char lastname[64];
    double cgpa;
};

typedef struct {
    int producer_id;
    char *fileName;
    int bs; // size of the buffer
} buffer_t;

struct student **buffers; // global variable for the buffer
int *indices; // indicating which producers have an item available to consume
int producerCount = 0; 
int *consumeLocation; // indicating the location where the consumer can consume the student data
buffer_t *bufferts; // give buffer_ts to all created producers
int num_of_records = 0; // stop the consumer once all records have been consumed

pthread_mutex_t bufferLock = PTHREAD_MUTEX_INITIALIZER;
// pthread_mutex_t indexLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t can_produce = PTHREAD_COND_INITIALIZER;
pthread_cond_t can_consume = PTHREAD_COND_INITIALIZER;

void* producer(void *arg) {
    buffer_t *props = (buffer_t*)arg;
    int p_id = props->producer_id;
    printf("producer %d started\n", p_id);
    FILE *fp = fopen(props->fileName, "r");
    int id;
    long student_id;
    char first_name[64];
    char last_name[64];
    double gpa;
    while(fscanf(fp, "%d %ld %s %s %lf", &id, &student_id, first_name, last_name, &gpa) == 5) {
        // printf("%d %ld %s %s %lf\n", id, student_id, first_name, last_name, gpa);
        if (id == p_id) {       
            pthread_mutex_lock(&bufferLock);
            // critical section
            if (consumeLocation[p_id] == props->bs) {
                pthread_cond_wait(&can_produce, &bufferLock);
            }
            struct student *temp = (struct student *) malloc(sizeof(struct student));
            temp->sid = student_id;
            strcpy(temp->firstname, first_name);
            strcpy(temp->lastname, last_name);
            temp->cgpa = gpa; 
            buffers[p_id][consumeLocation[p_id]] = *temp;
            consumeLocation[p_id]++;
            indices[p_id] = 1;
            printf("%d %ld %s %s %lf loaded by %d\n", id, student_id, first_name, last_name, gpa, p_id);
            pthread_mutex_unlock(&bufferLock);
            pthread_cond_signal(&can_consume);
        }
    }
    pthread_exit(1);
}

void* consumer(void *arg) {
    printf("consumer started\n");
    struct student arr[200];
    int index = 0;
    buffer_t *props = (buffer_t *) arg;
    while (index < num_of_records) {
        printf("test\n");
        pthread_mutex_lock(&bufferLock);
        pthread_cond_wait(&can_consume, &bufferLock);
        printf("test after cond\n");
        for (int i = 0; i < producerCount; i++) {
            if (indices[i] == 1) {
                for (int j = 0; j < consumeLocation[i]; j++) {
                    arr[index] = buffers[i][j];
                    printf("inside consumer: %ld %s %s\n", arr[index].sid, arr[index].firstname, arr[index].lastname);
                    index++;
                }
                consumeLocation[i] = 0;
                indices[i] = 0;
            }
        }
        pthread_mutex_unlock(&bufferLock);
        pthread_cond_signal(&can_produce);
    }
    
    pthread_exit(1);
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
    long ta1 = -1;
    char ta2[64];
    char ta3[64];
    double ta4;

    // initial pass to find the number of producers
    while(fscanf(fp, "%d %ld %s %s %lf", &maxID, &ta1, ta2, ta3, &ta4) == 5) {
        num_of_records++;
        if (maxID > N) {
            N = maxID;
        }
    }
    N++;
    fclose(fp);
    
    // create producer thread array and the consumer thread
    pthread_t threads[N];
    pthread_t cons;
    bufferts = (buffer_t*) malloc(sizeof(buffer_t) * N);
    for (int i = 0; i < N; i++) {
        buffer_t buffer =  {.producer_id = i, .fileName = inFileName, .bs = buffersize};
        bufferts[i] = buffer;
    }
    // initialize all buffers for all producers as a global variable
    buffers = (struct student **) malloc(N * sizeof(struct student *));
    for (int i = 0; i < N; i++) {
        buffers[i] = (struct student *) malloc(buffersize * sizeof(struct student));
    }

    // create each producer thread
    for (int i = 0; i < N; i++) {
        int n = pthread_create(&threads[i], NULL, producer, (void*)&bufferts[i]);
    }
    // initialize indices array
    indices = (int *) malloc(sizeof(int) * (N+1));
    for (int i = 0; i < (N+1); i++) {
        indices[i] = 0;
    }
    // initialize locations array
    consumeLocation = (int *) malloc(sizeof(int) * (buffersize));
    for (int i = 0; i < (N+1); i++) {
        consumeLocation[i] = 0;
    }

    producerCount = N; 
    buffer_t buffer =  {.producer_id = -1, .fileName = inFileName, .bs = buffersize};
    pthread_create(&cons, NULL, consumer, (void*)&buffer);
    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_join(cons, NULL);

    return 0;
}