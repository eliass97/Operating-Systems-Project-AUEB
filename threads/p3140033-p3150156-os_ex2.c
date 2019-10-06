#include "p3140033-p3150156-os_ex2.h"

int main(int argc, char **argv) {
    if(argc != 5) { return -1; }
    numbers = find("-numbers=",argv); //Find numbers
    threads = find("-threads=",argv); //Find threads
    seed = find("-seed=",argv); //Find seed
    mode = find("-mode=",argv); //Find mode
    if(numbers == -1) { return -1; }
    if(seed == -1) { return -1; }
    //Allowed threads {1,2,4,8}
    if(threads != 1 && threads != 2 && threads != 4 && threads != 8) { return -1; }
    //Allowed mode {1,2,3} 
    if(mode != 1 && mode != 2 && mode != 3) { return -1; }
    //Each thread need at least 2 slots at the array
    if(numbers < 2*threads) { return -1; }
    //Initial array
    array = (int*)malloc(sizeof(int)*numbers);
    printf("Array size: %d\n",numbers);
    printf("Number of threads: %d\n",threads);
    printf("Seed: %d\n",seed);
    printf("Mode: %d\n",mode);
    printf("Randomizing...\n");
    array = randomize();//Create random integers
    writeFile("Initial"); //Write initial
    printf("Executing mergesort...\n");
    struct timeval start,end;
    gettimeofday(&start,NULL); //Start counting time
    count=threads-1;//Number of threads with first thread 0
    int rc = pthread_cond_init(&cond1,NULL);//Initialise condition variables
    if(rc != 0) { //Error check
        printf("Thread error!");
        return -1;
    }
    rc = pthread_mutex_init(&mutex1,NULL);//Initialise mutex
    if(rc != 0) { //Error check
        printf("Thread error!");
        return -1;
    }
    pthread_t threads_array[threads];//Create an array that contains the threads
    int * threadIds=malloc(sizeof(int) * threads);
    if(!threadIds) {
        printf("Failed to allocate memory for threads!");
        return -1;
    }
    //Create threads
    if(mode == 1) { //Use sort1
        for(int i=0;i<threads;i++) {
            threadIds[i]=i;
            rc = pthread_create(&threads_array[i],NULL,sort1,&threadIds[i]);
            if(rc != 0) { //Error check
                printf("Thread sort error!");
                return -1;
            }
        }
    } else if(mode == 2) { //Use sort2
        for(int i=0;i<threads;i++) {
            threadIds[i]=i;
            rc = pthread_create(&threads_array[i],NULL,sort2,&threadIds[i]);
            if(rc != 0) { //Error check
                printf("Thread sort error!");
                return -1;
            }
        }
    } else { //Use sort3
        for(int i=0;i<threads;i++) {
            threadIds[i]=i;
            rc = pthread_create(&threads_array[i],NULL,sort3,&threadIds[i]);
            if(rc != 0) { //Error check
                printf("Thread sort error!");
                return -1;
            }
        }
    }
    void * status;
    for(int i=0;i<threads;i++) { //Join threads
        rc=pthread_join(threads_array[i],&status);
        if(rc != 0) { //Error check
            printf("Failed  to join threads!");
            return -1;
        }
    }
    rc = pthread_cond_destroy(&cond1);
    if(rc != 0) { //Error check
        printf("Failed to destroy condition!");
        return -1;
    }
    rc = pthread_mutex_destroy(&mutex1);
    rc = pthread_mutex_destroy(&mutex2);
    array = merge(); //Merge the parts of each thread
    gettimeofday(&end,NULL); //Stop counting time
    long time = ((end.tv_sec*1000000+end.tv_usec) - (start.tv_sec*1000000+start.tv_usec)); //Compute time
    printf("Computing Time: %ld\n",time);
    writeFile("Sorted"); //Write results
    printf("Results have been saved in results.dat.\n");
    if(array) { //Free array
        free(array);
    }
    return 0;
}

int find(char * word, char ** arg) {
    int count = 1;
    int wordsize = strlen(word);
    int argsize;
    int i,j,ok;
    while(count < 5) {
        ok = 1;
        argsize = strlen(arg[count]);
        //Check if this is the argument we want
        for(i=0; i<wordsize; i++) {
            if(word[i] != arg[count][i]) {
                ok = 0;
                break;
            }
        }
        if(ok != 0) {
            //Create an array with a slot for each number
            char num[argsize-wordsize];
            j = 0;
            //Save the numbers in num[]
            for(i=wordsize; i<argsize; i++) {
                num[j] = arg[count][i];
                j++;
            }
            //Turn them into an integer
            i = atoi(num);
            //Return the integer if it's positive
            if(i>0) { return i; } else { return -1; }
        }
        //Argument doesn't match - try the next one
        count++;
    }
    return -1;
}

int * randomize() {
    srand(seed); //Seed
    for(int i=0; i<numbers; i++) {
        array[i] = rand()%(numbers*10); //Create random
    }
    return array;
}

void writeFile(char * arrayType) {
    FILE * fp = fopen("results.dat", "a");
    if(fp==NULL) { //Error check
        printf("Failed to open the file.\n");
        return;
    }
    //Write info of the array
    fprintf(fp,"%s array = {",arrayType);
    fprintf(fp,"%d",array[0]);
    for(int i=0; i<numbers; i++) {
        fprintf(fp,", ");
        fprintf(fp,"%d",array[i]);
    }
    fprintf(fp,"}\n");
    fclose(fp);
    return;
}

int * merge() {
    //Position of the smallest int for each thread
    int * pos = (int*)malloc(sizeof(int)*threads);
    int * arrayCopy = malloc(numbers*sizeof(int));
    memcpy(arrayCopy,array,numbers*sizeof(int));
    int i,j;
    //The position on array of each thread
    for(i=0;i<threads;i++) {
        pos[i] = i*(numbers/threads);
    }
    //Min int on array and the thread it belongs
    int thrNum, min;
    for(i=0;i<numbers;i++) {
        thrNum = -1;
        min = INT_MAX;
        //Search in all thread parts of array
        for(j=0;j<threads;j++) {
            //Find the min int between the thread parts
            if(pos[j]<(j+1)*(numbers/threads)) {
                if(arrayCopy[pos[j]] < min) {
                    min = arrayCopy[pos[j]];
                    thrNum = j;   
                }
            }
        }
        //The specific thread part moves 1 slot
        pos[thrNum]++;
        //Save the min at the new array
        array[i] = min; 
    }
    return array;
}


void * sort1(void * args) { //Locked read/write area
    //Thread number
    int * tid = (int *)args;
    //Start of the thread array part
    int l = (numbers/threads)*(*tid);
    //End of the thread array part
    int r = ((numbers/threads)*((*tid)+1))-1;
    printf("Started thread %d for sorting cells from %d to %d.\n",*tid+1,l,r);
    int rc = pthread_mutex_lock(&mutex1);
    if(rc!=0){ //Error check
        printf("Failed to lock!");
        pthread_exit(&rc);
    }
    int temp, min;
    for(int i=l;i<=r;i++) {
        min=i;
        for(int j=i+1;j<=r;j++) {
            if(array[j] < array[min]) {
                min = j;
            }
        }
        temp = array[min];
        array[min]=array[i];
        array[i]=temp;
    }
    rc=pthread_mutex_unlock(&mutex1);
    if(rc != 0) { //Error check
        printf("Failed to unlock!");
        pthread_exit(&rc);
    }
    rc=pthread_mutex_lock(&mutex2);
    if(rc != 0) { //Error check
        printf("Failed to lock!");
        pthread_exit(&rc);
    }
    while(*tid != count) {
        rc = pthread_cond_wait(&cond1,&mutex2);	
        if(rc!=0){
            printf("Thread error!");
            pthread_exit(&rc);
        }
    }
    printf("Thread#%d Sorted {",*tid+1);
    for(int i=l;i<=r;i++) {
        printf("%d,",array[i]);
    }
    printf("} \n");
    count--;
    rc = pthread_cond_broadcast(&cond1);
    if(rc != 0) { //Error check
        printf("Thread error!");
        pthread_exit(&rc);
    }
    rc = pthread_mutex_unlock(&mutex2);
    if(rc != 0) { //Error check
        printf("Failed to unlock!");
        pthread_exit(&rc);
    }
    pthread_exit(tid);
}

void * sort2(void * args) { //Locked write
    int * tid = (int *)args;
    int l = (numbers/threads)*(*tid);
    int r = ((numbers/threads)*((*tid)+1))-1;
    int temp, min, rc;
    printf("Started thread %d for sorting cells from %d to %d.\n",*tid+1,l,r);
    for(int i=l;i<=r;i++) {
        min=i;
        for(int j=i+1;j<=r;j++) {
            if(array[j] < array[min]) {
                min = j;
            }
        }
        rc = pthread_mutex_lock(&mutex1);
        if(rc != 0) { //Error check
            printf("Failed to lock!");
            pthread_exit(&rc);
        }
        temp = array[min];
        array[min]=array[i];
        array[i]=temp;
        rc = pthread_mutex_unlock(&mutex1);
        if(rc != 0) { //Error check
            printf("Failed to unlock!");
            pthread_exit(&rc);
        }
    }
    rc = pthread_mutex_lock(&mutex2);
    if(rc != 0) { //Error check
        printf("Failed to lock!");
        pthread_exit(&rc);
    }
    while(*tid != count) {
        rc = pthread_cond_wait(&cond1,&mutex2);	
        if(rc!=0){
            printf("Thread error!");
            pthread_exit(&rc);
        }
    }
    printf("Thread#%d Sorted {",*tid+1);
    for(int i=l;i<=r;i++) {
       printf("%d,",array[i]);
    }
    printf("} \n");
    count--;
    rc = pthread_cond_broadcast(&cond1);
    if(rc!=0){ //Error check
        printf("Thread error!");
        pthread_exit(&rc);
    }
    rc = pthread_mutex_unlock(&mutex2);
    if(rc!=0){ //Error check
        printf("Failed to unlock!");
        pthread_exit(&rc);
    }
    pthread_exit(tid);	
}


void * sort3(void * args) { //Free read/write
    int * tid = (int *)args;
    int l = (numbers/threads)*(*tid);
    int r = ((numbers/threads)*((*tid)+1))-1;
    printf("Started thread %d for sorting cells from %d to %d.\n",*tid+1,l,r);
    int min, temp;
    for(int i=l;i<=r;i++) {
        min=i;
        for(int j=i+1;j<=r;j++) {
            if(array[j] < array[min]) {
                min = j;
            }
        }
        temp = array[min];
        array[min]=array[i];
        array[i]=temp;
    }
    int rc;
    rc = pthread_mutex_lock(&mutex2);
    if(rc != 0) { 
        printf("Failed to lock!");
        pthread_exit(&rc);
    }
    while(*tid != count) {
        rc = pthread_cond_wait(&cond1,&mutex2);
        if(rc!=0){
            printf("Thread error!");
            pthread_exit(&rc);
        }
    }

    printf("Thread#%d Sorted {",*tid+1);
    for(int i=l;i<=r;i++) {
        printf("%d,",array[i]);
    }
    printf("} \n");
    count--;
    rc = pthread_cond_broadcast(&cond1);
    if(rc!=0){ //Error check
        printf("Thread error!");
        pthread_exit(&rc);
    }
    rc = pthread_mutex_unlock(&mutex2);
    if(rc!=0){ //Error check
        printf("Failed to unlock!");
        pthread_exit(&rc);
    }
    pthread_exit(tid);	
}
