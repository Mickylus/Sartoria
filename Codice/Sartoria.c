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

// pulizia schermo compatibile con i 2 sistemi operativi
#ifdef _WIN32
	#define CLEAR "cls"		// Compatibile con Windows
#else
	#define CLEAR "clear"	// Compatibile con Linux e MacOS
#endif

//File
// File usato per salvare l'inventario
FILE *FInv;
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
	int RCount=0,PCount=0,scelta,err;
	char val[10],filter[MAXSTRING];
	do{
		scelta=menu();
		switch(scelta){
			case 41:
				salvaInventario(RCount,PCount);				// Salvo il programma
				printf("Uscita in corso...\n");				// Termino il programma
				break;
			default:
				printf("Si e' verificato un'errore!\n");	// In caso di qualche bug durante la scelta
				break;
		}
		if(scelta != 41){
			system(CLEAR);		// Pulisco lo schermo
		}
	}while(scelta!=41);
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
	char val[10];
	do{
		printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
		printf(" Menu Sartoria      |  Budget: %.2f euro\n",budget);
		printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
		printf("\t1) Gestione rotoli\n");
		printf("\t2) Gestione progetti\n");
		printf("\t3) Gestione inventario\n");
		printf("\t4) Esci\n");
		printf("Scelta: ");
		scanf(" %s",val);
		s1=checkVal(val);
		if(s1<=0 || s1 > 4){
			printf("ERRRORE: Scelta non valida!!\n");
			pausa("Continua...\n");
			system(CLEAR);
		}
	}while(s1<=0 || s1 > 4);
	system(CLEAR);
	do{
		switch(s1){
			case 1:
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
				printf(" Sartoria: Rotoli   |  Budget: %.2f euro\n",budget);
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
				printf("\t1) Nuovo rotolo\n");
				printf("\t2) Modifica rotolo\n");
				printf("\t3) Elimina rotolo\n");
				break;
			case 2:
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
				printf(" Sartoria: Progetti |  Budget: %.2f euro\n",budget);
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
				printf("\t1) Nuovo progetto\n");
				printf("\t2) Modifica progetto\n");
				printf("\t3) Elimina progetto\n");
				printf("\t4) Elenco progetti\n");
				printf("\t5) Avvia taglio\n");
				break;
			case 3:
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
				printf(" Sartoria: Inventario| Budget: %.2f euro\n",budget);
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
				printf("\t1) Elenco tessuti\n");
				printf("\t2) Controllo tessuti\n");
				printf("\t3) Rotazione scorte\n");
				printf("\t4) Salvataggio manuale\n");
				break;
			case 4:
				return 41;
		}
		printf("Scelta: ");
		scanf(" %s",val);
		s2=checkVal(val);
		if((s1==1 && (s2 <=0 || s2 > 3)) || (s1==2 && (s2<=0 || s2 > 5)) || (s1==3 && (s2 <= 0 || s2 >4))){
			printf("ERRORE: Scelta non valida!!\n");
			pausa("Continua...\n");
			system(CLEAR);
		}
	}while((s1==1 && (s2 <=0 || s2 > 3)) || (s1==2 && (s2<=0 || s2 > 5)) || (s1==3 && (s2 <= 0 || s2 >4)));
	system(CLEAR);
	return s1*10 + s2;
}