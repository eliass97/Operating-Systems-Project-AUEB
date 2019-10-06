#include "mysh-common.h"

int main(int argc, char **argv) {
    char * input; //Input
    char ** arguments; //Array for the words
    int i=-1; //i,j=ints for positions of < and >/>> on arguments array
    int j=-1; //-1 represents no input/output
    int state = 1;
    while(state != 0) {
	j=-1;
	i=-1;
        arguments = NULL;
        input=readInput(3); //Read input
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
        } else { //Order execution
            i = locate(arguments,"<",0); //Locate position of <
            j = locate(arguments,">",0); //Locate position of >
            if(j == -1) { //If > was not found
                j = locate(arguments,">>",0); //Locate position of >>
            }
            if (i == -1 && j == -1) { //No input and no output
                execute12(arguments,0); //Execute commands
            } else {
                execute3(arguments,i,j,0); //Execute commands using files
            }   
        } 
    }
    if(arguments) {free(arguments);}
    if(input) {free(input);}
    return 0;
}
