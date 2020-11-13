#include "shell.h"


int main() { 
	char env[1024]="";
	char befehlLine[MAX], *parsedBefehle[MAX]; 
	int commandType = 0; 

	while (1) {
		// gebe die Shell-Line aus: 
		dir(); 
		
        // lese Eingabe vom User
		if ( readline(befehlLine) ) 
			continue; 
		
        // Prozess:
		//gibt 0 zurück, wenn es kein command ist oder wenn es ein buildin ist.
        commandType = findCommandType(befehlLine, parsedBefehle, env); 

		// führe den Befehl aus 
		//1 => einfacher command, wie ls
		if (commandType == 1) 
			execCmds(parsedBefehle); 

	} 
	return 0; 
} 
