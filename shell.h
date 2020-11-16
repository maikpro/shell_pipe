//REFERENZEN: GeeksforGeeks abgerufen am 11.11.2020: 
//https://www.geeksforgeeks.org/making-linux-shell-c/

#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> //um fork zu nutzen
#include<sys/types.h> 
#include<sys/wait.h> 
#include <errno.h> 

#define MAX 1000 //maximale Länge von commands

// Clearing the shell using escape sequences 
#define clear() printf("\033[H\033[J") 

/*
	Quelle: https://stackoverflow.com/questions/55672661/what-this-character-sequence-033h-033j-does-in-c
	\033 - ASCII escape character
	[H - move the cursor to the home position
	[J - erases the screen from the current line down to the bottom of the screen
*/

void printArray(char **array){
	printf("\n=============================\n");
	//durchlaufe alle geparsten Befehle
    for(int j = 0; j<strlen(*array); j++){
		if( array[j] == NULL ){
			return;
		}
		printf("Befehl %d: %s\n", j+1, array[j]);
		printf("Befehl-Länge %ld\n", strlen( array[j] ));
    }
	printf("=============================\n");
}

//gibt das jetztige Verzeichnis aus.
void dir() { 
	char path[1024]; 
	realpath(".", path); 
	printf("\n%s>", path); 
} 

// Help command builtin 
void help() { 
	printf("\n***WELCOME TO MY SHELL HELP***"
		"\nList of Commands supported:"
		"\n>cd: wechselt das Verzeichnis."
		"\n>ls: zeigt das aktuelle Verzeichnis an."
		"\n>export: fügt eine custom Variable in env ein. export ...=..."
		"\n>showenv: zeigt alle custom Variablen in env an."
		"\n>clear: löscht alle Eingaben."
		"\n>exit: schließt das Programm."); 

	return; 
} 

//gibt env aus.
void showenv(char *env){
	if(strlen(env) < 1){
		printf("env ist leer!\n");
	} else{
		printf("***env:*** \n%s\n", env);
	}
}

//teilt alle strings von str durch delimiter auf und packt sie in das Array str2
void aufteilen(char *str, char **str2, char *delimiter){
	int i = 0;

    //teilt den ersten Teil von BefehlLine auf bei Leerzeichen " "
    char *p = strtok(str, delimiter);

	//teile weitere Teile von BefehlLine auf.
	while( p != NULL ){
		str2[i++] = p;
		p = strtok(NULL, delimiter);
	}
}

int checkIfExists(char *env, char *var){
	char varGleich[MAX]="";
	char sucheVar[MAX]="";

	strcat(sucheVar, var);

	char *p = strtok(sucheVar, "=");
	strcat(varGleich, p);
	strcat(varGleich, "=");
	printf("Suche nach Variable: %s\n", varGleich);
	

	if( strstr(env, varGleich) != NULL ){
		char *result = strstr(env, varGleich);

		//lösche vorhandene Variable aus env.
		memset(result,0,strlen(result));
	
	}

	//gefunden 
	return 1;

	//nicht gefunden
	return 0;
}

//legt eine Variable in env ab.
void exportenv(char **parsedBefehle, char *env){
    char *var;

	//schaue ob die Variable im env existiert, falls ja wird sie gelöscht.
	checkIfExists(env, parsedBefehle[1]);

	//var wird in env eingefügt.
	//Beim export müssen alle befehle durchlaufen werden
	for(int i = 1; i<strlen(*parsedBefehle); i++){
		if( parsedBefehle[i] == NULL){
			return;
		}
		var = parsedBefehle[i];

		//Variable wird in env gepackt:
		strcat(env, var);
		if( parsedBefehle[i+1] != NULL){
			strcat(env, " "); 
		}
		
		//entfernt überschüssige Befehle für nächste Eingabe
		memset(parsedBefehle[i], 0, strlen(parsedBefehle[i]));
	}
	strcat(env, "\n");//nächste Zeile
}

/*überprüft, ob es sich um einen builtin command handelt oder um ein normalen.
gibt 0 aus, wenn es ein einfacher command ist
und gibt 1 aus, wenn es ein buildin ist.*/
int checkCommandType(char** parsedBefehle, char *env) { 
	int anzahl = 7, auswahlCmd = 0; 
	//environment
	
	char *builtInCmds[anzahl]; 

	//builtin commands
	builtInCmds[0] = "exit\n"; 
	builtInCmds[1] = "cd"; 
	builtInCmds[2] = "help\n"; 
	builtInCmds[3] = "showenv\n"; 
	builtInCmds[4] = "export"; 
	builtInCmds[5] = "clear\n"; 
	builtInCmds[6] = "echo"; 

	//geht die Liste der buildIns durch
	for (int i = 0; i < anzahl; i++) { 
		if (strcmp(parsedBefehle[0], builtInCmds[i]) == 0) { 
			auswahlCmd = i + 1; 
			break; 
		} 
	} 

	switch (auswahlCmd) { 
	case 1: 
        //exit
		printf("\nTschüss! :-) \n"); 
		exit(0); 
	case 2: 
        //cd
		strtok( parsedBefehle[ 1 ], "\n" );
		chdir(parsedBefehle[1]); 
		return 1; 
	case 3: 
		//help
		help(); 
		return 1;

	case 4:
		//showenv
		showenv(env);
		return 1; 
	
	case 5:
		//exportenv
		//übergebe die befehle und das env
		exportenv(parsedBefehle, env);
		return 1; 

	case 6:
		//clear
		clear();
		return 1; 

	case 7: 
		//echo
		//strtok( parsedBefehle[ 1 ], "\n" );
		printf("%s\n", parsedBefehle[1]);
		return 1;

	default: 
		break; 
	} 

	return 0; 
} 

void toString(char *buffer){
    printf("%s\n", buffer);
}

//lese Eingabe vom User
int readline(char *befehlLine) { 
    char buf[MAX];

    //Befehl auslesen
    fgets(buf, MAX, stdin);

    //wenn der Befehl nicht leer ist, kopiere die Zeile in befehlLine
    if( strlen(buf) != 0 ){
        strcpy(befehlLine, buf);
        return 0;
    } else{
        return 1;
    }
} 

//Suche nach export-Variable in env und löse sie auf.
void ersetzeVariable(char **parsedBefehle, char *env, char *var, int i){
	//"var="
    char varGleich[MAX]="";
    //das Kopie von env als Array.
    char *envArr[MAX];
    //Kopie von env
    char envCpy[MAX];
    //Such Schlüssel
    const char *needle;
    //Inhalt der Variablen
    char *varInhalt[MAX];
    
    //schneidet umbruch raus.
    strtok(var, "\n");
    
    //fügt die Variable in diesem Format zusammen: var=
    strcat(varGleich, var);
    strcat(varGleich, "=");
    //

    //Schlüssel festlegen: var=
    needle = varGleich;

    //kopieren von env nach envCpy
    strcpy(envCpy, env);

    //envCpy wird in ein Array verwandelt.
    aufteilen(envCpy, envArr, "\n");

    //Suche nach Variablen in env
    for( int j = 0; j<strlen(*envArr);j++ ){
        //falls leer
        if(envArr[j] == NULL){
            break;
        }

        //"Such nach Nadel im Heuhaufen"
        //Suche nach var= und gebe den Inhalt hinter = aus
        //Beispiel: abc="hello" -> Suche nach abc= -> gibt "hello" aus.
        if( strstr(envArr[j], needle) != NULL ){
            aufteilen(envArr[j], varInhalt, "=");
            parsedBefehle[i] = varInhalt[1];
			return;
        }
    }

	printf("Variable gibt es nicht in env!\n");
}

//Umgebungsvariablen wie $HOME auflösen.
void varAufloesen(char **parsedBefehle, char *env){
    //var auflösen nach $
    char *key = "$";

    //aufgelöste Variable
    char *var;
	
    //durchlaufe alle geparsten Befehle
    for(int i = 0; i<strlen(*parsedBefehle); i++){
		//wenn Array leer sein sollte:
        if( parsedBefehle[i] == NULL ){
			return;
		}
		
        // $ auflösen, falls es in Befehlen vorkommt:
		if( strstr(parsedBefehle[i], key) != NULL ){	
			strtok( parsedBefehle[ strlen(*parsedBefehle)-1 ], "\n" );
			var = strtok(parsedBefehle[i], "$");
			
			//TEST
			strtok(var,"\n");
			//

            //wenn $ eine env variable (nicht bash) aufgelöst werden soll:
			if(getenv(var) == NULL){
				//suche nach variable und löse sie auf:
				if(strlen(env) < 1){
					printf("env hat keine Variablen! Nutze export ... = ... um eine Variable hinzuzufügen");
					return;
				}
                ersetzeVariable(parsedBefehle,env,var,i);
			} else if( getenv(var) != NULL ){
				parsedBefehle[i] = getenv(var);
			} else{
				printf("Variable: %s konnte nicht aufgelöst werden.", var);
			}
			
			printf("AUFGELÖST: %s\n", parsedBefehle[i]);
			return;
		}

    }
}



//liest befehlLine und teilt die Befehle auf.
void parseBefehle(char *befehlLine, char **parsedBefehle, char *env){
    //entfernt die Leerzeichen von der Eingabe und gliedert die Befehle in Arrays.
    aufteilen(befehlLine, parsedBefehle, " ");	

    //variablen auflösen, wenn $ enthalten.
    varAufloesen(parsedBefehle, env);
    //printArray(parsedBefehle);
	
}

/*findCommandType gibt 0 aus, wenn es ein builtin command ist
und gibt 1 aus, wenn es ein einfacher command ist. */
int findCommandType(char* befehlLine, char** parsedBefehle, char* env) { 

	//1.) parse die Commands
	parseBefehle(befehlLine, parsedBefehle, env);

	//2.) finde CommandType
    //0: builtin
    //1: einfacher command, wie ls
	if ( checkCommandType(parsedBefehle, env) ){
		return 0;
	} else{
		strtok( parsedBefehle[ strlen(*parsedBefehle)-1 ], "\n" );
		return 1;
	}
}

// Function where the system command is executed 
void execCmds(char** parsedBefehle) { 
	// Forking a child 
	pid_t pid = fork(); 

	//fügt NULL am Ende des Arrays ein, damit execvp ausgeführt werden kann.
	parsedBefehle[ strlen(*parsedBefehle) ] = NULL;

	switch(pid){
		case -1:
			printf("\nKind-Prozess konnte nicht erstellt werden: %d\n", errno); 
			return;

		case 0:
			if (execvp(parsedBefehle[0], parsedBefehle) < 0) { 
				printf("\nDer Command kann nicht ausgeführt werden..."); 
			} 
			break;
			//exit(0); //beende den Kind-Prozess.
		
		default:
			//warte auf den Kind-Prozess bis es terminiert.
			//wenn waitpid nicht 0 liefert dann ist ein Fehler aufgetreten.
			if (waitpid (pid, NULL, WUNTRACED) == 0) {
          		perror("waitpid()");
          		return;
       		}
			break;
	}
} 