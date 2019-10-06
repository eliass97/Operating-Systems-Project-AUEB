#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <limits.h>

int numbers, threads, seed, mode, count;
int * array;
pthread_mutex_t mutex1; //Used for locking the read/write
pthread_cond_t cond1; //Used for locking print
pthread_mutex_t mutex2; //Used for print

int find(char * word, char ** arg);

int * randomize();

void writeFile(char * arrayType);

int * merge();

void * sort1(void * args);

void * sort2(void * args);

void * sort3(void * args);
