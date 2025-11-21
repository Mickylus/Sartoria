#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Legge i file Inventario.txt e Progetti.txt e stampa JSON minimale
// Prova ad aprire il file in working dir, altrimenti in parent dir

FILE *try_open(const char *name){
    FILE *f = fopen(name, "r");
    if(f) return f;
    char path[512];
    snprintf(path, sizeof(path), "../%s", name);
    f = fopen(path, "r");
    return f;
}

int main(int argc, char **argv){
    FILE *f;
    int RCount=0,PCount=0;
    float budget=0.0f;
    // Inventario
    f = try_open("Inventario.txt");
    if(!f){
        fprintf(stderr, "{" "\"error\":\"Inventario.txt non trovato\"}" );
        return 1;
    }
    if(fscanf(f, "%d %f", &RCount, &budget)!=2){
        fclose(f);
        fprintf(stderr, "{" "\"error\":\"Formato Inventario non valido\"}");
        return 1;
    }
    printf("{\"inventario\":[");
    for(int i=0;i<RCount;i++){
        char codice[64]={0}, fornitore[64]={0}, tipo[64]={0}, colore[64]={0}, fantasia[64]={0}, codice_f[64]={0};
        float lunghezza, larghezza, costo, usura, quantita, util;
        int g,m,a,scarti;
        if(fscanf(f, "%s %s %s %s %s %f %f %s %f %f %d %d %d %f %f %d", codice, fornitore, tipo, colore, fantasia, &lunghezza, &larghezza, codice_f, &costo, &usura, &g, &m, &a, &quantita, &util, &scarti) != 16){
            // formato non atteso: spezza
        }
        printf("{\"codice\":\"%s\",\"fornitore\":\"%s\",\"quantita\":%.2f}", codice, fornitore, quantita);
        if(i<RCount-1) printf(",");
    }
    fclose(f);
    printf("],\"progetti\":[");
    // Progetti
    f = try_open("Progetti.txt");
    if(!f){
        printf("]}");
        return 0;
    }
    if(fscanf(f, "%d", &PCount)!=1){
        printf("]}");
        fclose(f);
        return 0;
    }
    for(int i=0;i<PCount;i++){
        char nome[64]={0}, tipoCapo[64]={0};
        float costo_approssimato, paga, ricavi, valore;
        int mini, scarti_richiesti, rdim;
        if(fscanf(f, "%s %f %d %f %s %d %f %f %f", nome, &costo_approssimato, &mini, &scarti_richiesti, tipoCapo, &rdim, &paga, &ricavi, &valore) < 1){
            break;
        }
        printf("{\"nome\":\"%s\",\"valore\":%.2f,\"rotoli\":[", nome, valore);
        for(int j=0;j<rdim;j++){
            char codice[64]={0}; float q;
            if(fscanf(f, "%s %f", codice, &q) == 2){
                printf("{\"codice\":\"%s\",\"q\":%.2f}", codice, q);
                if(j<rdim-1) printf(",");
            }
        }
        printf("]}");
        if(i<PCount-1) printf(",");
    }
    fclose(f);
    printf("]}");
    return 0;
}
