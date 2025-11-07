#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Gruppo: Michele Valiati & Filippo Mancini
// Progetto Sartoria

#define MAXSTRING 30
#define MAXTESSUTI 100


struct rotolo{
	char tipo_tessuto[MAXSTRING];
	char colore[MAXSTRING];
	char fantasia[MAXSTRING];
	float lunghezza;
	float larghezza;
	char codice_fornitura[MAXSTRING];
	float costo;
	float usura;
};

struct data{
	int g;
	int m;
	int a;
};

struct scheda{
	char codice_rotolo[MAXSTRING];
	char fornitore[MAXSTRING];
	struct rotolo rot;
	struct data data_acquisto;
	float quantita_disponibile;
	float utilizzo_previsto;
} inventario[MAXTESSUTI];



int main(){
	
	return 0;
}