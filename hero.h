#ifndef __HERO__

#define __HERO__

#include "glowny.h"

void tworz_Hero();
void Hero_smierc();

bool Hero_zaloz_uzbrojenie();
bool Hero_zdejmij_uzbrojenie();
bool Hero_idz_schodami();
bool Hero_podnies_przedmiot();
bool Hero_wyrzuc_przedmiot();
bool Hero_atak();
bool Hero_rzuc_czar(int czar);
bool Hero_wybierz_czar();
bool Hero_uzyj_przedmiotu();
bool Hero_wypij_miksture();
bool Hero_otworz_drzwi();
bool Hero_zamknij_drzwi();
bool Hero_wywaz_drzwi();
bool Hero_rozbroj_pulapke();
int Hero_test_percepcji();
bool Hero_ukryj_sie();
bool Hero_rozmawiaj();
bool Hero_handluj();
bool Hero_kradziez();
bool Hero_odpoczywaj();

#endif
