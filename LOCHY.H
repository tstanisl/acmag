#ifndef __LOCHY__

#define __LOCHY__

#include "glowny.h"

void wczytaj_opis_terenu(char* sciezka);
void czysc_mape(char wzor);
void tworz_lochy(int n);
void wczytaj_pietro(int n,char *sciezka);
void tworz_pietro(int n, int dlugosc, int szerokosc, int typ=-1);
void niszcz_pietro(int np);
void niszcz_lochy();

#endif
