// Include necessary headers for threading and synchronization
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

// Define the maximum number of processes
#define MAX_PROCESSES 100

// Define a structure to hold process information
// Each process has a type (reader or writer), a process ID, and a duration
typedef struct {
    int pid;
    int duration;
} Process;

// Define semaphores for synchronization
// rw_mutex is used to control access to the shared resource
sem_t rw_mutex;

// mutex is used to control access to the read_count variable      
sem_t mutex;    

// read_count keeps track of the number of readers currently accessing the resource
// It is initialized to 0, indicating no readers are currently accessing the resource
int read_count = 0;  

// Function prototypes for reader and writer threads
void* reader(void* arg) {
    Process* proc = (Process*)arg;

    // Print the process ID and indicate that the reader is trying to read
    printf("Reader %d is trying to read.\n", proc->pid);

    // Use semaphores to ensure mutual exclusion when accessing read_count
    sem_wait(&mutex);
    read_count++;
    if (read_count == 1) {
        sem_wait(&rw_mutex);  // First reader locks the resource
    }
    sem_post(&mutex);

    // Simulate reading by sleeping for the specified duration
    printf("Reader %d is reading for %d seconds...\n", proc->pid, proc->duration);
    sleep(proc->duration);
    
    // Print the process ID and indicate that the reader is done reading
    printf("Reader %d has finished reading.\n", proc->pid);

    sem_wait(&mutex);

    // Decrement the read_count and check if it's the last reader
    read_count--;
    if (read_count == 0) {
        sem_post(&rw_mutex);  // Last reader unlocks the resource
    }
    // Release the mutex to allow other readers or writers to access the resource
    sem_post(&mutex);

    pthread_exit(NULL);
}

// Function to simulate a writer process
void* writer(void* arg) {
    Process* proc = (Process*)arg;

    // Print the process ID and indicate that the writer is trying to write
    printf("Writer %d is trying to write.\n", proc->pid);
    sem_wait(&rw_mutex);

    // Simulate writing by sleeping for the specified duration
    printf("Writer %d is writing for %d seconds...\n", proc->pid, proc->duration);
    sleep(proc->duration);

    // Print the process ID and indicate that the writer is done writing
    printf("Writer %d has finished writing.\n", proc->pid);

    // Release the rw_mutex semaphore to allow other processes to access the resource
    sem_post(&rw_mutex);
    pthread_exit(NULL);
}

// Main function to read process information from a file and create threads
int main() {
    // Open the file containing process information with the format "R/W PID DURATION"
    FILE* file = fopen("processes.txt", "r");
    if (!file) {
        perror("Error opening processes.txt");
        return 1;
    }

    // Initialize variables to hold process information
    // type indicates whether the process is a reader (R) or writer (W)
    char type;
    // pid is the process ID, and duration is the time the process will take to read/write
    int pid, duration;
    // threads array holds the thread IDs for each process
    pthread_t threads[MAX_PROCESSES];
    // processes array holds pointers to Process structures for each process
    Process* processes[MAX_PROCESSES];
    // threadCount keeps track of the number of processes read from the file
    int threadCount = 0;

    // Initialize semaphores
    sem_init(&rw_mutex, 0, 1);
    sem_init(&mutex, 0, 1);

    // Read process information from the file until EOF
    while (fscanf(file, " %c %d %d", &type, &pid, &duration) == 3) {
        processes[threadCount] = malloc(sizeof(Process));
        processes[threadCount]->pid = pid;
        processes[threadCount]->duration = duration;

        // Create a thread for each process based on its type (reader or writer)
        if (type == 'R') {
            pthread_create(&threads[threadCount], NULL, reader, processes[threadCount]);
        } else if (type == 'W') {
            pthread_create(&threads[threadCount], NULL, writer, processes[threadCount]);
        } else {
            printf("Invalid process type: %c\n", type);
        }
        threadCount++;
    }

    fclose(file);

    for (int i = 0; i < threadCount; i++) {
        pthread_join(threads[i], NULL);
        free(processes[i]);
    }

    // Destroy the semaphores to free resources    
    // rw_mutex is destroyed to release the lock on the shared resource
    sem_destroy(&rw_mutex);
    // mutex is destroyed to release the lock on the read_count variable
    sem_destroy(&mutex);

    return 0;
}

