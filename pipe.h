int parsePipe(char* befehlLine, char** pipedBefehle){
    //die Befehlszeile wird in ZWEI Stücke geteilt.
    //     0     |     1
    //echo hello | echo bye
    for( int i=0; i<3; i++ ){
        pipedBefehle[i] = strsep(&befehlLine, "|");

        if( pipedBefehle[i] == NULL ){
            break;
        }
    }

    //Fehlerbehandlung von leeren Pipes!
    if( strlen(pipedBefehle[0]) < 2 || strlen(pipedBefehle[1]) < 2 ){
        printf("Das ist kein gültiger Befehl!\n");
        return -1;
    }

    //Fehlerbehandlung bei mehrstufigen Pipes:
    if(pipedBefehle[2] != NULL){
        printf("Nur einstufige Pipes möglich!\n");
        return -1;
    }

    if( pipedBefehle[1] == NULL ){
        //printf("keinen Pipe-Befehl gefunden!\n");
        return 0;  //keinen pipe befehl gefunden
    } 

    else{
        //printf("Pipe-Befehl gefunden!\n");
        return 1;
    }
}

void execPipe(char **parsedBefehle, char **parsedPipeBefehle){
	int pipefd[2];
	pid_t p1, p2;

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

    //1. Prozess wird ausgeführt
	if( p1 == 0 ){
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
		
        if ( execvp(parsedBefehle[0], parsedBefehle) < 0 ) { 
			printf("Der 1. Command kann nicht ausgeführt werden...\n"); 
		}  

        close(pipefd[1]);
	}

    //Eltern-Prozess ausführen
    p2 = fork(); // Kind-Prozess für zweiten Befehl

    if( p2 < 0 ){
        printf("p2 konnte nicht Kind-Prozess erzeugen!\n");
        return;
    }

	//2. Prozess wird ausgeführt
    if( p2 == 0 ){
        //Hier wird nur gelesen, deswegen wird Schreibdeskiptor geschlossen!
        close( pipefd[1] );

        //STDIN: Standard-Eingabe
        if( dup2( pipefd[0], STDIN_FILENO ) < 0){
            printf("p2-> dup2 lieferte einen Fehler!\n");
            return;
        }

        if ( execvp(parsedPipeBefehle[0], parsedPipeBefehle) < 0 ) { 
            printf("Der 2. Command kann nicht ausgeführt werden...\n"); 
        } 

        close(pipefd[0]);
    }

    // close parent's pipes
    close(pipefd[0]);
    close(pipefd[1]);

    // warte auf die Prozesse bis sie beendet wurden
    if (waitpid (p1, NULL, WUNTRACED) == 0) { 
        perror("waitpid()");
        return;
    }

    if (waitpid (p2, NULL, WUNTRACED) == 0) { 
        perror("waitpid()");
        return;
    }
}