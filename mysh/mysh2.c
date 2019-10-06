#include "mysh-common.h"

int main(int argc, char **argv) {
    char * input; //Input
    char ** arguments; //Array for the words
    int state = 1;
    while(state != 0) {
        arguments = NULL;
        input=readInput(2); //Read input
        if(input == NULL) { //If EOF
            printf("\n");            
            break;
        }
        arguments = split(input); //Tokenize
        if(!arguments) { //Check errors
            printf("Failed to create array for arguments.");
            return -1;
        }
        if(strcmp(input,"\0") == 0) { //No command
            //Nothing
        } else { 
            execute12(arguments,0); //Order execution
        }
    }
    if(arguments){free(arguments);}
    if(input){free(input);}
    return 0;
}
