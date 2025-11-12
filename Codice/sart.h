// Libreria che contiene funzioni minori
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

// Librerie per la funzione pausa()
#ifdef _WIN32
    #include <conio.h>   // per getch() su Windows
	#include <windows.h>
#else
    #include <termios.h> // per impostazioni terminale su Linux/macOS
    #include <unistd.h>  // per STDIN_FILENO
#endif

/*
Controlla se la data e' valida.
0 = Valida
1 = Non Valida
*/
int checkData(int g,int m, int a){
	if(g<=0 || g >31){
		return 1;
	}
	if((a % 4 == 0 && (a % 100 != 0 || a % 400 == 0)) && m == 2){
		if(g>29){
			return 1;
		}
	}else if(m ==2 && g>28){
		return 1;
	}
	if((m == 4 && g > 30) || (m == 6 && g > 30) || (m == 9 && g > 30) || (m == 11 && g > 30)){
		return 1;
	}
	if(m<=0 || m>12){
		return 1;
	}
	if(a<1900 || a>2100){
		return 1;
	}
	return 0;
}
/*
Stampa il messaggio e attende un input (qualunque tasto).

*/
int pausa(const char *messaggio) {
    printf("%s", messaggio);
    fflush(stdout);  // forza la stampa immediata del messaggio
    #ifdef _WIN32
        return getch();  // attende un tasto senza Invio (Windows)
    #else
        struct termios oldt, newt;
        int ch;
        // Legge le impostazioni correnti del terminale
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        // Disabilita modalità canonica ed echo
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar(); // legge un singolo carattere
        // Ripristina le impostazioni originali
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        (void)ch; // evita warning "unused variable"
    #endif
}
/*
Funzione che controlla che il valore intero inserito non contenga lettere
-1 = Lettere trovate
num = Numero convertito
*/
int checkValInt(char valore[]){
	int num;
	num=atoi(valore);
	if(num==0){
		return -1;
	}
	return num;
}
/*
Funzione che controlla che il valore float inserito non contenga lettere
-1 = Lettere trovate
num = Numero convertito
*/
float checkValFloat(char valore[]){
	float num;
	num=atof(valore);
	if(num==0){
		return -1;
	}
	return num;
}
/*
| Valore | Colore testo     |
| ------ | ---------------- |
| 0      | Nero             |
| 1      | Blu              |
| 2      | Verde            |
| 3      | Azzurro          |
| 4      | Rosso            |
| 5      | Viola            |
| 6      | Giallo scuro     |
| 7      | Bianco (default) |
| 8      | Grigio           |
| 9      | Blu chiaro       |
| 10     | Verde chiaro     |
| 11     | Azzurro chiaro   |
| 12     | Rosso chiaro     |
| 13     | Magenta          |
| 14     | Giallo           |
| 15     | Bianco brillante |
*/
void co(int colore){
	#ifdef _WIN32
    	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    	SetConsoleTextAttribute(hConsole, colore);
	#else
		// --- macOS / Linux ---
    	switch (colore) {
        	case 0:
				printf("\033[30m");
				break; // Nero
        	case 1:
				printf("\033[34m");
				break; // Blu
        	case 2:
				printf("\033[32m");
				break; // Verde
        	case 3:
				printf("\033[36m");
				break; // Azzurro
        	case 4:
				printf("\033[31m");
				break; // Rosso
			case 5:
				printf("\033[35m");
				break; // Viola
			case 6:
				printf("\033[33m");
				break; // Giallo scuro
			case 7:
				printf("\033[37m");
				break; // Bianco
			case 8:
				printf("\033[90m");
				break; // Grigio
			case 9:
				printf("\033[94m");
				break; // Blu chiaro
			case 10:
				printf("\033[92m");
				break; // Verde chiaro
			case 11:
				printf("\033[96m");
				break; // Azzurro chiaro
			case 12:
				printf("\033[91m");
				break; // Rosso chiaro
			case 13:
				printf("\033[95m");
				break; // Magenta
			case 14:
				printf("\033[93m"); 
				break; // Giallo chiaro
			case 15:
				printf("\033[97m");
				break; // Bianco brillante
			default:	
				printf("\033[0m");
				break;  // Reset
    	}
    	fflush(stdout);
	#endif
}