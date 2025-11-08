// Gruppo: Michele Valiati & Filippo Mancini
// Su questo progetto abbiamo collaborato usando Live Share e GitHub
// Progetto Sartoria

// Librerie
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "sart.h"		// Libreria personale, contiene checkData,checkVal,pausa

// Constanti
// Lunghezza massima di una stringa
#define MAXSTRING 30
// Numero massimo di tessuti
#define MAXTESSUTI 100
// Numero massimo di progetti
#define MAXPROGETTI 20
// Budget iniziale della sartoria
#define BUDGETINIZIALE 1000
// Nome del file di salvataggio
#define FILEINVENTARIO "Inventario.txt"
#define FILEPROGETTI "Progetti.txt" 

//File
// File usato per salvare l'inventario
FILE *Finv;
// File usato per salvare i progetti
FILE *FProg;

// Variabili Globali
// Budget sartoria
float budget = BUDGETINIZIALE;

// Structs
// Struttura per i rotoli di tessuto
struct rotolo{
	char tipo_tessuto[MAXSTRING]; // Tipo (cotone, lino ,seta, lana, ecc...)
	char colore[MAXSTRING];	// Colore (rosso, verde, blu, ecc...);
	char fantasia[MAXSTRING]; // Fantasia (nulla, righe, quadretti, ecc...)
	float lunghezza; // In M
	float larghezza; // In cm
	// Ogni volta che verra' riacquistato sarà aggiunta la stessa dimensione del rotolo, non quella che serve
	char codice_fornitura[MAXSTRING]; // Codice fornitore 
	float costo; // Costo del rotolo intero 
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
	int scarti_utilizzabili;		// Scarti utilizzabili (>50cm);
} inventario[MAXTESSUTI];			// Array globale dei tessuti

// Struttura per i progetti
struct progetto{
	char nome_progetto[MAXSTRING];		// Nome
	char rotolo_richiesto[MAXSTRING];	// Codice rotolo richiesto
	float metraggio_richiesto;			// Metraggio richiesto
	float costo_approssimato;			// Costo approssimato (ottenuto da calcolaCostoProgetto() )
	int mini;							// Variabile che stabilisce se e' un mini progetto (cravatta), ovvero che si crea utilizzando degli scarti
	int scarti_richiesti;				// Scarti richiesti (se mini=1)
} progetti[MAXPROGETTI];				// Array globale dei progetti


// FUNZIONI

int menu();							// Stampa il menu
int nuovoRotolo(int*);				// Aggiunge un nuovo rotolo
int modificaRotolo(int);			// Modifica un rotolo esistente
int eliminaRotolo(int*);			// Elimina un rotolo (azzera la scheda e diminusce la dimensione logica)
int nuovoProgetto(int*);			// Aggiunge un nuovo progetto
int modificaProgetto(int);			// Modifica un progetto
int eliminaProgetto(int*);			// Elimina un progetto
float calcolaCostoProgetto(int);	// Calcola il costo progetto (Ogni volta che il rotolo finisce lo riacquista)
void mostraProgetti(int);			// Stampa i progetti
int avviaTaglio(int*);				// Avvia il taglio (rimuove i progetti in attesa)
void mostraTessuti(int);			// Mostra i tessuti
int controlloTessuti(int);			// Controlla i tessuti con usura troppo alta e ne propone la sostituzione
int rotazioneScorte(int);			// Ruota le scorte
// Should
float aumentoUsura();				// Aumenta l'usura (a ogni azione o a caso. ancora da decidere...)
void salvaInventario(int,int);		// Sotto forma di file .txt
void caricaInventario(int*,int*);	// e salva sia tessuti che progetti


// Main
int main(){
	return 0;
}
/*
Funzione che stampa il menu:
 1) Gestione rotoli
	1.1) Nuovo rotolo
	1.2) Modifica rotolo
	1.3) Elimina rotolo
 2) Gestione progetti
	2.1) Nuovo progetto
	2.2) Modifica progetto
	2.3) Elimina progetto
	2.4) Mostra progetti
	2.5) Avvia taglio
 3) Gestione inventario
	3.1) Mostra tessuti
	3.2) Controlla i tessuti
	3.3) Rotazione Scorta
	3.4) Salvataggio
 4) Uscita
	4.1) Termina il programma
*/
int menu(){
	int s1,s2;
	
	return s1*10 + s2;
}