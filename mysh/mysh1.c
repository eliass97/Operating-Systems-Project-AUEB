#include "mysh-common.h"

int main(int argc, char **argv) 
{
    char *input; //Input
    char **arguments; //Array for words
    int state = 1; //Status for pid and state for the loop
    size_t buffer=255;
    while(state != 0) {
        arguments = NULL;
        input=readInput(1); //Read input
        if(input == NULL) { //If EOF
            printf("\n");            
            break;
        }
        arguments = malloc(buffer*sizeof(char*));
        arguments[0] = strtok(input,"\t\r\n\a "); //First word
        arguments[1] = strtok(NULL,"\t\r\n\a "); //Second word
        if(!arguments) { //Check errors
            printf("Failed to create array for arguments.");
            return -1;
        }
        if(arguments[1] != NULL) { //If more than 1 word
            printf("Please insert only 1-word commands.\n");
        } else {
            if(strcmp(input,"\0") == 0) { //No command
                //Nothing
            } else { 
                execute12(arguments,0); //Order execution
            }
        } 
    }
    if(arguments) {free(arguments);} 
    if(input) {free(input);}
    return 0;
}
