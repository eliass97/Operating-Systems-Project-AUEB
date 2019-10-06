#include "mysh-common.h"

int main(int argc, char **argv) {
    char * input; //Input
    int i,j,k; //Ints for positions of < , >/>> and | on arguments array
    int state = 1;
    char ** arguments;
    while(state != 0) {
        i = -1; j = -1; k = -1; //Represents no input, no output and no pipe
        arguments = NULL;
        input=readInput(5); //Read input
        if(input == NULL) { //If EOF
            printf("\n");        
            break;
        }
        k = containsPipe(input);
        if(strcmp(input,"\0") == 0) { //No command
            //Nothing
        } else if(k==0) { //No pipe
            arguments = split(input); //Array for the words
            i = locate(arguments,"<",0); //Locate position of <
            j = locate(arguments,">",0); //Locate position of >
            if(j == -1) j = locate(arguments,">>",0); //If > was not found,locate position of >>		
            if (i == -1 && j == -1) execute12(arguments,0); //No input, no output and no pipe
            else execute3(arguments,i,j,0); //Input or Output and no pipe
        } else { //If it contains a pipe
            pid_t pid,waitPid;
            int status;
            pid=fork();
            if(pid<0){ //Check errors
                printf("ERROR: Fork failed.\n");
                return -1;
            } else if(pid==0) {
                execute5(input);
            } else {
                waitPid=wait(&status);
                if(waitPid==-1){
                    perror("ERROR: Waitpid failed.\n");
                    exit(-1);
                }
            }
        }
    }
    if(input) {free(input);}
    if(arguments){free(arguments);}
    return 0;
}
