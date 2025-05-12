#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
//string parsers
char* lstrip(char *str);
char** parse_str_to_args(char *str, int *argc);
void reduce_spaces(char *str);

// commands binaries
int echo(char *msg);
int pwd();

const char *exit_str = "exit";
const char *echo_str = "echo";
const char *cd_str = "cd";
const char *pwd_str = "pwd";

const int MAX_CMD_LEN = 1<<20;

int picoshell_main(int argc, char *argv[]) {
    
    char cmd[MAX_CMD_LEN];
    char bin[MAX_CMD_LEN];
    int status = 0;
    while(1){
        
        printf("Pico shell prompt > ");
        if(fgets(cmd , MAX_CMD_LEN , stdin) == NULL){
            return status;
        }
        int sz = strlen(cmd) - 1;
        if(sz == 0){
            //status = 0;
            continue;
            
        }
        cmd[sz] = 0;
        reduce_spaces(cmd);
        int lst = -1;
        for(int i = 0;i <= sz;i++){
            if(cmd[i] == ' ' || cmd[i] == 0){
                bin[i] = 0;
                lst = i + 1;
                break;
            }
            bin[i] = cmd[i];
        }
        
        if(strcmp(bin , exit_str) == 0){
            // exit
            break;
        }else if(strcmp(bin , echo_str) == 0){
            // echo ......
            if(lst == -1 || lst >= sz){
                puts("Invalid command");
                status = -1;
                continue;
            }
            status = 0;
            echo(lstrip(cmd + lst));
        }
        else if(strcmp(bin , pwd_str) == 0){
            // pwd
            status = pwd();
        }
        else if(strcmp(bin , cd_str) == 0){
            // cd path
            if(lst == -1 || lst >= sz){
                puts("Invalid command");
                status = -1;
                continue;
            }
            status = 0;
            if((status = chdir(cmd + lst)) != 0){
                printf("cd: %s: %s\n",cmd + lst , strerror(2));
                //status = -1;
                continue;
            }
        }
        else{
            /*
            status = -1;
            puts("Invalid command");*/
            //printf("invoking child!\n");
            int chid = fork();
            if(chid > 0){
                //still inside parent
                int wstatus;
                wait(&wstatus);
                if (WIFEXITED(wstatus)) {
                    status = WEXITSTATUS(wstatus);
                    //printf("child status : %d\n",status);
                    if(status != 0){
                        if(status == 2){
                            printf("%s: command not found\n" , bin);
                        }
                        else
                            {
                                //printf("%s\n",strerror(status));
                            }
                        //status = -1;
                    }
                }
            }else if(chid == 0){
                //child process
                int argc_cmd;//seems to be useless for now
                char ** new_args = parse_str_to_args(cmd , &argc_cmd);
                //printf("%s #\n",bin);
                execvp(bin , new_args);
                //if we come here again, error occured
                //printf("%d\n" , errno);
                _exit(errno);
                
            }else{
                // failed
                status = -1;
            }
        }
        
    }
    puts("Good Bye");
    return status;
}
int echo(char *msg) {
    puts(msg);
    return 0;
}
int pwd() {
    
    char *path = getcwd(NULL , 0);
    if(path){
        printf("%s\n" , path);
        free(path);
        return 0;
    }else{
        //printf("Couldn't PWD\n");
        return -1;
    }
}

/**
 * parses string to argv format , and counts number of args to argc
 */
 
char** parse_str_to_args(char *str, int *argc) {
    if (str == NULL || argc == NULL) {
        return NULL;
    }

    int capacity = 10;
    char **args = (char**)malloc(capacity * sizeof(char*));
    if (!args) return NULL;

    int count = 0;
    char *token = strtok(str, " ");
    while (token) {
        // Grow the array if needed
        if (count + 1 >= capacity) {
            capacity *= 2;
            char **tmp = (char**)realloc(args, capacity * sizeof(char*));
            if (!tmp) {
                free(args);
                return NULL;
            }
            args = tmp;
        }

        args[count++] = token;
        token = strtok(NULL, " ");
    }

    // NULL-terminate the array
    args[count] = NULL;

    *argc = count;
    return args;
}

/**
 * returns pointer to first character following leading white spaces
 * */
char* lstrip(char *str){
    if(str == NULL)return NULL;
    while((*str) == ' ')str++;
    return str;
}

void reduce_spaces(char *str){
    if(str == NULL)return;
    int sz = strlen(str);
    //printf
    char mem[sz + 1];
    mem[sz] = 0;
    int p1 = 0 ,p2 = 0;
    for(;p1 < sz;p1++){
        //printf("%d \n",p1);
        if(str[p1] != ' ')continue;
        if(p1 && str[p1 - 1] != ' ')continue;
        while(p2 < sz && (p2 <= p1 || str[p2] == ' '))p2++;
        //printf("%d \n",-1);
        if(p2 == sz){
            //remove trailling spaces
            str[p1] = 0;
            if(p1 && str[p1 - 1] == ' ')str[p1 - 1] = 0;
            break;
        }
        while(p2 < sz && str[p2] != ' '){
            str[p1] = str[p2];
            str[p2] = ' ';
            p1++;
            p2++;
        }

        p1--;
    }
    
}
int main(){

    int res = picoshell_main(0 , NULL);
    printf("return = %d\n" , res);
    return 0;
}
