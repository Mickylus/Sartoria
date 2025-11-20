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
// Massimo di rotoli per ogni progetto
#define MAXP 10
// Costante degli scarti
#define SCARTI 0.24
// Usura massima rotoli
#define MAXUSURA 40
// Numero massimo di preset
#define MAXPRESET 100
// Nome del file di salvataggio
#define FILEINVENTARIO "Inventario.txt"
#define FILEPROGETTI "Progetti.txt"
#define FILEPRESET "Preset.txt"

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
// File usato per salvare i preset
FILE *Preset;

// Variabili Globali
// Budget sartoria
float budget = BUDGETINIZIALE;
// Array preset
char ArrayPreset[MAXPRESET][50];

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

// Struttura per le schede dei tessuti
struct scheda{
	char codice_rotolo[MAXSTRING]; 	// Codice prodotto
	char fornitore[MAXSTRING];		// Fornitore
	struct rotolo rot;				// Rotolo
	int g,m,a;						// Data di acquisto (FIFO)
	float quantita_disponibile;		// Quantita' disponibile
	float utilizzo_previsto;		// Utilizzo previsto
	int scarti_utilizzabili;		// Scarti utilizzabili (>50cm);
} inventario[MAXTESSUTI];			// Array globale dei tessuti

struct rotoli{
	char rotolo_richiesto[MAXSTRING];
	float quantita_richiesta;
};

// Struttura per i progetti
struct progetto{
	char nome_progetto[MAXSTRING];		// Nome
	struct rotoli rotoli_richiesti[MAXP];// Rotoli richiesti
	float costo_approssimato;			// Costo approssimato (ottenuto da calcolaCostoProgetto() )
	int mini;							// Variabile che stabilisce se e' un mini progetto (cravatta), ovvero che si crea utilizzando degli scarti
	int scarti_richiesti;				// Scarti richiesti (se mini=1)
	char tipoCapo[MAXSTRING];			// Il tipo di vestito ("Gonna", "Giacca")
	int rdim;							// Numero di rotoli richiesti
	float paga;							// Paga del progetto
	float ricavi;						// Ricavi del progetto
	float valore;						// Valore del progetto
} progetti[MAXPROGETTI];				// Array globale dei progetti


// FUNZIONI

int menu(int);							// Stampa il menu
int nuovoRotolo(int*);				// Aggiunge un nuovo rotolo
int modificaRotolo(int,char[]);			// Modifica un rotolo esistente
int eliminaRotolo(int*,char[]);			// Elimina un rotolo (azzera la scheda e diminusce la dimensione logica)
int nuovoProgetto(int*,int);			// Aggiunge un nuovo progetto
int modificaProgetto(int,char[],int);			// Modifica un progetto
int eliminaProgetto(int*,char[]);			// Elimina un progetto
float calcolaCostoProgetto(int,int);	// Calcola il costo progetto (Ogni volta che il rotolo finisce lo riacquista)
int mostraProgetti(int*,int);			// Stampa i progetti
int avviaTaglio(int*,char[],int);				// Avvia il taglio (rimuove i progetti in attesa)
int mostraTessuti(int);			// Mostra i tessuti
int controlloTessuti(int);			// Controlla i tessuti con usura troppo alta e ne propone la sostituzione
int rotazioneScorte(int);			// Ruota le scorte
// Should
float aumentoUsura(float);				// Aumenta l'usura (a ogni azione o a caso. ancora da decidere...)
void salvaInventario(int,int,int);		// Sotto forma di file .txt
void caricaInventario(int*,int*,int*);	// e salva sia tessuti che progetti
void reset(int*,int*);				// reset inventario
void aggiorna(int,int);				// aggiorna i dati
int assegnaScarti(float);			// assegna gli scarti del progetto in base al taglio effettuato
void riacquista(int);				// ricompra il rotolo
void nuovoPreset(int,int,int*);		// Crea un nuovo preset
void caricaPreset(int*,int*,char[]);// Carica un preset
int mostraPreset(int*,int*,int*);	// Mostra i preset
int elimiaPreset(int*,char[]);		// Elimina un preset

// Main
int main(){
	int RCount=0,PCount=0,scelta,err,tasto,presetCount=0;
	char val[10],filter[MAXSTRING];
	caricaInventario(&RCount,&PCount,&presetCount);
	system(CLEAR);
	srand(time(NULL));	// Inizializzo la generazione di numeri casuali
	do{
		scelta=menu(1);
		switch(scelta){
			case 11:
				err=nuovoRotolo(&RCount);											// Input nuovo rotolo
				if(err==1){
					errore("Dimensione massima inventario raggiunta!\n");
				}
				break;
			case 12:
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
				printf(" Menu Sartoria      |  Budget: %.2f euro\n",budget);
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
				printf("Inserisci il codice da cercare: ");							// Input codice da cercare
				scanf(" %s",filter);
				if(modificaRotolo(RCount,filter)==1){								// procedo con la modifica
					errore("Rotolo non trovato!\n");
					pausa("Continua...\n");
				}
				break;
			case 13:
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
				printf(" Menu Sartoria      |  Budget: %.2f euro\n",budget);
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
				printf("Inserisci il codice da cercare: ");		// Input codice da cercare
				scanf(" %s",filter);
				if(eliminaRotolo(&RCount,filter)==1){		// Procedo con l'eliminazione
					errore("Rotolo non trovato!\n");
				}else{
					co(2);
					printf("Rotolo eliminato con successo!\n");
					co(7);
				}
				pausa("Continua...\n");
				break;
			case 21:
				if(RCount>0){
					if(nuovoProgetto(&PCount,RCount)==1){		// Input nuovo progetto
						errore("ERRORE: dimensione massima raggiunta!\n");
						pausa("\nContinua...\n");
					}
				}else{
					errore("ERRORE: Non ci sono rotoli in magazzino!\n");
					pausa("\nContinua...\n");
				}
				break;
			case 22:
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
				printf(" Menu Sartoria      |  Budget: %.2f euro\n",budget);
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
				printf("Inserisci il nome da cercare: ");		// Input nome da cercare
				scanf(" %s",filter);
				if(modificaProgetto(PCount,filter,RCount)==1){
					errore("Rotolo non trovato!\n");
					pausa("Continua...\n");
				}
				break;
			case 23:
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
				printf(" Menu Sartoria      |  Budget: %.2f euro\n",budget);
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
				printf("Inserisci il nome da cercare: ");
				scanf(" %s",filter);
				if(eliminaProgetto(&PCount,filter)==1){
					errore("Progetto non trovato!\n");
				}else{
					co(2);
					printf("Progetto eliminato con sucesso!\n");
					co(7);
				}
				pausa("Continua...\n");
				break;
			case 24:
				if(mostraProgetti(&PCount,RCount)==1){
					errore("Non ci sono progetti al momento!\n");
					pausa("\nContinua...\n");
				}
				break;
			case 25:
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
				printf(" Menu Taglio        |  Budget: %.2f euro\n",budget);
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
				printf("Inserisci il nome da cercare: ");
				scanf(" %s",filter);
				if(avviaTaglio(&PCount,filter,RCount)==1){
					errore("Qualcosa e' andato storto...\n");
				}
				pausa("\nContinua...\n");
				break;
			case 31:
				if(mostraTessuti(RCount)==1){
					errore("Non ci sono tessuti al momento!\n");
					pausa("\nContinua...\n");
				}
				break;
			case 32:
				if(controlloTessuti(RCount)==1){
					errore("Non ci sono tessuti inutilizzabili!\n");
					pausa("Continua...\n");
				}
				break;
			case 33:
				if(rotazioneScorte(RCount)==1){
					printf("I tessuti erano gia' in ordine\n");
				}else{
					printf("Tessuti ordinati\n");
				}
				pausa("Continua...\n");
				break;
			case 34:
				co(8);
				printf("Salvataggio in corso...\n");
				co(7);
				salvaInventario(RCount,PCount,presetCount);		// Salvataggio manuale
				co(2);
				printf("Salvataggio effettuato!\n");
				co(7);
				pausa("Continua...\n");
				break;
			case 35:
				int i=0;
				do{
					system(CLEAR);
					printf("Sei sicuro di voler resetare?\n");
					if(i%2==0){
						co(7);
						printf("[CONFERMA] ");
						co(8);
						printf("[ANNULLA]");
						co(7);
					}else{
						co(8);
						printf("[CONFERMA] ");
						co(7);
						printf("[ANNULLA]");
					}
					tasto=pausa(" ");
					if(tasto!=13){
						i++;
					}
				}while(tasto!=13);
				printf("\n");
				if(i%2==0){
					caricamento("Reset in corso",1);
					reset(&RCount,&PCount);											// Resetto il programma
					co(2);
					printf("Reset effettuato!\n");
					co(7);
				}else{
					errore("Reset annullato\n");
				}
				pausa("\nContinua...\n");
				break;
			case 41:
				nuovoPreset(RCount,PCount,&presetCount);
				pausa("\nContinua...\n");
				break;
			case 42:
				if(mostraPreset(&presetCount,&RCount,&PCount)==1){
					errore("Non sono presenti preset!\n");
					pausa("\nContinua...\n");
				}
				break;
			case 51:
				salvaInventario(RCount,PCount,presetCount);				// Salvo il programma
				printf("Uscita in corso...\n");				// Termino il programma
				break;
			default:
				errore("Funzione non ancora disponibile!\n");	// In caso di qualche bug durante la scelta
				pausa("\nContinua...\n");
				break;
		}
		aggiorna(RCount,PCount);
		if(scelta != 51){
			system(CLEAR);		// Pulisco lo schermo
		}
		co(7);
	}while(scelta!=51);
	return 0;
}
// Funzione che elimina un preset
int eliminaPreset(int *PresetCount, char preset[]){
    int i,j;
    for(i=0;i<*PresetCount;i++){
        if(strcmp(ArrayPreset[i], preset) == 0){
            /* shift left */
            for(j=i;j<*PresetCount-1;j++){
                strcpy(ArrayPreset[j], ArrayPreset[j+1]);
            }
            (*PresetCount)--;
            /* remove file */
            if(remove(preset) != 0) {
                return 1; // errore
            }
            /* riscrivi FILEPRESET */
            Preset = fopen(FILEPRESET, "w");
            if(Preset == NULL) return 1;
            fprintf(Preset, "%d\n", *PresetCount);
            for(j=0;j<*PresetCount;j++){
                fprintf(Preset, "%s\n", ArrayPreset[j]);
            }
            fclose(Preset);
            return 0; // successo
        }
    }
    return 1; // non trovato
}
/*
Funzion che carica un preset
*/
void caricaPreset(int *RCount, int *PCount, char percorso_preset[]){
	int i,j;
	Preset=fopen(percorso_preset,"r");
	if(Preset==NULL){
		co(4);
		printf("Si e' verificato un'errore nell'apertura del file '%s'.\n",percorso_preset);
		co(7);
	}else{
		// leggo i tessuti
		fscanf(Preset,"%d %f",RCount,&budget);
		for(i=0;i<*RCount;i++){
			fscanf(Preset,"%s %s %s %s %s %f %f %s %f %f %d %d %d %f %f %d",inventario[i].codice_rotolo,inventario[i].fornitore,inventario[i].rot.tipo_tessuto,inventario[i].rot.colore,inventario[i].rot.fantasia,&inventario[i].rot.lunghezza,&inventario[i].rot.larghezza,inventario[i].rot.codice_fornitura,&inventario[i].rot.costo,&inventario[i].rot.usura,&inventario[i].g,&inventario[i].m,&inventario[i].a,&inventario[i].quantita_disponibile,&inventario[i].utilizzo_previsto,&inventario[i].scarti_utilizzabili);
		}
		// leggo i progetti
		fscanf(Preset,"%d",PCount);
		for(i=0;i<*PCount;i++){
			fscanf(Preset,"%s %f %d %f %s %d %f %f %f",progetti[i].nome_progetto,&progetti[i].costo_approssimato,&progetti[i].mini,&progetti[i].scarti_richiesti,progetti[i].tipoCapo,&progetti[i].rdim,&progetti[i].paga,&progetti[i].ricavi,&progetti[i].valore);
			for(j=0;j<progetti[i].rdim;j++){
				fscanf(Preset,"%s %f",progetti[i].rotoli_richiesti[j].rotolo_richiesto,&progetti[i].rotoli_richiesti[j].quantita_richiesta);
			}
		}
		co(2);
		printf("Preset caricato con successo...\n");
		co(7);
	}
	pausa("Continua...\n");
}
/*
Mostra i preset
1= Nessun preset esistente
*/
int mostraPreset(int *PresetCount, int*RCount, int *PCount){
	int i,j,f=1,tasto=0;
	for(i=0;i<*PresetCount;i++){
		f=0;
		do{
			co(7);
			system(CLEAR);
			printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
			printf("                 Menu Preset\n");
			printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
			printf("[SU/GIU] Muoviti | [INVIO] Carica | [SPAZIO] Elimina | [ESC] Esci\n");
			for(j=0;j<*PresetCount;j++){
				if(j==i){
					co(15);
				}else{
					co(8);
				}
				printf("\t%s\n",ArrayPreset[j]);
			}
			tasto=pausa("\n");
			if(tasto==27){
				return 0;
			}
			if(tasto==13){
				caricaPreset(RCount,PCount,ArrayPreset[i]);
				return 0;
			}
			if(tasto==32){
				// Da fixare
				if(eliminaPreset(PresetCount,ArrayPreset[i])==1){
					errore("\nERRORE: Impossibile eliminare il salvataggio!\n");
				}else{
					co(2);
					printf("\nIl salvataggio e' stato eliminato!\n");
					co(7);
				}
				pausa("\nContinua...\n");
				if(*PresetCount==0){
					return 0;
				}
				if(i>=*PresetCount){
					i=*PresetCount-1;;
				}
			}
			if(tasto==1001){
				if(i>=*PresetCount-1){
					i--;
				}
			}
			if(tasto==1000){
				if(i<=0){
					i--;
				}else{
					i-=2;
				}
			}
		}while(tasto!=1000 && tasto!=1001 && tasto!=13);
	}
	return f;
}
/*
Funzione che crea un nuovo preset
*/
void nuovoPreset(int RCount, int PCount,int *PresetCount){
	int i,j;
	char preset[20],percorso[50];
	if(*PresetCount>=MAXPRESET){
		errore("Numero massimo di preset raggiunto\n");
	}else{
		printf("Inserisci il nome del preset: ");
		scanf(" %s",preset);
		// genero il nome e percorso del file
		strcpy(percorso,"Presets/");
		strcat(preset,".txt");
		strcat(percorso,preset);
		// Lo salvo
		Preset=fopen(FILEPRESET,"w");
		(*PresetCount)++;
		strcpy(ArrayPreset[*PresetCount-1],percorso);
		fprintf(Preset,"%d\n",*PresetCount);
		for(i=0;i<*PresetCount;i++){
			fprintf(Preset,"%s\n",ArrayPreset[i]);
		}
		fclose(Preset);

		Preset=fopen(percorso,"w");
		fprintf(Preset,"%d %f\n",RCount,budget);
		for(i=0;i<RCount;i++){
			fprintf(Preset,"%s %s %s %s %s %f %f %s %f %f %d %d %d %f %f %d\n",inventario[i].codice_rotolo,inventario[i].fornitore,inventario[i].rot.tipo_tessuto,inventario[i].rot.colore,inventario[i].rot.fantasia,inventario[i].rot.lunghezza,inventario[i].rot.larghezza,inventario[i].rot.codice_fornitura,inventario[i].rot.costo,inventario[i].rot.usura,inventario[i].g,inventario[i].m,inventario[i].a,inventario[i].quantita_disponibile,inventario[i].utilizzo_previsto,inventario[i].scarti_utilizzabili);
		}
		fprintf(Preset,"%d\n",PCount);
		for(i=0;i<PCount;i++){
			fprintf(Preset,"%s %f %d %f %s %d %f %f %f\n",progetti[i].nome_progetto,progetti[i].costo_approssimato,progetti[i].mini,progetti[i].scarti_richiesti,progetti[i].tipoCapo,progetti[i].rdim,progetti[i].paga,progetti[i].ricavi,progetti[i].valore);
			for(j=0;j<progetti[i].rdim;j++){
				fprintf(FProg,"%s %f\n",progetti[i].rotoli_richiesti[j].rotolo_richiesto,progetti[i].rotoli_richiesti[j].quantita_richiesta);
			}
		}
		fclose(Preset);
		co(8);
		printf("Preset salvato in %s ...\n",percorso);
		co(7);
	}
}
/*
Funzione che ordina le scorte in base alla data
*/
int rotazioneScorte(int RCount){
	int f=1,i,j;
	struct scheda temp;
	for(i=0;i<RCount;i++){
		if(cmpData(inventario[i].g,inventario[i].m,inventario[i].a,inventario[i+1].g,inventario[i+1].m,inventario[i+1].a)==1){
			for(j=i;j<RCount-1;j++){
				temp=inventario[j];
				inventario[j]=inventario[j+1];
				inventario[j+1]=temp;
			}
			f=0;
		}
	}
	return f;
}
// Funzione che controlla quali tessuti sono andati oltre la soglia di usura
int controlloTessuti(int RCount){
	int i,tasto=0,j,k,f=1;
	for(i=0;i<RCount;i++){
		if(inventario[i].rot.usura>MAXUSURA){
			f=0;
			do{
				// Stampo tutti i campi
				system(CLEAR);
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
				printf(" Menu Sartoria      |  Budget: %.2f euro\n",budget);
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");	
				printf("Rotolo [%d/%d]\n",i+1,RCount);
				co(9);
				printf("Rotolo inutilizzabile! E' necessario ricomprarlo per utilizzarlo di nuovo!\n");
				co(8);
				printf("Codice: %s\n",inventario[i].codice_rotolo);
				printf("Fornitore: %s\n",inventario[i].fornitore);
				printf("Tipo di tessuto: %s\n",inventario[i].rot.tipo_tessuto);
				printf("Colore: %s\n",inventario[i].rot.colore);
				printf("Fantasia: %s\n",inventario[i].rot.fantasia);
				printf("Codice fornitore: %s\n",inventario[i].rot.codice_fornitura);
				printf("Data di acquisto: %d/%d/%d\n",inventario[i].g,inventario[i].m,inventario[i].a);
				printf("Quantita' disponibile: %.2f M^2\n",inventario[i].quantita_disponibile);
				printf("Utilizzo previsto: %.2f M^2\n",inventario[i].utilizzo_previsto);
				printf("Scarti: %d\n",inventario[i].scarti_utilizzabili);
				co(7);
				printf("Usura: ");
				if(inventario[i].rot.usura>MAXUSURA){
					co(4);
				}else{
					if(inventario[i].rot.usura>15){
						co(6);
					}else{
						co(7);
					}
				}
				printf("%.2f",inventario[i].rot.usura);
				co(7);
				printf(" %%\n");
				printf("Costo: %.2f euro\n",inventario[i].rot.costo);
				printf("Costo M^2: %.2f euro\n\n",inventario[i].rot.costo/((inventario[i].rot.larghezza/100)*inventario[i].rot.lunghezza));
				// Attendo un input
				tasto=pausa("[<-] [->] Muoviti | [INVIO] Ricompra | [ESC] Esci");
				// Se <- torno indietro di 1
				if(tasto==13){
					riacquista(i);
				}			
				if(tasto==1002){
					if(i>0){
						i-=2;
					}else{
						i--;
					}
				}
				// Se -> e sono al limite rimane sullo stesso tessuto
				if(tasto==1003 && i==RCount-1){
					i--;
				}
				// Esco dalla funzione
				if(tasto==27){
					return 0;
				}
			}while(tasto!=1003 && tasto != 1002 && tasto!=27);
		}
	}
	return f;
}
// Funzione che assegna l'usura in base al taglio effettuato
float aumentoUsura(float q){
	int max;
	max=(int)q*0.39;
	if(max<1){
		max=1;
	}
	return (rand()%max)+1;
}
// Funzione che ricompra il rotolo
void riacquista(int i){
	int err,g,m,a;
	char v1[100],v2[100],v3[100];
	do{
		printf("\nIserisci la data di oggi (GG MM AAAA): ");
		scanf(" %s %s %s",v1,v2,v3);
		inputData(v1,v2,v3,&g,&m,&a);
		err=checkData(g,m,a);
		if(err!=0){
			errore("ERRORE: Data non valida!\n");
		}
	}while(err!=0);
	inventario[i].g=g;
	inventario[i].m=m;
	inventario[i].a=a;
	if(inventario[i].rot.usura>MAXUSURA){
		inventario[i].quantita_disponibile=0;
		inventario[i].rot.usura=0;
	}else{
		inventario[i].rot.usura-=aumentoUsura(inventario[i].quantita_disponibile);
	}
	inventario[i].quantita_disponibile+=inventario[i].rot.lunghezza*(inventario[i].rot.larghezza/100);
	budget-=inventario[i].rot.costo;
	if(inventario[i].rot.usura<=0){
		inventario[i].rot.usura=0;
	}
}
/*
Viene avviato il taglio e rimosso il progetto
1: Errore
0: Sucesso
*/
int avviaTaglio(int *PCount, char nome[],int RCount){
	int i,j,k,f=1;
	float tot=0;
	int durata=0,g,m,a,err;
	char v1[100],v2[100],v3[100];
	for(i=0;i<*PCount;i++){
		if(strcmp(progetti[i].nome_progetto,nome)==0){
			f=0;
			progetti[i].costo_approssimato=calcolaCostoProgetto(i,RCount);
			progetti[i].ricavi=progetti[i].paga-progetti[i].costo_approssimato;
			co(8);
			do{
				printf("\nIserisci la data di oggi (GG MM AAAA): ");
				scanf(" %s %s %s",v1,v2,v3);
				inputData(v1,v2,v3,&g,&m,&a);
				err=checkData(g,m,a);
				if(err!=0){
					errore("ERRORE: Data non valida!\n");
					co(8);
				}
			}while(err!=0);
			printf("Avvio taglio in corso...\n\n");
			printf("Ricavi stimati: ");
			if(progetti[i].ricavi<0){
				co(4);
			}else{
				co(2);
			}
			printf("%.2f",progetti[i].ricavi);
			co(8);
			printf(" euro\n");
			for(j=0;j<progetti[i].rdim;j++){
				for(k=0;k<RCount;k++){
					if(strcmp(progetti[i].rotoli_richiesti[j].rotolo_richiesto,inventario[k].codice_rotolo)==0){
						if(progetti[i].mini==0){
							if(inventario[k].rot.usura<MAXUSURA){
								inventario[k].utilizzo_previsto=progetti[i].rotoli_richiesti[j].quantita_richiesta;
								tot+=progetti[i].rotoli_richiesti[j].quantita_richiesta;
								if(inventario[k].utilizzo_previsto>inventario[k].quantita_disponibile){
									inventario[k].rot.usura=aumentoUsura(inventario[k].utilizzo_previsto);
								}else{
									inventario[k].rot.usura=0;
								}
								do{
									if(inventario[k].quantita_disponibile<inventario[k].utilizzo_previsto){
										inventario[k].quantita_disponibile++;
										budget-=inventario[k].rot.costo/((inventario[k].rot.larghezza/100)*inventario[k].rot.lunghezza);
										// Aggiorno la data di acquisto
										inventario[k].g=g;
										inventario[k].m=m;
										inventario[k].a=a;
									}
								}while(inventario[k].quantita_disponibile<inventario[k].utilizzo_previsto);
								inventario[k].quantita_disponibile-=inventario[k].utilizzo_previsto;
								inventario[k].scarti_utilizzabili+=assegnaScarti(inventario[k].utilizzo_previsto);
								if(inventario[k].rot.usura>100){
									inventario[k].rot.usura=100;
								}
							}else{
								errore("ERRORE: Il tessuto e' troppo consumato per essere usato! Interrompo il progetto...\n");
								return 1;
							}
						}else{
							if(progetti[i].scarti_richiesti>inventario[k].scarti_utilizzabili){
								errore("ERRORE: Non ci sono abbastanza scarti per questo progetto!\n");
								return 1;
							}else{
								inventario[k].scarti_utilizzabili-=progetti[i].scarti_richiesti;
							}
						}
						break;
					}
				}
			}
			co(8);
			/*
			printf("\n");
			durata=tot/10;
			if(durata<1){
				durata=1;
			}
			printf("Durata stimata: %ds\n",durata);			
			caricamento("Taglio in corso ",durata);
			*/
			printf("\n");
			printf("Taglio effettuato!\n");
			budget+=progetti[i].paga;
			eliminaProgetto(PCount,progetti[i].nome_progetto);
		}
	}
	co(7);
	aggiorna(RCount,*PCount);
	return f;
}
// Assegna gli scarti in base al taglio effettuato
int assegnaScarti(float q){
	int max;
	max=(int)q * SCARTI;
	if(max<1){
		max=1;
	}
	return (rand() % max) + 1;
}
/*
Funzione che stampa a schermo i progetti
*/
int mostraProgetti(int *PCount, int RCount){
	int i,tasto=0,j,k,f=1;
	for(i=0;i<*PCount;i++){
		// Controllo se ci sono progetti
		f=0;
		do{
			system(CLEAR);
			printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
			printf(" Menu Sartoria      |  Budget: %.2f euro\n",budget);
			printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
			printf("Progetto [%d/%d]\n",i+1,*PCount);
			printf("Nome: %s\n",progetti[i].nome_progetto);
			printf("Tipo: ");
			if(progetti[i].mini==0){
				printf("Normale\n");
				printf("Capo: %s\n",progetti[i].tipoCapo);
				printf("Rotoli: ");
				for(j=0;j<progetti[i].rdim;j++){
					printf("%s, %.2f M^2 | ",progetti[i].rotoli_richiesti[j].rotolo_richiesto,progetti[i].rotoli_richiesti[j].quantita_richiesta);
				}
				printf("\n");
			}else{
				printf("Mini\n");
				printf("Capo: %s\n",progetti[i].tipoCapo);
				printf("Rotolo: %s\n",progetti[i].rotoli_richiesti[0].rotolo_richiesto);
				printf("Scarti richiesti: %d\n",progetti[i].scarti_richiesti);
			}
			printf("Valore risorse: %.2f euro\n",progetti[i].valore);
			printf("Costo approssimato: %.2f euro\n",progetti[i].costo_approssimato);
			printf("Paga: %.2f euro\n",progetti[i].paga);
			progetti[i].ricavi=progetti[i].paga-progetti[i].costo_approssimato;
			printf("Ricavi: ");
			if(progetti[i].ricavi<0){
				co(4);
			}else{
				co(2);
			}
			printf("%.2f ",progetti[i].ricavi);
			co(7);
			printf("euro\n");
			// Attendo un input
			tasto=pausa("[<-] [->] Muoviti | [SPAZIO] Modifica | [INVIO] Esegui progetto |[ESC] Esci");
			if(tasto==32){
				modificaProgetto(*PCount,progetti[i].nome_progetto,RCount);
			}
			if(tasto==13){
				printf("\n");
				avviaTaglio(PCount,progetti[i].nome_progetto,RCount);
				pausa("\nContinua...\n");
				if(*PCount==0){
					return 0;
				}
				if(i>=*PCount){
					i=*PCount-1;;
				}
			}
			if(tasto==1002){
				if(i>0){
					i-=2;
				}else{
					i--;
				}
			}
			if(tasto==1003 && i>=*PCount-1){
				i--;
			}
			if(tasto==27){
				return 0;
			}
		}while(tasto!=1003 && tasto != 1002 && tasto!=27);
	}
	return f;
}
// Funzione che aggiorna i dati
void aggiorna(int RCount,int PCount){
	int i=0,j,k;
	// Aggiorno i rotoli
	for(i=0;i<RCount;i++){
		// resetto l'utilizzo previsto
		inventario[i].utilizzo_previsto=0;
	}
	for(j=0;j<PCount;j++){
		for(k=0;k<progetti[j].rdim;k++){
			if(progetti[j].mini!=1){
				for(i=0;i<RCount;i++){
					if(strcmp(inventario[i].codice_rotolo,progetti[j].rotoli_richiesti[k].rotolo_richiesto)==0){
						inventario[i].utilizzo_previsto+=progetti[j].rotoli_richiesti[k].quantita_richiesta;
						break;
					}
				}
			}
		}
	}
	// Aggiorno i progetti
	for(i=0;i<PCount;i++){
		// Aggiorno il costo approssimato
		progetti[i].costo_approssimato=calcolaCostoProgetto(i,PCount);
		progetti[i].ricavi=progetti[i].paga-progetti[i].costo_approssimato;
	}
}
/*
Funzione che modifica un progetto
1: se non è stato trovato
0: se è stato trovato
*/
int modificaProgetto(int dim,char filtro[],int RCount){
	int i,j,tasto=0,stato=0,f=1,err=1,k;
	char val[100];
	for(i=0;i<dim;i++){
		if(strcmp(progetti[i].nome_progetto,filtro)==0){
			do{	
				system(CLEAR);
				printf("MODIFICA:\n");
				printf("(SU/GIU): Spostati, (INVIO): Seleziona, (ESC): Esci\n\n");
				if(tasto==1000){
					stato--;
				}
				if(tasto==1001){
					stato++;
				}
				if(stato<0){
					stato=0;
				}
				if(stato>4){
					stato=4;
				}
				if(stato==3 && progetti[i].mini==0 && tasto==1001){
					stato=4;
				}else if((stato==4 || stato==3) && progetti[i].mini==0 && tasto==1000){
					stato=2;
				}
				for(j=0;j<5;j++){
                    if(stato==j){	
						co(15);	// se il campo è quello scelto allora lo evidenzio
					}else{
						co(8);
					}
					switch(j){
                        case 0:
                            printf("Nome progetto: %s\n", progetti[i].nome_progetto);
                            break;
                        case 1:
                            printf("Tipo capo: %s\n", progetti[i].tipoCapo);
                            break;
                        case 2:
							if(progetti[i].mini==0){
								printf("Tessuti:...\n");
							}else{
								printf("Scarti richiesti: %d\n",progetti[i].scarti_richiesti);
							}
							break;
                        case 3:
							if(progetti[i].mini==1){
                            	printf("Tessuto: %s\n", progetti[i].rotoli_richiesti[0].rotolo_richiesto);
							}
							break;
                        case 4:
                            printf("Paga: %.2f\n", progetti[i].paga);
                            break;
					}
				}
				co(7);
				printf("- - - - - - - - - - - - - - - - - - -\n");
				tasto=pausa("");
				if(tasto==13){
					switch(stato){
						case 0:
							printf("Nuovo nome: ");
							scanf(" %s",progetti[i].nome_progetto);
							break;
						case 1:
							printf("Nuovo tipo capo: ");
							scanf(" %s",progetti[i].tipoCapo);
							break;
						case 2:
							if(progetti[i].mini==0){
								printf("Rotoli:\n");
								for(j=0;j<progetti[i].rdim;j++){
									do{
										printf("\tCodice rotolo [%d/%d]: ",j+1,progetti[i].rdim);
										scanf(" %s",progetti[i].rotoli_richiesti[j].rotolo_richiesto);
										for(k=0;k<RCount;k++){
											if(strcmp(progetti[i].rotoli_richiesti[j].rotolo_richiesto,inventario[k].codice_rotolo)==0){		// Controllo che ci sia il rotolo scelto
												err=0;
											}
										}
										if(err!=0){
											errore("\tERRORE: Rotolo non trovato!\n");
										}
									}while(err!=0);
									err=1;
									do{
										printf("\tQuantita' richiesta (M^2): ");
										scanf(" %s",val);
										progetti[i].rotoli_richiesti[j].quantita_richiesta=checkValFloat(val);
										if(progetti[i].rotoli_richiesti[j].quantita_richiesta<=0){
											errore("\tERRORE: Valore non valido!\n");
										}
									}while(progetti[i].rotoli_richiesti[j].quantita_richiesta<=0);
								}
								progetti[i].costo_approssimato=calcolaCostoProgetto(i,dim);
								progetti[i].ricavi=progetti[i].paga-progetti[i].costo_approssimato;
								co(8);
								printf("\t(Costo: %.2f)\n",progetti[i].costo_approssimato);
								pausa("Continua...\n");
							}else{
								do{
									printf("Nuovi scarti: ");
									scanf(" %s",val);
									progetti[i].scarti_richiesti=checkValInt(val);
									if(progetti[i].scarti_richiesti<=0){
										errore("ERRORE: Valore non valido!\n");
									}
								}while(progetti[i].scarti_richiesti<=0);
							}
							break;
						case 3:
							do{
								err=1;
								printf("Nuovo tessuto: ");
								scanf(" %s",progetti[i].rotoli_richiesti[0].rotolo_richiesto);
								for(k=0;k<RCount;k++){
									if(strcmp(progetti[i].rotoli_richiesti[0].rotolo_richiesto,inventario[k].codice_rotolo)==0){
										err=0;
									}
								}
								if(err!=0){
									errore("ERRORE: Rotolo non trovato!\n");
								}
							}while(err!=0);
							break;
						case 4:
							do{
								printf("Nuova paga: ");
								scanf(" %s",val);
								progetti[i].paga=checkValFloat(val);
								if(progetti[i].paga<=0){
									errore("ERRORE: Valore non valido!\n");
								}
							}while(progetti[i].paga<=0);
							progetti[i].ricavi=progetti[i].paga-progetti[i].costo_approssimato;
							break;
					}
				}
			}while(tasto!=27);
			f=0;
		}
	}
	return f;
}
// Funzione che stampa i tessuti
int mostraTessuti(int dim){
	int i,tasto=0,j,k,f=1;
	for(i=0;i<dim;i++){
		f=0;
		do{
			// Stampo tutti i campi
			system(CLEAR);
			printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
			printf(" Menu Sartoria      |  Budget: %.2f euro\n",budget);
			printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");	
			printf("Rotolo [%d/%d]\n",i+1,dim);
			if(inventario[i].rot.usura>MAXUSURA){
				co(9);
				printf("Rotolo inutilizzabile! E' necessario ricomprarlo per utilizzarlo di nuovo!\n");
				co(8);
			}
			printf("Codice: %s\n",inventario[i].codice_rotolo);
			printf("Fornitore: %s\n",inventario[i].fornitore);
			printf("Tipo di tessuto: %s\n",inventario[i].rot.tipo_tessuto);
			printf("Colore: %s\n",inventario[i].rot.colore);
			printf("Fantasia: %s\n",inventario[i].rot.fantasia);
			printf("Codice fornitore: %s\n",inventario[i].rot.codice_fornitura);
			printf("Data di acquisto: %d/%d/%d\n",inventario[i].g,inventario[i].m,inventario[i].a);
			printf("Quantita' disponibile: %.2f M^2\n",inventario[i].quantita_disponibile);
			printf("Utilizzo previsto: %.2f M^2\n",inventario[i].utilizzo_previsto);
			printf("Scarti: %d\n",inventario[i].scarti_utilizzabili);
			co(7);
			printf("Usura: ");
			if(inventario[i].rot.usura>MAXUSURA){
				co(4);
			}else{
				if(inventario[i].rot.usura>15){
					co(6);
				}else{
					co(7);
				}
			}
			printf("%.2f",inventario[i].rot.usura);
			co(7);
			printf(" %%\n");
			printf("Costo: %.2f euro\n",inventario[i].rot.costo);
			printf("Costo M^2: %.2f euro\n\n",inventario[i].rot.costo/((inventario[i].rot.larghezza/100)*inventario[i].rot.lunghezza));
			// Attendo un input
			if(inventario[i].rot.usura>MAXUSURA){
				tasto=pausa("[<-] [->] Muoviti | [INVIO] Ricompra | [ESC] Esci");	
			}else{
				tasto=pausa("[<-] [->] Muoviti | [SPAZIO] Modifica | [INVIO] Ricompra | [ESC] Esci");
			}
			// Se <- torno indietro di 1
			if(tasto==13){
				riacquista(i);
			}			
			if(tasto==1002){
				if(i>0){
					i-=2;
				}else{
					i--;
				}
			}
			// Se -> e sono al limite rimane sullo stesso tessuto
			if(tasto==1003 && i==dim-1){
				i--;
			}
			// Modifico il rotolo corrente
			if(tasto==32 && inventario[i].rot.usura<=MAXUSURA){
				modificaRotolo(dim,inventario[i].codice_rotolo);
			}
			// Esco dalla funzione
			if(tasto==27){
				return 0;
			}
		}while(tasto!=1003 && tasto != 1002 && tasto!=27);
	}
	return f;
}
/*
Funzione che elimina un progetto
0: Eliminato
1: Non trovato
*/
int eliminaProgetto(int *PCount, char filtro[]){
	int i,j;
	for(i=0;i<*PCount;i++){
		if(strcmp(progetti[i].nome_progetto,filtro)==0){
			for(j=i;j<*PCount-1;j++){
				progetti[j]=progetti[j+1];
			}
			(*PCount)--;
			return 0;
		}
	}
	return 1;
}
// Funzione che crea un nuovo progetto
int nuovoProgetto(int *PCount,int RCount){
	if(*PCount>=MAXPROGETTI){
		return 1;
	}else{
		int i=*PCount,j,k,t,scelta,err,tasto=0;
		char val[100];
		printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
		printf(" Menu Sartoria      |  Budget: %.2f euro\n",budget);
		printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
		printf("Nuovo progetto:\n");
		do{
			err=0;
			printf("\tNome progetto: ");
			scanf(" %s",progetti[i].nome_progetto);
			for(j=0;j<*PCount;j++){
				if(strcmp(progetti[j].nome_progetto,progetti[i].nome_progetto)==0){
					errore("\tERRORE: Nome gia' esistente!\n\n");
					err=1;
				}
			}
		}while(err==1);
		err=1;
		// Chiedo che tipo di progetto è
		do{
			printf("\tTipo progetto:\n");
			printf("\t1: Normale - 2: Mini (Utilizza gli scarti)\n");
			printf("\tScelta: ");
			scanf(" %s",val);
			scelta=checkValInt(val);
			if(scelta<1 || scelta >2){
				errore("\tERRORE: Valore non valio!\n");
			}
		}while(scelta<1 || scelta >2);
		if(scelta==2){
			progetti[i].mini=1;
		}else{
			progetti[i].mini=0;
		}
		printf("\tTipo di capo: ");
		scanf(" %s",progetti[i].tipoCapo);
		if(scelta==1){
			do{	
				printf("\tQuanti rotoli usa il progetto (MAX: ");
				if(RCount<MAXP){
					printf("%d",RCount);
				}else{
					co(2);
					printf("%d",MAXP);
					co(7);
				}
				printf(") : ");
				scanf(" %s",val);
				progetti[i].rdim=checkValInt(val);
				if(progetti[i].rdim<=0){
					errore("\tERRORE: Valore non valido!\n");
				}
				if(progetti[i].rdim>RCount){
					errore("\tERRORE: Non ci sono abbastanza rotoli!\n");
				}
			}while(progetti[i].rdim<=0 || progetti[i].rdim>RCount);
			for(j=0;j<progetti[i].rdim;j++){
				for(k=0;k<RCount;k++){
					system(CLEAR);
					printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
					printf(" Menu Sartoria      |  Budget: %.2f euro\n",budget);
					printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
					printf("Nuovo progetto:\n");
					printf("\tNome progetto: %s\n",progetti[i].nome_progetto);
					printf("\tTipo progetto:\n");
					printf("\t1: Normale - 2: Mini (Utilizza gli scarti)\n");
					printf("\tScelta: %d\n",scelta);
					printf("\tTipo di capo: %s\n",progetti[i].tipoCapo);
					printf("\tQuanti rotoli usa il progetto (MAX: ");
					if(RCount<MAXP){
						printf("%d",RCount);
					}else{
						co(2);
						printf("%d",MAXP);
						co(7);
					}
					printf(") : %d\n",progetti[i].rdim);
					printf("\n\tRotoli:\n");
					for(t=0;t<j;t++){
						printf("\t\tRotolo [%d/%d]\n",t+1,progetti[i].rdim);
						printf("\t\tCodice rotolo: %s\n",progetti[i].rotoli_richiesti[t].rotolo_richiesto);
						printf("\t\tQuantita' richiesta (M^2): %.2f\n\n",progetti[i].rotoli_richiesti[t].quantita_richiesta);
					}
					printf("\t\tRotolo [%d/%d]\n",j+1,progetti[i].rdim);
					printf("\t\tCodice rotolo: ");
					co(15);
					printf("%s (%.2f)",inventario[k].codice_rotolo,inventario[k].quantita_disponibile);
					co(8);
					printf("\t[<-][->]");
					co(7);
					tasto=pausa("\n");
					if(tasto==1002){
						if(k>0){
							k-=2;
						}else{
							k--;
						}
					}
					if(tasto==13){
						strcpy(progetti[i].rotoli_richiesti[j].rotolo_richiesto,inventario[k].codice_rotolo);
						k=RCount;
					}
					if(k==RCount-1){
						k--;
					}
				}
				do{
					printf("\t\tQuantita' richiesta (M^2): ");
					scanf(" %s",val);
					progetti[i].rotoli_richiesti[j].quantita_richiesta=checkValFloat(val);
					if(progetti[i].rotoli_richiesti[j].quantita_richiesta<=0){
						errore("\t\tERRORE: Valore non valido!\n");
					}
				}while(progetti[i].rotoli_richiesti[j].quantita_richiesta<=0);
			}
		}else{
			// Se il progetto è 'mini' allora chiedo gli scarti che servono
			do{
				printf("\tScarti richiesti: ");
				scanf(" %s",val);
				progetti[i].scarti_richiesti=checkValInt(val);
				if(progetti[i].scarti_richiesti<=0){
					errore("\tERRORE: Valore non valido!\n");
				}
			}while(progetti[i].scarti_richiesti<=0);
			for(k=0;k<RCount;k++){
				system(CLEAR);
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
				printf(" Menu Sartoria      |  Budget: %.2f euro\n",budget);
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
				printf("Nuovo progetto:\n");
				printf("\tNome progetto: %s\n",progetti[i].nome_progetto);
				printf("\tTipo progetto:\n");
				printf("\t1: Normale - 2: Mini (Utilizza gli scarti)\n");
				printf("\tScelta: %d\n",scelta);
				printf("\tTipo di capo: %s\n",progetti[i].tipoCapo);
				printf("\tScarti richiesti: %d\n",progetti[i].scarti_richiesti);
				printf("\tRotolo: ");
				if(inventario[k].scarti_utilizzabili>=progetti[i].scarti_richiesti){
					co(15);
				}else{
					co(4);
				}
				printf("%s (%d)",inventario[k].codice_rotolo,inventario[k].scarti_utilizzabili);
				co(7);
				tasto=pausa("\n");
				if(tasto==1002){
					if(k>0){
						k-=2;
					}else{
						k--;
					}
				}
				if(tasto==13){
					strcpy(progetti[i].rotoli_richiesti[0].rotolo_richiesto,inventario[k].codice_rotolo);
					k=RCount;
				}
				if(k==RCount-1){
					k--;
				}
			}
			for(j=0;j<RCount;j++){
				if(strcmp(progetti[i].rotoli_richiesti[0].rotolo_richiesto,inventario[j].codice_rotolo)==0){
					if(progetti[i].scarti_richiesti>inventario[j].scarti_utilizzabili){
						errore("\tImpossibile eseguire il progetto, non ci sono scarti a sufficenza!\n");
						pausa("Continua...\n");
						return 0;
					}
				}
			}
			progetti[i].rotoli_richiesti[0].quantita_richiesta=0;
			progetti[i].rdim=1;
		}
		co(3);
		printf("\n\tCalcolo costo in corso...\n");
		co(7);
		progetti[i].costo_approssimato=calcolaCostoProgetto(i,RCount);		// Calcolo il costo del progetto in base alle scorte attuali
		co(8);
		printf("\tValore risorse: %.2f euro\n",progetti[i].valore);
		printf("\tIl progetto ha un costo approssimato di %.2f euro\n\n",progetti[i].costo_approssimato);
		co(7);
		do{
			printf("\tPaga del progetto: ");
			scanf(" %s",val);
			progetti[i].paga=checkValFloat(val);
			if(progetti[i].paga<=0){
				errore("\tERRORE: Valore non valido!\n");
			}
		}while(progetti[i].paga<=0);
		progetti[i].ricavi=progetti[i].paga-progetti[i].costo_approssimato;
		printf("\tRicavi stimati: ");
		if(progetti[i].ricavi>=0){
			co(2);
		}else{
			co(4);
		}
		printf("%.2f ",progetti[i].ricavi);		// Stampo il ricavo
		co(7);
		printf("euro\n");
		(*PCount)++;
		pausa("Continua...\n");
		return 0;
	}
}
// Funzione che calcola il costo di un progetto
float calcolaCostoProgetto(int dim,int RCount){
	int i,j;
	float costo=0,q,u;
	progetti[dim].valore=0;
	for(j=0;i<progetti[dim].rdim;j++){
		for(i=0;i<RCount;i++){
			if(strcmp(inventario[i].codice_rotolo,progetti[dim].rotoli_richiesti[j].rotolo_richiesto)==0){
				u=progetti[dim].rotoli_richiesti[j].quantita_richiesta;
				q=0;
				// Valore
				do{
					if(u>q){
						q++;
						progetti[dim].valore+=inventario[i].rot.costo/(inventario[i].rot.lunghezza*(inventario[i].rot.larghezza/100));	// Aumento il valore
					}
				}while(u>q);
				// Costo
				q=inventario[i].quantita_disponibile;
				do{
					if(u>q){
						q++;
						costo+=inventario[i].rot.costo/(inventario[i].rot.lunghezza*(inventario[i].rot.larghezza/100));	// Aumento il costo fino a quando ne ho abbastanza
					}
				}while(u>q);
				break;
			}
		}
	}
	return costo;
}
/*
Funzione che elimina un rotolo
0: Eliminato
1: Non trovato
*/
int eliminaRotolo(int *RCount, char filtro[]){
	int i,j;
	for(i=0;i<*RCount;i++){
		if(strcmp(inventario[i].codice_rotolo,filtro)==0){
			for(j=i;j<*RCount-1;j++){
				inventario[j]=inventario[j+1];	// Sposto tutto di uno
			}
			(*RCount)--;
			return 0;
		}
	}
	return 1;
}
/*
Funzione che modifica i dati di un rotolo
0: Modfica andata a buon fine
1: Nessun rotolo trovato
*/
int modificaRotolo(int dim, char filtro[]){
	int i,tasto=0,stato=0,j,f=1;
	char val[100];
	for(i=0;i<dim;i++){
		if(strcmp(filtro,inventario[i].codice_rotolo)==0){
			printf("Rotolo trovato!\n");
			do{
				system(CLEAR);
				printf("MODIFICA:\n");
				printf("(SU/GIU): Spostati, (INVIO): Seleziona, (ESC): Esci\n\n");
				if(tasto==1000){
					stato--;
				}
				if(tasto==1001){
					stato++;
				}
				if(stato<1){
					stato=1;
				}
				if(stato>7){
					stato=7;
				}
				for(j=0;j<8;j++){
                    if(stato==j){	
						co(15);	// se il campo è quello scelto allora lo evidenzio
					}else{
						co(8);
					}
					switch(j){
                        case 0:
							co(11);
                            printf("Codice rotolo: %s\n", inventario[i].codice_rotolo);
                            break;
                        case 1:
                            printf("Fornitore: %s\n", inventario[i].fornitore);
                            break;
                        case 2:
                            printf("Tipo tessuto: %s\n", inventario[i].rot.tipo_tessuto);
                            break;
                        case 3:
                            printf("Colore: %s\n", inventario[i].rot.colore);
                            break;
                        case 4:
                            printf("Fantasia: %s\n", inventario[i].rot.fantasia);
                            break;
                        case 5:
                            printf("Lunghezza: %.2f m\n", inventario[i].rot.lunghezza);
                            break;
                        case 6:
                            printf("Larghezza: %.2f cm\n", inventario[i].rot.larghezza);
                            break;
                        case 7:
                            printf("Costo: %.2f euro\n",inventario[i].rot.costo);
                            break;
					}
				}
				co(7);
				printf("- - - - - - - - - - - - - - - - - - -\n");
				tasto=pausa("");
				if(tasto==13){
					switch(stato){
						case 1:
							printf("Nuovo Fornitore: ");
							scanf(" %s",inventario[i].fornitore);
							break;
						case 2:
							printf("Nuovo Tipo tessuto: ");
							scanf(" %s",inventario[i].rot.tipo_tessuto);
							break;
						case 3:
							printf("Nuovo Colore: ");
							scanf(" %s",inventario[i].rot.colore);
							break;
						case 4:
							printf("Nuova Fantasia: ");
							scanf(" %s",inventario[i].rot.fantasia);
							break;
						case 5:
							do{
								printf("Nuova Lunghezza (m): ");
								scanf(" %s",val);
								inventario[i].rot.lunghezza=checkValFloat(val);
								if(inventario[i].rot.lunghezza<=0){
									errore("ERRORE: Valore non valido!\n");
								}
							}while(inventario[i].rot.lunghezza<=0);
							inventario[i].quantita_disponibile=(inventario[i].rot.larghezza/100)*inventario[i].rot.lunghezza;
							break;
						case 6:
							do{
								printf("Nuova Larghezza (cm): ");
								scanf(" %s",val);
								inventario[i].rot.larghezza=checkValFloat(val);
								if(inventario[i].rot.larghezza<=0){
									errore("ERRORE: Valore non valido!\n");
								}
							}while(inventario[i].rot.larghezza<=0);
							inventario[i].quantita_disponibile=(inventario[i].rot.larghezza/100)*inventario[i].rot.lunghezza;
							break;
						case 7:
							// float vecchio=inventario[i].rot.costo;
							do{
								printf("Nuovo Costo (euro): ");
								scanf(" %s",val);
								inventario[i].rot.costo=checkValFloat(val);
								if(inventario[i].rot.costo<0){
									errore("ERRORE: Valore non valido!\n");
								}
							}while(inventario[i].rot.costo < 0);
							//Errato
							// budget+=vecchio;
							// budget-=inventario[i].rot.costo;
							break;
					}
				}
			}while(tasto!=27);
			f=0;
		}
	}
	return f;
}
/*
Funzione che aggiunge un rotolo all'inventario
 0 = Inserimento avvenuto con successo
 1 = Dimensione massima superata
*/
int nuovoRotolo(int *RCount){
	if(*RCount>=MAXTESSUTI){
		return 1;
	}else{
		int i=*RCount,g,m,a,err,j;
		char scelta='Y';
		char val[100],v1[100],v2[100],v3[100];
		printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
		printf(" Menu Sartoria      |  Budget: %.2f euro\n",budget);
		printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
		printf("Nuovo rotolo:\n");
		do{
			err=0;
			printf("\tCodice Rotolo: ");
			scanf(" %s",inventario[i].codice_rotolo);							// Input codice rotolo
			/*
			for(j=0;j<*RCount;j++){
				if(strcmp(inventario[j].codice_rotolo,inventario[i].codice_rotolo)==0){
					errore("\tERRORE: Rotolo gia' esistente!\n\n");
					err=1;
				}
			}
			*/
		}while(err!=0);
		printf("\tFornitore: ");
		scanf(" %s",inventario[i].fornitore);								// Input nome fornitore
		printf("\tDati rotolo:\n");
		printf("\t\tTipo di tessuto: ");
		scanf(" %s",inventario[i].rot.tipo_tessuto);						// Input Tipo di tessuto
		printf("\t\tColore: ");
		scanf(" %s",inventario[i].rot.colore);								// Input colore tessuto
		printf("\t\tFantasia: ");
		scanf(" %s",inventario[i].rot.fantasia);							// Input fantasia
		do{
			printf("\t\tLunghezza (M): ");
			scanf(" %s",val);
			inventario[i].rot.lunghezza=checkValFloat(val);					// Controllo che il valore sia valido
			if(inventario[i].rot.lunghezza<=0){
				errore("\t\tERRORE: Valore non valido!!\n");
			}
		}while(inventario[i].rot.lunghezza<=0);
		do{
			printf("\t\tLarghezza (cm): ");
			scanf(" %s",val);
			inventario[i].rot.larghezza=checkValFloat(val);					// Controllo che il valore sia valido
			if(inventario[i].rot.larghezza<=0){
				errore("\t\tERRORE: Valore non valido!!\n");
			}
		}while(inventario[i].rot.larghezza<=0);
		inventario[i].quantita_disponibile=(inventario[i].rot.larghezza/100)*inventario[i].rot.lunghezza;
		co(8);
		printf("\t\tQuantita' disponibile: %.2f M^2\n",inventario[i].quantita_disponibile);
		co(7);
		printf("\t\tCodice Fornitura: ");
		scanf(" %s",inventario[i].rot.codice_fornitura);					// Input coice del fornitore
		do{
			printf("\t\tCosto ");
			co(8);
			printf("(rotolo intero)");
			co(7);
			printf(": ");
			scanf(" %s",val);
			inventario[i].rot.costo=checkValFloat(val);						// Controllo che il valore sia valido
			if(inventario[i].rot.costo<0){
				errore("\t\tERRORE: Valore non valido!\n");
			}
			if(inventario[i].rot.costo>budget){
				printf("\t\tIl costo va oltre il tuo budget. Sei sicuro di volerlo comprare? (Y/N):\n");
				printf("\t\tScelta: ");
				scanf(" %c",&scelta);
			}
			if(scelta!='y' && scelta !='Y'){
				printf("\t\tVuoi interrompere l'inserimento? (Y/N) : ");
				scanf(" %c",&scelta);
				if(scelta=='y' || scelta=='Y'){								// Se si vuole si può interrompere l'inserimento
					return 0;
				}
			}
		}while(inventario[i].rot.costo<0 || scelta!='Y' && scelta!='y');
		co(8);
		printf("\t\tCosto M^2: %.2f\n",inventario[i].rot.costo/((inventario[i].rot.larghezza/100)*inventario[i].rot.lunghezza));
		co(7);
		budget-=inventario[i].rot.costo;
		inventario[i].rot.usura=0;
		do{
			printf("\n\tData di acquisto (GG MM AAAA): ");
			scanf(" %s %s %s",v1,v2,v3);
			inputData(v1,v2,v3,&g,&m,&a);
			err=checkData(g,m,a);
			if(err!=0){
				errore("\tData non valida!!\n");
			}
		}while(err!=0);
		inventario[i].g=g;
		inventario[i].m=m;
		inventario[i].a=a;
		inventario[i].utilizzo_previsto=0;
		inventario[i].scarti_utilizzabili=0;
		co(8);
		printf("\n\nRotolo aggiunto con successo! ID: %s\n",inventario[i].codice_rotolo);
		co(7);
		pausa("Continua...\n");												// Attendo un input
		(*RCount)++;
		return 0;
	}
}
/*
Funzione che carica i dati dai due file e stampa messaggi di errore se si verificano
*/
void caricaInventario(int *RCount, int *PCount,int *PresetCount){
	int i,j;
	FInv=fopen(FILEINVENTARIO,"r");
	if(FInv==NULL){
		co(4);
		printf("Si e' verificato un'errore nell'apertura del file '%s'.\n",FILEINVENTARIO);
		co(7);
	}else{
		// leggo i tessuti
		fscanf(FInv,"%d %f",RCount,&budget);
		for(i=0;i<*RCount;i++){
			fscanf(FInv,"%s %s %s %s %s %f %f %s %f %f %d %d %d %f %f %d",inventario[i].codice_rotolo,inventario[i].fornitore,inventario[i].rot.tipo_tessuto,inventario[i].rot.colore,inventario[i].rot.fantasia,&inventario[i].rot.lunghezza,&inventario[i].rot.larghezza,inventario[i].rot.codice_fornitura,&inventario[i].rot.costo,&inventario[i].rot.usura,&inventario[i].g,&inventario[i].m,&inventario[i].a,&inventario[i].quantita_disponibile,&inventario[i].utilizzo_previsto,&inventario[i].scarti_utilizzabili);
		}
		co(2);
		printf("Caricati con successo i tessuti!\n");
		co(7);
	}
	FProg=fopen(FILEPROGETTI,"r");
	if(FProg==NULL){
		co(4);
		printf("Si e' verificato un'errore nell'apertura del file '%s'.\n",FILEPROGETTI);
		co(7);
	}else{
		// leggo i progetti
		fscanf(FProg,"%d",PCount);
		for(i=0;i<*PCount;i++){
			fscanf(FProg,"%s %f %d %f %s %d %f %f %f",progetti[i].nome_progetto,&progetti[i].costo_approssimato,&progetti[i].mini,&progetti[i].scarti_richiesti,progetti[i].tipoCapo,&progetti[i].rdim,&progetti[i].paga,&progetti[i].ricavi,&progetti[i].valore);
			for(j=0;j<progetti[i].rdim;j++){
				fscanf(FProg,"%s %f",progetti[i].rotoli_richiesti[j].rotolo_richiesto,&progetti[i].rotoli_richiesti[j].quantita_richiesta);
			}
		}
		co(2);
		printf("Caricati con successo i progetti!\n");
		co(7);
	}
	Preset=fopen(FILEPRESET,"r");
	if(Preset==NULL){
		co(4);
		printf("Si e' verificato un'errore nell'apertura del file '%s'.\n",FILEPRESET);
		co(7);
	}else{
		// Leggo i preset
		fscanf(Preset,"%d",PresetCount);
		for(i=0;i<*PresetCount;i++){
			fscanf(Preset,"%s",ArrayPreset[i]);
		}
		co(2);
		printf("Caricati con successo i preset!\n");
		co(7);
	}
	pausa("[INVIO] Continua...\n");
}
/*
Funzione che salva i dati du due file separati, uno per l'inventario e uno per i progetti
*/
void salvaInventario(int RCount, int PCount, int PresetCount){
	int i,j;
	FInv=fopen(FILEINVENTARIO,"w");
	if(FInv==NULL){
		co(4);
		printf("Si e' verificato un'errore nell'apertura del file '%s'.\n",FILEINVENTARIO);
		co(7);
	}else{
		// Salvo i tessuti
		fprintf(FInv,"%d %f\n",RCount,budget);
		for(i=0;i<RCount;i++){
			fprintf(FInv,"%s %s %s %s %s %f %f %s %f %f %d %d %d %f %f %d\n",inventario[i].codice_rotolo,inventario[i].fornitore,inventario[i].rot.tipo_tessuto,inventario[i].rot.colore,inventario[i].rot.fantasia,inventario[i].rot.lunghezza,inventario[i].rot.larghezza,inventario[i].rot.codice_fornitura,inventario[i].rot.costo,inventario[i].rot.usura,inventario[i].g,inventario[i].m,inventario[i].a,inventario[i].quantita_disponibile,inventario[i].utilizzo_previsto,inventario[i].scarti_utilizzabili);
		}
	}
	FProg=fopen(FILEPROGETTI,"w");
	if(FProg==NULL){
		co(4);
		printf("Si e' verificato un'errore nell'apertura del file '%s'.\n",FILEPROGETTI);
		co(7);
	}else{
		// Salvo i progetti
		fprintf(FProg,"%d\n",PCount);
		for(i=0;i<PCount;i++){
			fprintf(FProg,"%s %f %d %f %s %d %f %f %f\n",progetti[i].nome_progetto,progetti[i].costo_approssimato,progetti[i].mini,progetti[i].scarti_richiesti,progetti[i].tipoCapo,progetti[i].rdim,progetti[i].paga,progetti[i].ricavi,progetti[i].valore);
			for(j=0;j<progetti[i].rdim;j++){
				fprintf(FProg,"%s %f\n",progetti[i].rotoli_richiesti[j].rotolo_richiesto,progetti[i].rotoli_richiesti[j].quantita_richiesta);
			}
		}
	}
	Preset=fopen(FILEPRESET,"w");
	if(Preset==NULL){
		co(4);
		printf("Si e' verificato un'errore nell'apertura del file '%s'.\n",FILEPRESET);
		co(7);
	}else{
		// Salvo la lista dei preset
		fprintf(Preset,"%d\n",PresetCount);
		for(i=0;i<PresetCount;i++){
			fprintf(Preset,"%s\n",ArrayPreset[i]);
		}
	}
	fclose(Preset);
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
	3.5) Reset
 4) Presets
	4.1) Crea un preset
	4.2) Carica un preset
 5) Esci
	5.1) Termina il programma
*/
int menu(int mode){
	int s1,s2,tasto=0,stato=mode,i,j;
	do{
		system(CLEAR);
		co(7);
		printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
		printf(" Menu Sartoria      |  Budget: %.2f euro\n",budget);
		printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
		printf("[SU/GIU] Muoviti, [INVIO] Seleziona\n");
		for(i=1;i<=5;i++){
			// Evidenzia la scelta attuale
			if(stato==i){
				co(15);
			}else{
				co(8);
			}
			switch(i){
				case 1:
					printf("\tGestione rotoli\n");
					break;
				case 2:
					printf("\tGestione progetti\n");
					break;
				case 3:
					printf("\tGestione inventario\n");
					break;
				case 4:
					printf("\tPresets\n");
					break;
				case 5:
					printf("\tEsci\n");
					break;
			}
		}
		// Leggo l'input dell'utente
		tasto=pausa("\n");
		if(tasto==1000){
			stato--;	// Torno indietro
		}else if(tasto==1001){
			stato++;	// Vado avanti
		}
		if(stato<1){
			stato=1;	// Controllo di non uscire
		}else if(stato>5){
			stato=5;
		}	
		// Seleziono la scelta
		if(tasto==13){
			s1=stato;
		}
	}while(tasto!=13);
	stato=1;
	if(s1==5){
		return 51;
	}
	do{
		system(CLEAR);
		co(7);
		printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
		printf(" Menu Sartoria      |  Budget: %.2f euro\n",budget);
		printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
		printf("[SU/GIU] Muoviti, [INVIO] Seleziona, [ESC] Esci\n");
		for(j=1;j<6;j++){
			if(j==s1){
				co(11);
			}else{
				co(8);
			}
			switch(j){
				case 1:
					printf("\tGestione rotoli\n");
					break;
				case 2:
					printf("\tGestione progetti\n");
					break;
				case 3:
					printf("\tGestione inventario\n");
					break;
				case 4:
					printf("\tPreset\n");
					break;
				case 5:
					printf("\tEsci\n");
					break;
			}
			if(j==s1){
				for(i=1;i<=5;i++){
					if(stato==i){
						co(7);
					}else{
						co(8);
					}
					
					switch(i){
						case 1:
							if(s1==1){
								printf("\t\tNuovo rotolo\n");
							}else if(s1==2){
								printf("\t\tNuovo progetto\n");
							}else if(s1==3){
								printf("\t\tMostra tessuti\n");
							}else{
								printf("\t\tCrea preset\n");
							}
							break;
						case 2:
							if(s1==1){
								printf("\t\tModifica rotolo\n");
							}else if(s1==2){
								printf("\t\tModifica progetto\n");
							}else if(s1==3){
								printf("\t\tControlla i tessuti\n");
							}else{
								printf("\t\tCarica preset\n");
							}
							break;
						case 3:
							if(s1==1){
								printf("\t\tElimina rotolo\n");
							}else if(s1==2){
								printf("\t\tElimina progetto\n");
							}else if(s1==3){
								printf("\t\tRotazione scorte\n");
							}
							break;
						case 4:
							if(s1==2){
								printf("\t\tMostra progetti\n");
							}else if(s1==3){
								printf("\t\tSalvataggio\n");
							}
							break;
						case 5:
							if(s1==2){
								printf("\t\tAvvia taglio\n");
							}else if(s1==3){
								printf("\t\tReset\n");
							}
							break;
					}
				}
			}
		}
		tasto=pausa("");
		// Torno indietro
		if(tasto==27){
			return menu(s1);
		}
		if(tasto==1000){
			stato--;
		}else if(tasto==1001){
			stato++;
		}
		if(stato<1){
			stato=1;
		}
		if(s1==1){
			if(stato>3){
				stato=3;
			}
		}else if(s1==4){
			if(stato>2){
				stato=2;
			}
		}else{
			if(stato>5){
				stato=5;
			}
		}
		if(tasto==13){
			s2=stato;
		}
	}while(tasto!=13);
	co(7);
	system(CLEAR);
	return s1*10 + s2;
}
// Funzione che resetta il programma
void reset(int *RCount, int *PCount){
	budget=BUDGETINIZIALE;
	*RCount=0;
	*PCount=0;
}