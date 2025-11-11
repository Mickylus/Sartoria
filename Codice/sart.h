// Libreria che contiene funzioni minori
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

// Librerie per la funzione pausa()
#ifdef _WIN32
    #include <conio.h>   // per getch() su Windows
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
void pausa(const char *messaggio) {
    printf("%s", messaggio);
    fflush(stdout);  // forza la stampa immediata del messaggio
    #ifdef _WIN32
        getch();  // attende un tasto senza Invio (Windows)
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