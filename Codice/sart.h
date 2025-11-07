#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

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
    _getch();
}
/*
Funzione che controlla che il valore inserito non contenga lettere
-1 = Lettere trovate
num = Numero convertito
*/
int checkVal(char valore[]){
	int num;
	num=atoi(valore);
	if(num==0){
		return -1;
	}
	return num;
}