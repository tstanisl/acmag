#ifndef __MAGIA__

#define __MAGIA__

#include "glowny.h"

void wczytaj_opis_czarow(char* sciezka);
bool rzuc_czar (lpostac *postac, int x, int y, int czar);
void usun_efekt(lpostac *postac, int rodzaj );
bool efekt_czaru_modyfikator(lpostac *postac, lczar *lczar);
bool efekt_czaru(lpostac *postac, lczar *czar);
bool efekt_czaru(lczar* lczar,int np=-1);
bool zakoncz_czar(lczar* czar,lpostac *postac );
bool zakoncz_czary(int rodzaj,lpostac *postac );
void usun_czar(lczar* czar,lpostac *postac);
void dodaj_czar( lpostac *cel, Tczar czar, int zrodlo=0 );
void dodaj_czar( Topis_przedmiotu& cel, Tczar czar);
void dodaj_czar( bool obszar[Mobszar][Mobszar], int x, int y, Tczar czar,int zrodlo=0);
void dodaj_czar( int np, Tczar czar, int zrodlo=0 );
void aktualizuj_czary(lpostac *postac);
void aktualizuj_czary(int np=-1);

#endif

