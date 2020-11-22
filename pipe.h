int parsePipe(char* befehlLine, char** pipedBefehle){
    printf("hello from parsePipe\n");

    printf("befehlLine: %s\n", befehlLine);

    //die Befehlszeile wird in ZWEI Stücke geteilt.
    //     0     |     1
    //echo hello | echo bye
    for( int i=0; i<2; i++ ){
        pipedBefehle[i] = strsep(&befehlLine, "|");

        if( pipedBefehle[i] == NULL ){
            break;
        }
    }

    //geparsed:
    printf("pipedBefehle[0]: %s\n", pipedBefehle[0]);
    printf("pipedBefehle[1]: %s\n", pipedBefehle[1]);

    //TODO: Fehlerbehandlung bei mehrstufigen Pipes!!!!

    if( pipedBefehle[1] == NULL ){
        printf("keinen Pipe-Befehl gefunden!\n");
        return 0;  //keinen pipe befehl gefunden
    } 

    else{
        printf("Pipe-Befehl gefunden!\n");
        return 1;
    }
}

void execPipe(char **parsedBefehle, char **parsedPipeBefehle){
	printf("execPipe\n");
	
	//0 ist das LeseENDE und 1 ist das SchreibENDE
	int pipefd[2];
	pid_t p1, p2;

    printArray(parsedBefehle);

    /*
        pipefd[1] referenziert Datei können Daten geschrieben 
        pipefd[0] referenziert Datei können Daten gelesen
    */

	if( pipe(pipefd) < 0 ){
		printf("Pipe konnte nicht initialisiert werden!\n");
		return;
	}

	p1 = fork(); //Kind-Prozess erstellen

	if( p1 < 0 ){
		printf("p1 konnte nicht Kind-Prozess erzeugen!\n");
		return;
	}

    //p2 muss auf p1 warten?

    printf("p1: %d\n", p1);

    //1. Prozess wird ausgeführt
	if( p1 == 0 ){
        printf("Prozess 1 wird ausgeführt!\n");
        
        
        /*Der jeweils nicht verwendete Deskriptor (beim lesenden Prozess ist dies also
        fdes[1], beim schreibenden fdes[0]) sollte jeweils mit close() geschlossen werden, da über
        das Schließen der Dateien die Terminierung des Prozesses durch den jeweils anderen Prozesses
        erkannt wird. */
        
        //Hier wird geschrieben, deswegen wird Lesedeskriptor geschlossen!
        close( pipefd[0] );

        /*
            Findet ein exec<l|v>[e|p] Systemaufruf statt, so werden normalerweise alle offenen Dateien
            des Prozesses vorher geschlossen (close-on-exec), denn exec lädt einen Programm-Code in einen
            laufenden Prozess. Ansonsten könnten Konflikte speziell beim schreibenden Zugriff auf Dateien
            entstehen. deswegen => DUP
        */

        //verbindet Filedeskriptor 1 mit dem 0
        //STDOUT Standard-Ausgabe
        if( dup2( pipefd[1], STDOUT_FILENO ) < 0){
            printf("p1-> dup2 lieferte einen Fehler!\n");
            return;
        }

        //Hier war der Fehler Ausgabe von printArray wurde genommen! :D
        //printArray(parsedBefehle);
		
        if ( execvp(parsedBefehle[0], parsedBefehle) < 0 ) { 
			printf("Der 1. Command kann nicht ausgeführt werden...\n"); 
		}  

        close(pipefd[1]);
	}

    printf("Prozess 1 wurde beendet!\n");
    //Eltern-Prozess ausführen
    p2 = fork(); // Kind-Prozess für zweiten Befehl

    if( p2 < 0 ){
        printf("p2 konnte nicht Kind-Prozess erzeugen!\n");
        return;
    }

    printf("p2: %d\n", p2);

    //waitpid (p1, NULL, WUNTRACED) != 0
	//1. Prozess wird ausgeführt
    if( p2 == 0 ){
        printf("Prozess 2 wird ausgeführt...\n");

        //Hier wird nur gelesen, deswegen wird Schreibdeskiptor geschlossen!
        close( pipefd[1] );
        
        //strtok(parsedPipeBefehle[0], "\n");
        //strtok(parsedPipeBefehle[1], "\n");

        //STDIN: Standard-Eingabe
        if( dup2( pipefd[0], STDIN_FILENO ) < 0){
            printf("p2-> dup2 lieferte einen Fehler!\n");
            return;
        }

        //printArray(parsedPipeBefehle);

        if ( execvp(parsedPipeBefehle[0], parsedPipeBefehle) < 0 ) { 
            printf("Der 2. Command kann nicht ausgeführt werden...\n"); 
        } 

        close(pipefd[0]);
        //Prozess 2 wird hier terminiert!
        printf("ENDE von p2!\n");
    }

    // close parent's pipes
    close(pipefd[0]);
    close(pipefd[1]);

    // wait for the more process to finish
    //waitpid(p1, NULL, WUNTRACED);
    if (waitpid (p1, NULL, WUNTRACED) == 0) { 
        perror("waitpid()");
        return;
    }

    //waitpid(p2, NULL, WUNTRACED);

    if (waitpid (p2, NULL, WUNTRACED) == 0) { 
        perror("waitpid()");
        return;
    }
}