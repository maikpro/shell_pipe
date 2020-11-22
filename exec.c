#include <stdio.h>
#include <unistd.h>

int main(){
    
    char *cmd = "(sleep 10; cat -n)";
    char *argv[5];
    /*argv[0] = "(sleep";
    argv[1] = "10;";
    argv[2] = "cat";
    argv[3] = "-n)";
    argv[4] = NULL;*/

    /*char *cmd = "ls";
    char *argv[3];
    argv[0] = "ls";
    argv[1] = "-la";
    argv[2] = NULL;*/

    argv[0] = "sleep";
    argv[1] = "10";
    //argv[1] = " cat -n";
    argv[2] = NULL;


    if ( execvp("sleep", argv) < 0 ) { 
		printf("Der 1. Command kann nicht ausgeführt werden...\n"); 
	}  
    
    return 0;
}