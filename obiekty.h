#ifndef __PRZEDMIOTY__

#define __PRZEDMIOTY__

#include "glowny.h"

void generuj_przedmioty(int n_pietra, int l_przed);
void niszcz_liste(lprzedmiot *lista);
void niszcz_liste(lczar *lista);
void niszcz_liste(lpostac *lista);
void niszcz_liste(ldrzwi *lista);
void niszcz_liste(lschody *lista);
void niszcz_liste(lpulapka *lista);
void niszcz_przedmioty(int n_pietra);
void dodaj_przedmiot ( Tprzedmiot przedmiot, lprzedmiot *lista );
void usun_przedmiot( lprzedmiot **przedmiot );
void wczytaj_opis_przedmiotow(char* sciezka);
int kolor_przedmiotu(Tprzedmiot &p);
char kod_przedmiotu(int typ);
Topis_przedmiotu opis(Tprzedmiot przedmiot);
int zlicz_przedmioty(int x,int y,int np=-1);
lprzedmiot* szukaj_w_ekwipunku(Tpostac *postac,int typ,int nr, int p1);

int zlicz_liste(lprzedmiot *lista);
int zlicz_liste(lpostac *lista);

void dodaj_pulapke( int np, Tpulapka pulapka);
void usun_z_pulapek( lpulapka *pulapka, int np=-1 );
lpulapka* znajdz_pulapke(int x,int y,int np=-1);
lpulapka *znajdz_najblizsza_pulapke(int x, int y, int tryb=0, int np=-1);
int efekt_pulapka(lpulapka *lpulapka,lpostac *cel=NULL);
void tworz_pulapke(int x, int y, Tpulapka pulapka,int np=-1);

void tworz_drzwi(int x, int y, Tdrzwi drzwi,int np=-1);
int otworz_drzwi(ldrzwi *drzwi, int np=-1);
int zamknij_drzwi(ldrzwi *drzwi, int np=-1);
void dodaj_drzwi( int np, Tdrzwi drzwi);
ldrzwi *znajdz_drzwi(int x, int y, int tryb=0, int np=-1);
ldrzwi *znajdz_najblizsze_drzwi(int x, int y, int tryb=0, int np=-1);

lschody *znajdz_wyjscie(lschody *wejscie);
int dodaj_schody( int np1, int x1, int y1, int np2, int x2, int y2, bool aktywne=true);
void tworz_schody(int np1, int np2, bool aktywne=true);
void tworz_schody(int np1, int np2, int x1, int y1, bool aktywne=true);



#endif

