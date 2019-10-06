#include "mysh-common.h"

char * readInput(int i) {
    int input_size = 255;
    char * input = malloc(input_size*sizeof(char));
    if (!input) { //Error check
        fprintf(stderr, "ERROR: Failed to allocate memory.\n");
        exit(-1);
    }
    printf("mysh%d>",i);
    input = fgets(input,input_size*sizeof(char),stdin);  
    if(input == NULL) return NULL;  
    size_t length = strlen(input);
    input[length-1]='\0';//Replaces \n with \0
    return input;
}

int locate(char ** args, char * sign, int i) { 
    int k = -1;
    while(args[i] != NULL) {
        if(strcmp(args[i],sign) == 0) {
            k = i;
            break;
        }
        i++;
    }
    return k;
}

char ** splitLR(char * input) {
    char ** output=malloc(2*sizeof(char**));
    if(!output) {
        fprintf(stderr, "ERROR: Failed to allocate memory.\n");
        exit(-1);
    }
    output[0]=strtok(input,"|");
    output[1]=strtok(NULL,"|");
    return output;
}

int containsPipe(char * input) {
    size_t length=strlen(input);
    size_t i;
    for(i=0;i<length;i++) {
        if(input[i]=='|') return 1;	
    }
    return 0;
}

char ** split(char * in) {
    size_t buffer = 255;
    int pos = 0; //Position in the array
    char ** out = malloc(buffer*sizeof(char*)); //Create the array
    if (!out) { //Check if the creation was successful
        fprintf(stderr, "ERROR: Failed to allocate memory.\n");
        exit(-1);
    }
    char * token = strtok(in," "); //Pick the first token
    while(token != NULL) { //Do the same for all tokens
        out[pos] = token; //Save it in the array
        pos++; //Next position
        if (pos >= buffer-1) { //Check space availability 
            buffer = buffer + 255;
            out = realloc(out,buffer*sizeof(char*));
            if (!out) { //Check if the recreation was completed
                fprintf(stderr, "ERROR: Failed to allocate memory.\n");
                exit(-1);
            }
         }
         token = strtok(NULL, " "); //Next token
    }
    out[pos] = NULL; //Last position is null
    return out;
}

void openIO(char ** args,int i,int j) {
    int fd[2];//fd[0]=input,fd[1]=output
    if(i != -1) { //If input exists 
        int check = locate(args,"<",i+1);
        while(check!=-1) {
            args[i]=NULL;
            i=check;
            check=locate(args,"<",i+1);
        }	
        fd[0] = open(args[i+1],O_RDONLY,0);
        if(fd[0] < 0) { //Check for errors
            perror("ERROR: Failed to open input file.\n");
	    return;
        }
        dup2(fd[0], 0); //Make fd[0] the input stream
        close(fd[0]);
        args[i] = NULL; //Replace < with NULL
    }
    if(j != -1) { //If output exists 
        if(strcmp(args[j],">") == 0) {
            fd[1] = creat(args[j+1], 0644);
        } else { //Append mode 
            fd[1] = open(args[j+1],O_WRONLY|O_APPEND,1);
        }
        if(fd[1] < 0) { //Check for errors
            perror("ERROR: Failed to open file.\n");
            return;
        }
        dup2(fd[1], STDOUT_FILENO); //Make fd[1] the output stream 
        close(fd[1]);
        args[j] = NULL; //Replace >>/> with NULL
    }
}

int numberOfPipes(char * input) {
    int count = 0;
    size_t length = strlen(input);
    size_t i;
    for(i=0;i<length;i++) {
        if(input[i] == '|') count ++;
    }
    return count;
}

char ** seperatePipes(char * input, int count) {
    count = numberOfPipes(input) + 1;
    char ** args = malloc(count*sizeof(char*));
    if(!args) {
        fprintf(stderr, "ERROR: Failed to allocate memory.\n");
        exit(-1);
    }
    args[0] = strtok(input,"|");
    for(int i=1;i<count;i++) {
        args[i] = strtok(NULL,"|");
    }
    return args;
}

void execute12(char ** args, int s) {
    pid_t pid, waitPid; //Proccesses
    int status;
    pid = fork();
    if (pid < 0) { //Error is case fork fails
        perror("ERROR: Fork failed.\n");
        return;
    } else if (pid == 0) { //Child proccess
        execvp(args[s],args);
	perror("ERROR: Order execution failed.\n");
        return;
    } else { //Parent proccess
	waitPid = wait(&status);
	if (waitPid == -1) {
            perror("ERROR: Waitpid failed.\n");
            return;
	}
    } 
}

void execute3(char ** args, int i, int j, int s) {
    pid_t pid, waitPid; //Proccesses
    int status;
    int fd[2];//0 input, 1 output
    pid = fork();
    if (pid < 0) { //Error is case fork fails
        perror("ERROR: Fork failed.\n");
        return;
    } else if (pid == 0) { //Child proccess
        if(i != -1) { //If input exists
            int check = locate(args,"<",i+1);
            while(check!=-1) {
                args[i]=NULL;
                i=check;
                check=locate(args,"<",i+1);
            }			
            fd[0] = open(args[i+1],O_RDONLY,0);
            if(fd[0] < 0) { //Check for errors
                perror("ERROR: Failed to open input file.\n");
                return;
            }
            dup2(fd[0], 0); //Duplicate
            close(fd[0]);
            args[i] = NULL; //Replace < with NULL
        }
        if(j != -1) { //If input exists 
            if(strcmp(args[j],">") == 0) {
                fd[1] = creat(args[j+1], 0644);
            } else { //Append mode 
                fd[1] = open(args[j+1],O_WRONLY|O_APPEND,1);
            }
            if(fd[1] < 0) { //Check for errors
                perror("ERROR: Failed to open file.\n");
                return;
            }
            dup2(fd[1], STDOUT_FILENO); //Duplicate
            close(fd[1]);
            args[j] = NULL; //Replace >>/> with NULL
        }
        execvp(args[s],args); //Execute till </>/>>
        perror("ERROR: Order execution failed.\n");
    } else { //Parent proccess
        waitPid = wait(&status);
        if (waitPid == -1) {
            perror("ERROR: Waitpid failed.\n");
            return;
        }
    }
}

void execute4(char * leftInput,char * rightInput) {
    pid_t pid,waitPid;
    int status,l[2],r[2],pipefd[2];//l[0],r[0] will be input,l[1],r[1] will be output
    char ** left=split(leftInput);
    char ** right=split(rightInput);
    if(!left){
        printf("Failed to create array for arguments.\n");
        return;
    }
    if(!right){
       printf("Failed to create array for arguments.\n");
       return;
    }
    l[0] = locate(left,"<",0);//locate < in left command
    l[1] = locate(left,">",0);//locate > in left command
    if(l[1]==-1) { 
        l[1]=locate(left,">>",0);//locate >> in left
    }
    r[0] = locate(right,"<",0);//locate < in right command
    r[1] = locate(right,">",0);//locate > in right command
    if(r[1]==-1) { 
        r[1]=locate(right,">>",0);//locate >> in right
    }	
    if(pipe(pipefd)<0) {
        perror("ERROR: Pipe failed.\n");
        return;
    }
    pid=fork();
    if(pid<0) {
        perror("ERROR: Fork failed.\n");
        exit(-1);
    } else if(pid==0) {//child,left command
        if(l[1]==-1) {
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
        }
        openIO(left,l[0],l[1]);
        execvp(left[0],left);
        perror("ERROR: EXECVP failed on left command.\n");
        exit(-1);
    } else {//right command
        waitPid=wait(&status);
        if(waitPid==-1) {
            perror("ERROR: Waitpid failed.\n");
            exit(-1);
        }
        if(r[0]==-1) {
            close(pipefd[1]);
            dup2(pipefd[0],STDIN_FILENO);
        }
        openIO(right,r[0],r[1]);
        execvp(right[0],right);
        perror("ERROR: EXECVP failed on right command.\n");
        exit(-1);
    }
    if(left) {free(left);}
    if(right) {free(right);}
}

void execute5(char * input) {
    pid_t pid,waitPid;
    int status;
    int c=numberOfPipes(input)+1;	
    char ** args=seperatePipes(input,c);
    char ** command;
    int pipefd[2];
    int pipevalue;
    int old_fd=0;
    int in,out;
    for(int i=0;i<c;i++) {
        pipevalue=pipe(pipefd);
        if(pipevalue<0) {
            perror("ERROR: Pipe failed.\n");
            exit(-1);
        }
        in=-1;
        out=-1;
        command=split(args[i]);
        pid=fork();
        if(pid<0){exit(-1);}
        else if(pid==0) {
            in=locate(command,"<",0);
            out=locate(command,">",0);
            if(out==-1) { 
                out = locate(command,">>",0);
            }
            openIO(command,in,out);
            if(in==-1) {
                dup2(old_fd,STDIN_FILENO);
            }
            if((i!=c-1)&&out==-1) {
                dup2(pipefd[1],STDOUT_FILENO);//write to pipeline
            }
            execvp(command[0],command);
            perror("ERROR: EXECVP failed on right command.\n");
            exit(-1);
        } else { 
            waitPid=wait(&status);
            if(waitPid==-1) exit(-1);
            if(old_fd!=0) close(old_fd);//if old_fd=0 then we'll be closing the STDIN, which is not right
            old_fd=pipefd[0];
            close(pipefd[1]);
        } 
    }
    if(command) {free(command);}
    if(args) {free(args);}
}
