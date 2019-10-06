#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

char *readInput(int i); //Reads input from the user

char ** split(char * in); //Splits the input into words

int locate(char ** args, char * sign, int i); //Locates the position of the sign in the string array

char ** splitLR(char * input); //Splits input into 2 char** based on the position of the pipe

int containsPipe(char * input); //Checks for pipe

void openIO(char ** in,int i,int j); //Opens input/output streams

int numberOfPipes(char * input); //Counts the number of pipes

char ** seperatePipes(char * input, int count); //Splits the input into tokens according to the position of the pipes

void execute12(char ** args, int s); //Mysh1-2

void execute3(char ** args, int i, int j, int s); //Mysh3

void execute4(char * left,char * right); //Mysh4

void execute5(char * input); //Mysh5
