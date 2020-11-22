//REFERENZEN: GeeksforGeeks abgerufen am 11.11.2020: 
//https://www.geeksforgeeks.org/making-linux-shell-c/

#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <unistd.h> //um fork zu nutzen
#include <sys/types.h> 
#include <sys/wait.h> 
#include <errno.h> 

#define MAX 1000 //maximale Länge von commands

// Clearing the shell using escape sequences 
#define clear() printf("\033[H\033[J") 


//HEADER
#include "shell.h"
#include "pipe.h" //erweiterung mit Pipe-Befehl


int main() { 
	char env[1024]="";
	char befehlLine[MAX], *parsedBefehle[MAX], *parsedPipeBefehle[MAX]; 
	int commandType = 0; 

	while (1) {
		// gebe die Shell-Line aus: 
		dir(); 
		
        // lese Eingabe vom User
		if ( readline(befehlLine) ) 
			continue; 
		
        // Prozess:
		//gibt 0 zurück, wenn es kein command ist oder wenn es ein buildin ist.
        commandType = findCommandType(befehlLine, parsedBefehle, env, parsedPipeBefehle); 

		printf("COMMANDTYPE: %d\n", commandType);

		// führe den Befehl aus 
		//1 => einfacher command, wie ls
		if (commandType == 1){
			printf("jetzt wird execCmd ausgeführt...\n"); 
			execCmds(parsedBefehle); 
		}

		//piped Befehl ausführen
		if (commandType == 2){
			printf("jetzt wird execPipe ausgeführt...\n"); 
			execPipe(parsedBefehle, parsedPipeBefehle);
		}

		//Arrays frei machen von Strings!
		memset(parsedPipeBefehle, '\0', sizeof(parsedPipeBefehle));
		memset(parsedBefehle, '\0', sizeof(parsedBefehle));
	} 
	return 0; 
} 
