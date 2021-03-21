#ifndef __WEJ_DANYCH__

#define __WEJ_DANYCH__

#include "glowny.h"

void ruch_gracza(char odp,int &x,int &y);
bool Hero_pobierz_kierunek(int &x, int &y);
bool gdzie(int &x, int &y, int zasieg,int tryb=0);
bool Hero_podaj_liczbe(int &ile,int max=32000);

int pisz_ekwipunek(int typ=-1);
int pisz_ekwipunek_postaci(lpostac* lpostac, int typ=-1);
void pisz_uzbrojenie();
void pisz_umiejetnosci();
void pisz_statystyki();

void dodaj_doswiadczenie(lpostac *postac, long int ile);
void awans_na_poziom(lpostac *postac);
long int ile_dosw( int poziom, Tpostac *postac);

void ustaw_statystyki();
void wybierz_rase();

void wczytaj_Hero();


#endif