#ifndef __RYSOWANIE__

#define __RYSOWANIE__

#include "glowny.h"

#define SCHODY 62
void piszXY(int x,int y,int col,char text);
void piszXY(int x,int y,int col,char* text,...);
void czysc_linie(int linia);
void czysc_info();
void info(char* tekst, ...);
void generuj_bufor();
void pisz_informacje_Hero();
void rysuj_bufor();

void rysuj_przedmioty(int np);
void rysuj_wrogow(int np);

void animacja (int xp, int yp, int xc, int yc, int znak, int kolor,int op);
void rysuj_obszar(bool obszar[Mobszar][Mobszar], int x, int y, int kolor, char znak);
void rysuj_obszar(int *obszar, int dl, int kolor, char znak, int op);

#endif
