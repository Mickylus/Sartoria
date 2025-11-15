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

int menu(int);							// Stampa il menu
int nuovoRotolo(int*);				// Aggiunge un nuovo rotolo
int modificaRotolo(int,char[]);			// Modifica un rotolo esistente
int eliminaRotolo(int*,char[]);			// Elimina un rotolo (azzera la scheda e diminusce la dimensione logica)
int nuovoProgetto(int*,int);			// Aggiunge un nuovo progetto
int modificaProgetto(int,char[],int);			// Modifica un progetto
int eliminaProgetto(int*,char[]);			// Elimina un progetto
float calcolaCostoProgetto(int,int);	// Calcola il costo progetto (Ogni volta che il rotolo finisce lo riacquista)
void mostraProgetti(int);			// Stampa i progetti
int avviaTaglio(int*);				// Avvia il taglio (rimuove i progetti in attesa)
void mostraTessuti(int,int);			// Mostra i tessuti
int controlloTessuti(int);			// Controlla i tessuti con usura troppo alta e ne propone la sostituzione
int rotazioneScorte(int);			// Ruota le scorte
// Should
float aumentoUsura();				// Aumenta l'usura (a ogni azione o a caso. ancora da decidere...)
void salvaInventario(int,int);		// Sotto forma di file .txt
void caricaInventario(int*,int*);	// e salva sia tessuti che progetti
void reset(int*,int*);				// reset inventario
void aggiorna(int,int);				// aggiorna i dati


// Main
int main(){
	int RCount=0,PCount=0,scelta,err,tasto;
	char val[10],filter[MAXSTRING];
	caricaInventario(&RCount,&PCount);
	system(CLEAR);
	do{
		scelta=menu(1);
		switch(scelta){
			case 11:
				err=nuovoRotolo(&RCount);											// Input nuovo rotolo
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
				printf("Inserisci il codice da cercare: ");							// Input codice da cercare
				scanf(" %s",filter);
				if(modificaRotolo(RCount,filter)==1){								// procedo con la modifica
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
				printf("Inserisci il codice da cercare: ");							// Input codice da cercare
				scanf(" %s",filter);
				if(eliminaRotolo(&RCount,filter)==1){								// Procedo con l'eliminazione
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
				if(RCount>0){
					if(nuovoProgetto(&PCount,RCount)==1){								// Input nuovo progetto
						co(4);
						printf("ERRORE: dimensione massima raggiunta!\n");
						co(7);
					}
				}else{
					co(4);
					printf("ERRORE: Non ci sono rotoli in magazzino!\n");
					co(7);
				}
				break;
			case 22:
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
				printf(" Menu Sartoria      |  Budget: %.2f euro\n",budget);
				printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
				printf("Inserisci il nome da cercare: ");							// Input nome da cercare
				scanf(" %s",filter);
				if(modificaProgetto(PCount,filter,RCount)==1){
					co(4);
					printf("Rotolo non trovato!\n");
					co(7);
					pausa("Continua...\n");
				}
				break;
			case 31:
				mostraTessuti(RCount,PCount);
				break;
			case 34:
				co(8);
				printf("Salvataggio in corso...\n");
				co(7);
				salvaInventario(RCount,PCount);										// Salvataggio manuale
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
					reset(&RCount,&PCount);											// Resetto il programma
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
		aggiorna(RCount,PCount);
		if(scelta != 41){
			system(CLEAR);		// Pulisco lo schermo
		}
		co(7);
	}while(scelta!=41);
	return 0;
}
void aggiorna(int RCount,int PCount){
	int i,j,k;
	for(i=0;i<RCount;i++){
		inventario[i].utilizzo_previsto=0;
		for(j=0;j<PCount;j++){
			if(progetti[j].mini!=1){
				for(k=0;k<progetti[i].rdim;k++){
					if(strcmp(inventario[i].codice_rotolo,progetti[j].rotoli_richiesti[k].rotolo_richiesto)==0){
						inventario[i].utilizzo_previsto+=progetti[j].rotoli_richiesti[k].metraggio_richiesto;
					}
				}
			}
		}
	}
}
/*
Funzione che modifica un progetto
1: se non è stato trovato
0: se è stato trovato
*/
int modificaProgetto(int dim,char filtro[],int RCount){
	int i,j,tasto=0,stato=0,f=1,err=1,k;
	char val[10];
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
											co(4);
											printf("\tERRORE: Rotolo non trovato!\n");
											co(7);
										}
									}while(err!=0);
									err=1;
									do{
										printf("\tMetraggio richiesto (M^2): ");
										scanf(" %s",val);
										progetti[i].rotoli_richiesti[j].metraggio_richiesto=checkValFloat(val);
										if(progetti[i].rotoli_richiesti[j].metraggio_richiesto<=0){
											co(4);
											printf("\tERRORE: Valore non valido!\n");
											co(7);
										}
									}while(progetti[i].rotoli_richiesti[j].metraggio_richiesto<=0);
								}
								progetti[i].costo_approssimato=calcolaCostoProgetto(i,dim);
								co(8);
								printf("\t(Costo: %.2f)\n",progetti[i].costo_approssimato);
								pausa("Continua...\n");
							}else{
								do{
									printf("Nuovi scarti: ");
									scanf(" %s",val);
									progetti[i].scarti_richiesti=checkValInt(val);
									if(progetti[i].scarti_richiesti<=0){
										co(4);
										printf("ERRORE: Valore non valido!\n");
										co(7);
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
									co(4);
									printf("ERRORE: Rotolo non trovato!\n");
									co(7);
								}
							}while(err!=0);
							break;
						case 4:
							do{
								printf("Nuova paga: ");
								scanf(" %s",val);
								progetti[i].paga=checkValFloat(val);
								if(progetti[i].paga<=0){
									co(4);
									printf("ERRORE: Valore non valido!\n");
									co(7);
								}
							}while(progetti[i].paga<=0);
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
void mostraTessuti(int dim,int pdim){
	int i,tasto=0,j,k;
	for(i=0;i<dim;i++){
		do{
			// Stampo tutti i campi
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
			// Attendo un input
			tasto=pausa("[<-] [->] Muoviti | [SPAZIO] Modifica | [ESC] Esci");
			// Se <- torno indietro di 1
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
			if(tasto==32){
				modificaRotolo(dim,inventario[i].codice_rotolo);
			}
			// Esco dalla funzione
			if(tasto==27){
				i=dim;
			}
		}while(tasto!=1003 && tasto != 1002 && tasto!=27);
	}
}
// Funzione che crea un nuovo progetto
int nuovoProgetto(int *PCount,int RCount){
	if(*PCount>=MAXPROGETTI){
		return 1;
	}else{
		int i=*PCount,j,k,scelta,err=1,tasto=0;
		char val[10];
		printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
		printf(" Menu Sartoria      |  Budget: %.2f euro\n",budget);
		printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
		printf("Nuovo progetto:\n");
		printf("\tNome progetto: ");
		scanf(" %s",progetti[i].nome_progetto);
		// Chiedo che tipo di progetto è
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
		if(scelta==2){
			progetti[i].mini=1;
		}else{
			progetti[i].mini=0;
		}
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
					printf("\tQuanti rotoli usa il progetto (MAX: %d): %d\n",MAXP,progetti[i].rdim);
					printf("\n\tRotoli:\n");
					printf("\t\tRotolo [%d/%d]\n",j+1,progetti[i].rdim);
					printf("\t\tCodice rotolo: ");
					co(15);
					printf("%s",inventario[k].codice_rotolo);
					co(7);
					tasto=pausa("\n");
					if(tasto==1002){
						if(k>0){
							k-=2;
						}else{
							k--;
						}
					}
					if(tasto==1003 && k==RCount-1){
						k--;
					}
					if(tasto==13){
						strcpy(progetti[i].rotoli_richiesti[j].rotolo_richiesto,inventario[k].codice_rotolo);
						k=RCount;
					}
				}
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
			// Se il progetto è 'mini' allora chiedo gli scarti che servono
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
		progetti[i].costo_approssimato=calcolaCostoProgetto(i,RCount);		// Calcolo il costo del progetto in base alle scorte attuali
		co(8);
		printf("\tIl progetto ha un costo approssimato di %.2f euro\n\n",progetti[i].costo_approssimato);
		co(7);
		printf("\tRicavi stimati: ");
		if(progetti[i].paga-progetti[i].costo_approssimato>=0){
			co(2);
		}else{
			co(4);
		}
		printf("%.2f ",progetti[i].paga-progetti[i].costo_approssimato);		// Stampo il ricavo
		co(7);
		printf("euro\n");
		(*PCount)++;
		pausa("Continua...\n");
		return 0;
	}
}
// Funzione che calcola il costo di un progetto
float calcolaCostoProgetto(int dim,int PCount){
	int i,j;
	float costo=0,q,u;
	for(i=0;i<PCount;i++){
		for(j=0;j<progetti[dim].rdim;j++){
			if(strcmp(inventario[i].codice_rotolo,progetti[dim].rotoli_richiesti[j].rotolo_richiesto)==0){
				u=progetti[dim].rotoli_richiesti[j].metraggio_richiesto;
				q=inventario[i].quantita_disponibile;
				do{
					if(u>q){
						q+=inventario[i].rot.lunghezza*inventario[i].rot.larghezza;
						costo+=inventario[i].rot.costo;	// Aumento il costo fino a quando ne ho abbastanza
					}
				}while(u>q);
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
						co(15);	// se il campo è quello scelto allora lo evidenzio
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
		inventario[i].quantita_disponibile=inventario[i].rot.larghezza*inventario[i].rot.lunghezza;
		co(8);
		printf("\t\tQuantita' disponibile: %.2f\n",inventario[i].quantita_disponibile);
		co(7);
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
		// leggo i tessuti
		fscanf(FInv,"%d %f",RCount,&budget);
		for(i=0;i<*RCount;i++){
			fscanf(FInv,"%s %s %s %s %s %f %f %s %f %f %d %d %d %f %f %d",inventario[i].codice_rotolo,inventario[i].fornitore,inventario[i].rot.tipo_tessuto,inventario[i].rot.colore,inventario[i].rot.fantasia,&inventario[i].rot.lunghezza,&inventario[i].rot.larghezza,inventario[i].rot.codice_fornitura,&inventario[i].rot.costo,&inventario[i].rot.usura,&inventario[i].data_acquisto.g,&inventario[i].data_acquisto.m,&inventario[i].data_acquisto.a,&inventario[i].quantita_disponibile,&inventario[i].utilizzo_previsto,&inventario[i].scarti_utilizzabili);
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
		// Salvo i tessuti
		fprintf(FInv,"%d %f\n",RCount,budget);
		for(i=0;i<RCount;i++){
			fprintf(FInv,"%s %s %s %s %s %f %f %s %f %f %d %d %d %f %f %d\n",inventario[i].codice_rotolo,inventario[i].fornitore,inventario[i].rot.tipo_tessuto,inventario[i].rot.colore,inventario[i].rot.fantasia,inventario[i].rot.lunghezza,inventario[i].rot.larghezza,inventario[i].rot.codice_fornitura,inventario[i].rot.costo,inventario[i].rot.usura,inventario[i].data_acquisto.g,inventario[i].data_acquisto.m,inventario[i].data_acquisto.a,inventario[i].quantita_disponibile,inventario[i].utilizzo_previsto,inventario[i].scarti_utilizzabili);
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
	3.5) Reset
 4) Uscita
	4.1) Termina il programma
*/
int menu(int mode){
	int s1,s2,tasto=0,stato=mode,i,j;
	char val[10];
	do{
		system(CLEAR);
		co(7);
		printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
		printf(" Menu Sartoria      |  Budget: %.2f euro\n",budget);
		printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
		printf("[SU/GIU] Muoviti, [INVIO] Seleziona\n");
		for(i=1;i<=4;i++){
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
		}else if(stato>4){
			stato=4;
		}	
		// Seleziono la scelta
		if(tasto==13){
			s1=stato;
		}
	}while(tasto!=13);
	stato=1;
	if(s1==4){
		return 41;
	}
	do{
		system(CLEAR);
		co(7);
		printf("- - - - - - - - - - - - - - - - - - - - - - - -\n");
		printf(" Menu Sartoria      |  Budget: %.2f euro\n",budget);
		printf("- - - - - - - - - - - - - - - - - - - - - - - -\n\n");
		printf("[SU/GIU] Muoviti, [INVIO] Seleziona, [ESC] Esci\n");
		for(j=1;j<5;j++){
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
					printf("\tEsci\n");
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
							}else{
								printf("\t\tMostra tessuti\n");
							}
							break;
						case 2:
							if(s1==1){
								printf("\t\tModifica rotolo\n");
							}else if(s1==2){
								printf("\t\tModifica progetto\n");
							}else{
								printf("\t\tControlla i tessuti\n");
							}
							break;
						case 3:
							if(s1==1){
								printf("\t\tElimina rotolo\n");
							}else if(s1==2){
								printf("\t\tElimina progetto\n");
							}else{
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