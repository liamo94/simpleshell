/***************************************************************************
 * 
 * Synopsis:
 * simpleshell
 *
 * Design Notes:
 * To create a terminal that will run on Linux, and replicate the functions
 * seen in a Linux terminal.
 *
 * Bugs:
 * See Revision History ;)
 *
 *
 * File: simpleshell.c
 *      
 *      
 *
 * Version: See VERSION below
 *
 **************************************************************************/
/********************* R E V I S I O N   H I S T O R Y ********************
 * v0.1 13/02/2014: Started to attempt stage 1 of ace4. Created a method that
 *          will split whatever you type into its individual tokens.
 *          Space, <, > and | are all ignored.
 *
 * v0.2 13/02/2014: We then began our main method. It is designed to run like
 *          a linux shell, and to fit the requirements for stage 1, it
 *          will tokenise the input, and exit when you type exit or
 *          press 'ctrl + d'.
 *
 * v0.3 20/02/2014: Added the exeCommand function which is from the lecture
 *          slides. It uses the fork() function to create a new child
 *          process. 
 *
 * v0.4 27/02/2014: Fixed some bugs in the fork() command, so now the stage 1
 *          still works with it.
 *
 * v0.5 06/03/2014: Added a perror() command so if command not recognised, it
 *          will print an error. We also checked to see if any input
 *          is made, if not it won't fork. 
 * 
 * v0.6 06/03/2014: Set the home directory, and made it so it resets once
 *          we exit the shell. There is a bug. If you type a command
 *          say 'ls', then exit after with 'ctrl + d, it will run the
 *          command again before its exits. It doesn't do this when you
 *          type exit.
 *
 * v0.7 06/03/2014: I fixed the errors, replacing a while statement with some
 *          if statements. Not only does the code now work, the main
 *          is easier to follow. Stage 1 and 2 complete
 *
 * v0.8 13/03/2014: Accidentally done stage 4 first, as we never realized our pwd
 *          wasn't working. It was using the pwd that You do when you fork. We
 *          had no trouble with setting and getting the path, and creates a function
 *          outside the main to set the home.
 *          
 * v0.9 16/03/2014: Fixed pwd. Added a check to make sure nothing is added after
 *          pwd.
 *
 * v1.0 20/02/2014: Done the part of the cd outside the main. Still to add call
 *          it within the main and do the relevant checks
 * v1.01 21/02/2014: Added the checks, like making sure 2 parameters aren't entered etc.
 *  
 * v1.1 25/03/2014: Added in setpath and getpath checks so they work correctly, forgot
 *          to include this earlier.
 *          
 * v1.2 25/03/2014: Created a class called call history. What this does is whenever a
 *          non history related command is entered, it will be added to an array of commands.
 *          Was getting a problem where all the commands stored where history, but using 
 *          strdup() fixed this. Added a way to extend the array, once it reaches full capacity 
 *          (20).
 *          
 * v1.3 25/03/2014: Started on the call history method. So far if you type 'history' it will
 *          print out the array of commands and if you type '!!' if will call the last
 *          command that you entered. I'm having difficulty making a 2D array, so if you type
 *          say 'cd Documents',  it will only add the cd part to the array.
 *  
 * v1.4 26/03/2014: Now call history allows you to type in ! followed by a number, and it
 *          will run whatever command number it is. For example, if you type !5, it will run 
 *          the 5th command. Had to also add checks. If its a digit, i.e '!e' would give an 
 *          error, and if the command exists, i.e if you type '!5', and there are only 4 
 *          commands in the array this will give an error as well. For some reason !0 doesn't 
 *          work, and no one in my group, as well as one of the demonstrators could understand 
 *          why as it should work.
 *          
 *  v1.5 27/03/2014: Fixed the part of the main where we call the history, so only the non-
 *          history commands get added to the history array. It still tries to fork the command 
 *          which is the only problem and If can't figure out how to fix this. Also had to fix 
 *          stage 1 as it broke while attempting stage 6.
 *          
 *  v1.6 02/04/2014: Removed all the warnings from compile. Added (unsigned char) which got rid 
 *          of them as the warnings where all because of this tokens[0][1], which will select 
 *          the 2nd character of the 1st token, i.e !4, it will get the 4 out. The errors are all 
 *          gone now though.
 *
 *          STAGES COMPLETED:
 *          1,2,3,4,5 AND 6.
 *          
 **************************************************************************/
#define VERSION "simpleshell V1.6. Last Update 12/04/2014\n"
 
#include <stdio.h>
#include <limits.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
 
#define MAX_INPUT_SIZE 512
#define MAX_TOKENS 50
#define MAX_HISTORY 20
 
 /*array to store history*/
 char *history[MAX_HISTORY];
 /*historySize, the number of commands entered. 0 to start as nothing entered.*/
 int historySize = 0;
  
 
/* Function for splitting the input and putting it into a single string array*/
  char** tokenise(char input[MAX_INPUT_SIZE], char *tokens[MAX_TOKENS]){
  char *command;
  int i = 0;
   
  command = strtok(input, " |><\n\t");
  tokens[0] = command;
 
  while(command != NULL && !feof(stdin)){
    printf("%s\n", tokens[i]);
    i++;
    command = strtok(NULL," |><\n\t");
    tokens[i] = command;
  }
  return tokens;
}
 
/*  Function that will set the home directory */
int setHome(){
    char* temp;
    temp = getenv("HOME");
    chdir(temp);
    return 0;
}
 
/*  Function to implement the pwd command */
int pwd(){
    char bufs[MAX_INPUT_SIZE];
    printf("%s\n", getcwd(bufs, MAX_INPUT_SIZE));
    return 0;
}
 
/*  Function to implement the cd command */
int cd(char **tokens) {
    char *path = tokens[1];
 
    if (path == NULL) {
        setHome();
    }
    /*If it can't find the path, i.e the directory doesn't exist, it will give an error.*/
    else if (chdir(path) == -1){
        perror("cd");
    }
    return 0;
}
 
/*  Function to add the history commands to an array */
void addHistory(char **tokens){
    int i;
    /*If the current history size is less than the max history size (20), then
    it adds it to the history array*/
    if(historySize < MAX_HISTORY)
            {
                history[historySize++] = strdup(*tokens);
            }
    /*Otherwise deletes the first command in the array to make room for the
    most recent one.*/
    else{
        for(i = 1; i < MAX_HISTORY; i++){
            history[i - 1] = history[i];
            }
        history[MAX_HISTORY - 1] = strdup(*tokens);
    }
}
 
/*  Function to call the history commands, such as 'history', !1!, etc. */
void callHistory(char **tokens){
    int i;
    char *cmd = tokens[0]; 
    int number1 = cmd[1] - '0';  /*Takes the 2nd character of the first token and converts it to an int*/
    int number2 = cmd[2] - '0';  /*Takes the 3rd character of the first token and converts it to an int*/
     
        /*Prints history if the command is 'history'*/
        if(strcmp(tokens[0], "history") == 0){
            for(i = 0; i < historySize; i++){
                printf("Command %d is %s\n",i, history[i]);
            }
        }
        /*Prints the last command if the command is '!!'*/
        else if(strcmp(tokens[0], "!!") == 0){
                tokens[0] = history[historySize - 1];
        }
        /*Check to see if there is something after !, if that thing is an int and not 'e' for example,
        and also a check to see if the command you want to call has been entered, i.e if you've entered 3 commands
        and you want to call the 5th, it won't let you. To get double digit numbers, we need to multiply the first
        digit by 10 and add it to the second, as digit one is would be 1 for a 2 digit number, instead of 10 like it 
        should. It is a bit cluttered and complicated but it works and can't figure out how to make it simpler :/ */
        else if((tokens[0][2] != 0) && (isdigit((unsigned char)tokens[0][1])) && 
        (isdigit((unsigned char)tokens[0][2])) && (((number1 *10) +number2) < historySize)){
            tokens[0] = history[(number1 *10) + number2];
        }
        /*Same as above, just a bit less complicated :P */
        else if((number1 != 0) && (tokens[0][2] == 0) && (isdigit((unsigned char)tokens[0][1])) && (number1 < historySize)){
            tokens[0] = history[number1];
        }
        /*If you enter just !, !e, !@, !6 when there are only 4 commands etc, then it will jump to this. */
        else{
            printf("Invalid history call\n");
            printf ("%s is not a command in history\n", cmd);
        }
 
}
 
     
/*  Function for creating a new process (fork()) */
void forkInstruction(char *tokens[MAX_TOKENS]){
    pid_t pid;
    pid = fork();
     
    if(pid<0){
        fprintf(stderr, "Fork Failed");
        exit(-1);
    }
     
    else if(pid == 0){
        execvp(tokens[0], tokens);
        perror(tokens[0]);
        exit(0);
    }
    else{
        wait(NULL);
    } 
}
 
/*---------------------------------------------------------------*/
/*-------------------o START OF THE MAIN o-----------------------*/
/*---------------------------------------------------------------*/
 
int main(int argc, char **argv){
  /*array to store the max input size*/
  char input[MAX_INPUT_SIZE];
  /*array to store the max amount of tokens*/
  char *tokens[MAX_TOKENS];
  /*The original path*/
  char *originalPath;
  printf("Entered shell\n");
   
 /* Store the path & set working directory */
    originalPath = getenv("PATH");
     
    setHome();
 
  while(1){
        printf("$");
        if(fgets(input, MAX_INPUT_SIZE, stdin) == NULL){
            break;
        }   
     
 /* tokenise the input and store the individual tokens in an array */
    tokenise(input, tokens);
  
    /*If the command if a history call, it won't add it to the history. Otherwise,
    every command will get added to the history array.*/
    if ((strncmp(tokens[0], "!", 1) == 0) || (strcmp(tokens[0], "history") == 0)){ 
        callHistory(tokens);
    }
    else {
        addHistory(tokens);
    }
     
 
 
 /* Create a new process */
    if (tokens[0] != NULL){
        /*If the command is 'getpath', it will print the current path. It can't
        take in a paramter*/
        if(strcmp(tokens[0], "getpath") == 0){
            if(tokens[1] == NULL){
                printf("The path is %s\n", getenv("PATH"));
            }
            else
                printf("getpath did not expect a variable\n");
        }
     
        /*If the command is 'setpath', it will set the path to whatever you want. 
        It must take in one parameter.*/
        else if(strcmp(tokens[0], "setpath") == 0){
                if(tokens[1] == NULL){
                    printf("The path is set to %d\n", setenv("PATH", tokens[1], 1));
                }
                else
                    printf("setpath needs a destination\n");
        }
 
        /*If the command is 'pwd', it will print the working directory. It can't take
        a parameter after pwd.*/
        else if(strcmp(tokens[0], "pwd") == 0){
                if(tokens[1] == NULL){
                    pwd();
                }   
                else
                    printf("pwd did not expect a variable\n");
        }
         
        /*For CD command. If there is no parameter, its sets it to home. If there is one parameter
        it will move to the location if it exists and if there is more than one parameter it will give
        an error.*/
        else if(strcmp(tokens[0], "cd") == 0){
            if(tokens[1] == NULL){
                setHome();
            }   
            else if (tokens[2] == NULL){
                cd(tokens);
                printf("\nNew location is: ");
                pwd();
                }
            else if (tokens[2] != NULL){
                printf("cd only expects one parameter\n");
            }
        }
        /*If the command is 'exit', it will break and then exit the program.*/
        else if(strcmp(tokens[0], "exit") == 0){
            printf("Exit code received");           
            break;
        }
         
        /*Otherwise we just fork the input.*/
        else
            forkInstruction(tokens);
             
        }
    }
    /*When we exit, it resets the path to the original path, and then prints it*/
        setenv("PATH", originalPath, 1);
        printf("\nThe System Path has been reset to: %s\n", getenv("PATH"));
         
    return 0;
}
 
/* Compile and run results...
liam@liam-laptop ~/Dropbox/UNI/Second Year/210
$ gcc simpleshell.c -o simpleshell -pedantic -Wall
 
liam@liam-laptop ~/Dropbox/UNI/Second Year/210
$ ./simpleshell
Entered shell
$date
date
Mon, Apr 14, 2014 10:34:15 AM
$pwd
pwd
/cygdrive/c/Users/liam
$history
history
Command 0 is date
Command 1 is pwd
history: No such file or directory
$
*/
