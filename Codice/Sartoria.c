// Gruppo: Michele Valiati & Filippo Mancini
// Su questo progetto abbiamo collaborato usando Live Share e GitHub
// Progetto Sartoria

// Librerie
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "sart.h"		// Libreria personale, contiene checkData e pausa

// Constanti
// Lunghezza massima di una stringa
#define MAXSTRING 30
// Numero massimo di tessuti
#define MAXTESSUTI 100
// Numero massimo di progetti
#define MAXPROGETTI 20

// Structs
// Struttura per i rotoli di tessuto
struct rotolo{
	char tipo_tessuto[MAXSTRING]; // Tipo (cotone, lino ,seta, lana, ecc...)
	char colore[MAXSTRING];	// Colore (rosso, verde, blu, ecc...);
	char fantasia[MAXSTRING]; // Fantasia (nulla, righe, quadretti, ecc...)
	float lunghezza; // In M
	float larghezza; // In cm
	char codice_fornitura[MAXSTRING]; // Codice fornitore 
	float costo; // Costo per M^2
	float usura; // In %
};

// Struttura per la data
struct data{
	int g;	// Giorno
	int m;	// Mese
	int a;	// Anno
};

// Struttura per le schede dei tessuti
struct scheda{
	char codice_rotolo[MAXSTRING]; 	// Codice prodotto
	char fornitore[MAXSTRING];		// Fornitore
	struct rotolo rot;				// Rotolo
	struct data data_acquisto;		// Data di acquisto (FIFO)
	float quantita_disponibile;		// Quantita' disponibile
	float utilizzo_previsto;		// Utilizzo previsto
} inventario[MAXTESSUTI];			// Array globale dei tessuti

// Struttura per i progetti
struct progetto{
	char nome_progetto[MAXSTRING];		// Nome
	char rotolo_richiesto[MAXSTRING];	// Codice rotolo richiesto
	float metraggio_richiesto;			// Metraggio richiesto
	float costo_approssimato;			// Costo approssimato (ottenuto da calcolaCostoProgetto() )
} progetti[MAXPROGETTI];				// Array globale dei progetti


// FUNZIONI
// Must
int menu();
int nuovoRotolo(int*);
int modificaRotolo(int);
int eliminaRotolo(int*);
int nuovoProgetto(int*);
float calcolaCostoProgetto(int);
int avviaTaglio(int*);
void controllo(int);
int rotazioneScorte(int);
// Should
float aumentoUsura();
void salvaInventario(int,int);		// Sotto forma di file txt | bin | json
void caricaInventario(int*,int*);	// e salva sia tessuti che progetti


// Main
int main(){	
	return 0;
}