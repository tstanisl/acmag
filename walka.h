#ifndef __WALKA__

#define __WALKA__

#include "glowny.h"

void atak( lpostac *atakujacy, int np, int x, int y);
bool _obrazenia (int zrodlo, lpostac *postac, int rodzaj,int obr=-1);
int generuj_obszar( bool obszar[Mobszar][Mobszar], int x, int y, int rodzaj, int p[10]);
int generuj_obszar( int **obszar, int x, int y, int rodzaj, int p[10]);
int obszar_efekt(int x, int y, int rodzaj);
void obszar_efekt(int *obszar, int dl, int x, int y, int rodzaj);
int obszar_efekt(bool obszar[Mobszar][Mobszar], int x, int y, int rodzaj );
int atak_obszarowy (int zrodlo, int *obszar, int dl, int moc,void (*obr)(int,lpostac*,int,int*),int *p=NULL);
int atak_obszarowy( int zrodlo, bool obszar[Mobszar][Mobszar],int x, int y, int moc,void (*obr)(int,lpostac*,int,int*),int *p=NULL);
int atak_obszarowy (int zrodlo, int x, int y, int moc,void (*obr)(int,lpostac*,int,int*),int *p=NULL);
bool obrazenia  (int zrodlo, int x, int y, int rodzaj, int obr, int np=-1);
bool obrazenia  (int zrodlo, lpostac *postac, int rodzaj, int obr);
void normal_obr (lpostac *postac, int obr);
void magic_obr  (lpostac *postac, int obr);
void ogien_obr  (lpostac *postac, int obr);
void mroz_obr   (lpostac *postac, int obr);
void kwas_obr   (lpostac *postac, int obr);
void elekt_obr  (lpostac *postac, int obr);

#endif
