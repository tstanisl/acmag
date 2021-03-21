#ifndef __UMIEJETNOSCI__

#define __UMIEJETNOSCI__

#include "glowny.h"

int wykrywaj_niewidzialnych(lpostac* postac,int np=-1);
bool ukryj_sie_w_cieniu(lpostac* postac,int np=-1);
bool skradaj_sie(lpostac* postac,int np=-1);
bool otworz_drzwi(lpostac *postac,int x, int y, int np=-1);
bool wywaz_drzwi(lpostac* postac, int x, int y, int np=-1);
void odpoczywaj(lpostac *postac);
bool idz_do(lpostac *lpostac,int xc, int yc, int np=-1);

#endif