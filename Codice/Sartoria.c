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

struct rotoli{
	char rotolo_richiesto[MAXSTRING];
	float metraggio_richiesto;
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
} progetti[MAXPROGETTI];				// Array globale dei progetti


// FUNZIONI

int menu();							// Stampa il menu
int nuovoRotolo(int*);				// Aggiunge un nuovo rotolo
int modificaRotolo(int,char[]);			// Modifica un rotolo esistente
int eliminaRotolo(int*,char[]);			// Elimina un rotolo (azzera la scheda e diminusce la dimensione logica)
int nuovoProgetto(int*,int);			// Aggiunge un nuovo progetto
int modificaProgetto(int);			// Modifica un progetto
int eliminaProgetto(int*);			// Elimina un progetto
float calcolaCostoProgetto(int,int);	// Calcola il costo progetto (Ogni volta che il rotolo finisce lo riacquista)
void mostraProgetti(int);			// Stampa i progetti
int avviaTaglio(int*);				// Avvia il taglio (rimuove i progetti in attesa)
void mostraTessuti(int);			// Mostra i tessuti
int controlloTessuti(int);			// Controlla i tessuti con usura troppo alta e ne propone la sostituzione
int rotazioneScorte(int);			// Ruota le scorte
// Should
float aumentoUsura();				// Aumenta l'usura (a ogni azione o a caso. ancora da decidere...)
void salvaInventario(int,int);		// Sotto forma di file .txt
void caricaInventario(int*,int*);	// e salva sia tessuti che progetti
void reset(int*,int*);


// Main
int main(){
	int RCount=0,PCount=0,scelta,err,tasto;
	char val[10],filter[MAXSTRING];
	caricaInventario(&RCount,&PCount);
	system(CLEAR);
	do{
		scelta=menu();
		switch(scelta){
			case 11:
				err=nuovoRotolo(&RCount);
				if(err==1){
					co(4);
					printf("Dimensione massima inventario raggiunta!\n");
					co(7);
				}
				break;
			case 12:
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
				printf(" Menu Sartoria      |  Budget: %.2f euro\n",budget);
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
				printf("Inserisci il codice da cercare: ");
				scanf(" %s",filter);
				if(modificaRotolo(RCount,filter)==1){
					co(4);
					printf("Rotolo non trovato!\n");
					co(7);
					pausa("Continua...\n");
				}
				break;
			case 13:
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
				printf(" Menu Sartoria      |  Budget: %.2f euro\n",budget);
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
				printf("Inserisci il codice da cercare: ");
				scanf(" %s",filter);
				if(eliminaRotolo(&RCount,filter)==1){
					co(4);
					printf("Rotolo non trovato!\n");
					co(7);
				}else{
					co(2);
					printf("Rotolo eliminato con successo!\n");
					co(7);
				}
				pausa("Continua...\n");
				break;
			case 21:
				if(nuovoProgetto(&PCount,RCount)==1){
					co(4);
					printf("ERRORE: dimensione massima raggiunta!\n");
					co(7);
				}
				break;
			case 31:
				mostraTessuti(RCount);
				pausa("Continua...\n");
				break;
			case 34:
				co(8);
				printf("Salvataggio in corso...\n");
				co(7);
				salvaInventario(RCount,PCount);
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
					reset(&RCount,&PCount);
					co(2);
					printf("Reset effettuato!\n");
					co(7);
				}else{
					co(4);
					printf("Reset annullato\n");
					co(7);
				}
				pausa("\nContinua...\n");
				break;
			case 41:
				salvaInventario(RCount,PCount);				// Salvo il programma
				printf("Uscita in corso...\n");				// Termino il programma
				break;
			default:
				co(4);
				printf("Si e' verificato un'errore!\n");	// In caso di qualche bug durante la scelta
				co(7);
				break;
		}
		if(scelta != 41){
			system(CLEAR);		// Pulisco lo schermo
		}
		co(7);
	}while(scelta!=41);
	return 0;
}
// Funzione che stampa i tessuti
void mostraTessuti(int dim){
	int i,tasto;
	for(i=0;i<dim;i++){
		do{
			system(CLEAR);
			printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
			printf(" Menu Sartoria      |  Budget: %.2f euro\n",budget);
			printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");	
			printf("Rotolo [%d/%d]\n",i+1,dim);
			printf("Codice: %s\n",inventario[i].codice_rotolo);
			printf("Fornitore: %s\n",inventario[i].fornitore);
			printf("Tipo di tessuto: %s\n",inventario[i].rot.tipo_tessuto);
			printf("Colore: %s\n",inventario[i].rot.colore);
			printf("Fantasia: %s\n",inventario[i].rot.fantasia);
			printf("Codice fornitore: %s\n",inventario[i].rot.codice_fornitura);
			printf("Data di acquisto: %d/%d/%d\n",inventario[i].data_acquisto.g,inventario[i].data_acquisto.m,inventario[i].data_acquisto.a);
			printf("Quantita' disponibile: %.2f\n",inventario[i].quantita_disponibile);
			printf("Utilizzo previsto: %.2f\n",inventario[i].utilizzo_previsto);
			printf("Scarti: %d\n",inventario[i].scarti_utilizzabili);
			printf("Usura: %.1f\n",inventario[i].rot.usura);
			printf("Costo: %.2f\n\n",inventario[i].rot.costo);
			tasto=pausa("[<-] [->]");
			if(tasto==1002){
				if(i>0){
					i-=2;
				}
			}
		}while(tasto!=1003 && tasto != 1002);
	}
}
// Funzione che crea un nuovo progetto
int nuovoProgetto(int *PCount,int RCount){
	if(*PCount>=MAXPROGETTI){
		return 1;
	}else{
		int i=*PCount,j,k,scelta,err=1;
		char val[10];
		printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
		printf(" Menu Sartoria      |  Budget: %.2f euro\n",budget);
		printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
		printf("Nuovo progetto:\n");
		printf("\tNome progetto: ");
		scanf(" %s",progetti[i].nome_progetto);
		do{
			printf("\tTipo progetto:\n");
			printf("\t1: Normale - 2: Mini (Utilizza gli scarti)\n");
			printf("\tScelta: ");
			scanf(" %s",val);
			scelta=checkValInt(val);
			if(scelta<1 || scelta >2){
				co(4);
				printf("\tERRORE: Valore non valio!\n");
				co(7);
			}
		}while(scelta<1 || scelta >2);
		printf("\tTipo di capo: ");
		scanf(" %s",progetti[i].tipoCapo);
		if(scelta==1){
			do{	
				printf("\tQuanti rotoli usa il progetto (MAX: %d): ",MAXP);
				scanf(" %s",val);
				progetti[i].rdim=checkValInt(val);
				if(progetti[i].rdim<=0){
					co(4);
					printf("\tERRORE: Valore non valido!\n");
					co(7);
				}
			}while(progetti[i].rdim<=0);
			printf("\n\tRotoli:\n");
			for(j=0;j<progetti[i].rdim;j++){
				do{
					printf("\t\tCodice rotolo [%d/%d]: ",j+1,progetti[i].rdim);
					scanf(" %s",progetti[i].rotoli_richiesti[j].rotolo_richiesto);
					for(k=0;k<RCount;k++){
						if(strcmp(progetti[i].rotoli_richiesti[j].rotolo_richiesto,inventario[k].codice_rotolo)==0){
							err=0;
						}
					}
					if(err!=0){
						co(4);
						printf("\t\tERRORE: Rotolo non trovato!\n");
						co(7);
					}
				}while(err!=0);
				err=1;
				do{
					printf("\t\tMetraggio richiesto (M^2): ");
					scanf(" %s",val);
					progetti[i].rotoli_richiesti[j].metraggio_richiesto=checkValFloat(val);
					if(progetti[i].rotoli_richiesti[j].metraggio_richiesto<=0){
						co(4);
						printf("\t\tERRORE: Valore non valido!\n");
						co(7);
					}
				}while(progetti[i].rotoli_richiesti[j].metraggio_richiesto<=0);
			}
		}else{
			do{
				printf("\tScarti richiesti: ");
				scanf(" %s",val);
				progetti[i].scarti_richiesti=checkValInt(val);
				if(progetti[i].scarti_richiesti<=0){
					co(4);
					printf("\tERRORE: Valore non valido!\n");
					co(7);
				}
			}while(progetti[i].scarti_richiesti<=0);
			do{
				printf("\tRotolo da cui prendere gli scarti: ");
				scanf(" %s",progetti[i].rotoli_richiesti[j].rotolo_richiesto);
				for(k=0;k<RCount;k++){
					if(strcmp(progetti[i].rotoli_richiesti[j].rotolo_richiesto,inventario[k].codice_rotolo)==0){
						err=0;
					}
				}
				if(err!=0){
					co(4);
					printf("\tERRORE: Rotolo non trovato!\n");
					co(7);
				}
			}while(err!=0);
			for(j=0;j<*PCount;j++){
				if(strcmp(progetti[i].rotoli_richiesti[0].rotolo_richiesto,inventario[j].codice_rotolo)==0){
					if(progetti[i].scarti_richiesti>inventario[j].scarti_utilizzabili){
						co(4);
						printf("\tImpossibile eseguire il progetto, non ci sono scarti a sufficenza!\n");
						co(7);
						return 0;
					}
				}
			}
			progetti[i].rotoli_richiesti[0].metraggio_richiesto=0;
			progetti[i].rdim=1;
		}
		do{
			printf("\tPaga del progetto: ");
			scanf(" %s",val);
			progetti[i].paga=checkValFloat(val);
			if(progetti[i].paga<=0){
				co(4);
				printf("\tERRORE: Valore non valido!\n");
				co(7);
			}
		}while(progetti[i].paga<=0);
		co(3);
		printf("\n\tCalcolo costo in corso...\n");
		co(7);
		progetti[i].costo_approssimato=calcolaCostoProgetto(i,RCount);
		printf("\tIl progetto ha un costo approssimato di %.2f euro\n\n",progetti[i].costo_approssimato);
		(*PCount)++;
		pausa("Continua...\n");
		return 0;
	}
}
// Funzione che calcola il costo di un progetto
float calcolaCostoProgetto(int dim,int PCount){
	int i,j,flag=0,g,m,a;
	float costo=0;
	for(i=0;i<PCount;i++){
		for(j=0;j<progetti[dim].rdim;j++){
			if(strcmp(inventario[i].codice_rotolo,progetti[dim].rotoli_richiesti[j].rotolo_richiesto)==0){
				inventario[i].utilizzo_previsto+=progetti[dim].rotoli_richiesti[j].metraggio_richiesto;
				do{
					if(inventario[i].utilizzo_previsto>inventario[i].quantita_disponibile){
						co(8);
						printf("\tIl rotolo %s non e' sufficente per questo progetto, procedo con l'acquisto...\n",inventario[i].codice_rotolo);
						if(flag==0){
							do{
								printf("\tInserisci la data di oggi (GG MM AAAA): ");
								scanf(" %d %d %d",&g,&m,&a);
								if(checkData(g,m,a)==1){
									co(4);
									printf("\tERRRORE: Data non valida!\n");
									co(8);
								}
							}while(checkData(g,m,a)==1);
							inventario[i].data_acquisto.g=g;
							inventario[i].data_acquisto.m=m;
							inventario[i].data_acquisto.a=a;
							flag=1;
						}
						inventario[i].quantita_disponibile+=inventario[i].rot.lunghezza*inventario[i].rot.larghezza;
						costo+=inventario[i].rot.costo;
						printf("\tProcedo con l'acquisto...\n");
						printf("\t(Costo: %.2f)\n",costo);
					}
				}while(inventario[i].utilizzo_previsto>inventario[i].quantita_disponibile);
			}
		}
	}
	co(7);
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
				inventario[j]=inventario[j+1];
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
	char val[10];
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
				if(stato<0){
					stato=0;
				}
				if(stato>7){
					stato=7;
				}
				for(j=0;j<8;j++){
                    if(stato==j){
						co(15);
					}else{
						co(8);
					}
					switch(j){
                        case 0:
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
						case 0:
							printf("Nuovo Codice: ");
							scanf(" %s",inventario[i].codice_rotolo);
							break;
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
							}while(inventario[i].rot.lunghezza <= 0);
							inventario[i].quantita_disponibile=inventario[i].rot.larghezza*inventario[i].rot.lunghezza;
							break;
						case 6:
							do{
								printf("Nuova Larghezza (cm): ");
								scanf(" %s",val);
								inventario[i].rot.larghezza=checkValFloat(val);
							}while(inventario[i].rot.larghezza <= 0);
							inventario[i].quantita_disponibile=inventario[i].rot.larghezza*inventario[i].rot.lunghezza;
							break;
						case 7:
							// float vecchio=inventario[i].rot.costo;
							do{
								printf("Nuovo Costo (euro): ");
								scanf(" %s",val);
								inventario[i].rot.costo=checkValFloat(val);
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
		int i=*RCount,g,m,a,err;
		char scelta='Y';
		char val[10];
		printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
		printf(" Menu Sartoria      |  Budget: %.2f euro\n",budget);
		printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
		printf("Nuovo rotolo:\n");
		printf("\tCodice Rotolo: ");
		scanf(" %s",inventario[i].codice_rotolo);							// Input codice rotolo
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
				co(4);
				printf("\t\tERRORE: Valore non valido!!\n");
				co(7);
			}
		}while(inventario[i].rot.lunghezza<=0);
		do{
			printf("\t\tLarghezza (cm): ");
			scanf(" %s",val);
			inventario[i].rot.larghezza=checkValFloat(val);					// Controllo che il valore sia valido
			if(inventario[i].rot.larghezza<=0){
				co(4);
				printf("\t\tERRORE: Valore non valido!!\n");
				co(7);
			}
		}while(inventario[i].rot.larghezza<=0);
		printf("\t\tCodice Fornitore: ");
		scanf(" %s",inventario[i].rot.codice_fornitura);					// Input coice del fornitore
		do{
			printf("\t\tCosto ");
			co(8);
			printf("(per l'intero rotolo)");
			co(7);
			printf(": ");
			scanf(" %s",val);
			inventario[i].rot.costo=checkValFloat(val);						// Controllo che il valore sia valido
			if(inventario[i].rot.costo<0){
				co(4);
				printf("\t\tERRORE: Valore non valido!!\n");
				co(7);
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
		budget-=inventario[i].rot.costo;
		inventario[i].rot.usura=0;
		do{
			printf("\n\tData di acquisto (GG MM AAAA): ");
			scanf(" %d %d %d",&g,&m,&a);
			err=checkData(g,m,a);
			if(err!=0){
				co(4);
				printf("\tData non valida!!\n");
				co(7);
			}
		}while(err!=0);
		inventario[i].data_acquisto.g=g;
		inventario[i].data_acquisto.m=m;
		inventario[i].data_acquisto.a=a;
		inventario[i].quantita_disponibile=inventario[i].rot.larghezza*inventario[i].rot.lunghezza;
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
void caricaInventario(int *RCount, int *PCount){
	int i,j;
	FInv=fopen(FILEINVENTARIO,"r");
	if(FInv==NULL){
		co(4);
		printf("Si e' verificato un'errore nell'apertura del file '%s'.\n",FILEINVENTARIO);
		co(7);
	}else{
		fscanf(FInv,"%d %f",RCount,&budget);
		for(i=0;i<*RCount;i++){
			fscanf(FInv,"%s %s %s %s %s %f %f %s %f %f %d %d %f %f %d",inventario[i].codice_rotolo,inventario[i].fornitore,inventario[i].rot.tipo_tessuto,inventario[i].rot.colore,inventario[i].rot.fantasia,&inventario[i].rot.lunghezza,&inventario[i].rot.larghezza,inventario[i].rot.codice_fornitura,&inventario[i].rot.costo,&inventario[i].rot.usura,&inventario[i].data_acquisto.g,&inventario[i].data_acquisto.m,&inventario[i].data_acquisto.a,&inventario[i].quantita_disponibile,&inventario[i].utilizzo_previsto,&inventario[i].scarti_utilizzabili);
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
		fscanf(FProg,"%d",PCount);
		for(i=0;i<*PCount;i++){
			fscanf(FProg,"%s %f %d %f %s %d %f",progetti[i].nome_progetto,&progetti[i].costo_approssimato,&progetti[i].mini,&progetti[i].scarti_richiesti,progetti[i].tipoCapo,&progetti[i].rdim,&progetti[i].paga);
			for(j=0;j<progetti[i].rdim;j++){
				fscanf(FProg,"%s %f",progetti[i].rotoli_richiesti[j].rotolo_richiesto,&progetti[i].rotoli_richiesti[j].metraggio_richiesto);
			}
		}
		co(2);
		printf("Caricati con successo i progetti!\n");
		co(7);
	}
	pausa("Continua...\n");
}
/*
Funzione che salva i dati du due file separati, uno per l'inventario e uno per i progetti
*/
void salvaInventario(int RCount, int PCount){
	int i,j;
	FInv=fopen(FILEINVENTARIO,"w");
	if(FInv==NULL){
		co(4);
		printf("Si e' verificato un'errore nell'apertura del file '%s'.\n",FILEINVENTARIO);
		co(7);
	}else{
		fprintf(FInv,"%d %f\n",RCount,budget);
		for(i=0;i<RCount;i++){
			fprintf(FInv,"%s %s %s %s %s %f %f %s %f %f %d %d %f %f %d\n",inventario[i].codice_rotolo,inventario[i].fornitore,inventario[i].rot.tipo_tessuto,inventario[i].rot.colore,inventario[i].rot.fantasia,inventario[i].rot.lunghezza,inventario[i].rot.larghezza,inventario[i].rot.codice_fornitura,inventario[i].rot.costo,inventario[i].rot.usura,inventario[i].data_acquisto.g,inventario[i].data_acquisto.m,inventario[i].data_acquisto.a,inventario[i].quantita_disponibile,inventario[i].utilizzo_previsto,inventario[i].scarti_utilizzabili);
		}
	}
	FProg=fopen(FILEPROGETTI,"w");
	if(FProg==NULL){
		co(4);
		printf("Si e' verificato un'errore nell'apertura del file '%s'.\n",FILEPROGETTI);
		co(7);
	}else{
		fprintf(FProg,"%d\n",PCount);
		for(i=0;i<PCount;i++){
			fprintf(FProg,"%s %f %d %f %s %d %f\n",progetti[i].nome_progetto,progetti[i].costo_approssimato,progetti[i].mini,progetti[i].scarti_richiesti,progetti[i].tipoCapo,progetti[i].rdim,progetti[i].paga);
			for(j=0;j<progetti[i].rdim;j++){
				fprintf(FProg,"%s %f\n",progetti[i].rotoli_richiesti[j].rotolo_richiesto,progetti[i].rotoli_richiesti[j].metraggio_richiesto);
			}
		}
	}
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
		s1=checkValInt(val);
		if(s1<=0 || s1 > 4){
			co(4);
			printf("ERRRORE: Scelta non valida!!\n");
			co(7);
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
				printf("\t5) Reset\n");
				break;
			case 4:
				return 41;
		}
		printf("Scelta: ");
		scanf(" %s",val);
		s2=checkValInt(val);
		if((s1==1 && (s2 <=0 || s2 > 3)) || (s1>=2 && (s2<=0 || s2 > 5))){
			co(4);
			printf("ERRORE: Scelta non valida!!\n");
			co(7);
			pausa("Continua...\n");
			system(CLEAR);
		}
	}while((s1==1 && (s2 <=0 || s2 > 3)) || (s1>=2 && (s2<=0 || s2 > 5)));
	system(CLEAR);
	return s1*10 + s2;
}
// Funzione che resetta il programma
void reset(int *RCount, int *PCount){
	budget=BUDGETINIZIALE;
	*RCount=0;
	*PCount=0;
}