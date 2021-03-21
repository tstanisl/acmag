#ifndef __NPC__

#define __NPC__

#include "glowny.h"

void wczytaj_rasy(char* sciezka);
void wczytaj_opis_npc(char* sciezka);
void tworz_npc(int np, int n, int sila, Temocje emocje);
void tworz_npc(int np,int x, int y, int n, int sila, Temocje emocje);
void generuj_wrogow(int np,int n);
void niszcz_npc(int np);
bool jest_npc(int x,int y,int np=-1);
bool jest_postac(Tlokacja l,int id,int np=-1);
lpostac* znajdz_postac(int x, int y, int np=-1);
lpostac* znajdz_npc(int x,int y,int np=-1);
lpostac* szukaj_postaci(int n, int np=-1);
lpostac* szukaj_postaci(int n, Tlokacja l, int np=-1);
void dodaj_npc( int np,Tpostac npc);
void zabij_npc( int np,lpostac *npc);
void usun_npc( int np,lpostac *npc);
void pisz_npc();

void ai_npc(lpostac *npc, int np=-1);
void ai_wojownik(lpostac *lnpc, int np=-1);
void ai_lucznik (lpostac *lnpc, int np=-1);
void ai_kaplan(lpostac *lnpc, int np=-1);
void ai_mag(lpostac *lnpc, int np=-1);

bool ai_ruch_losowy( lpostac *lnpc,int np=-1);
void ai_odpoczywaj(lpostac *lnpc, int np=-1);
bool ai_idz_do(lpostac *lnpc, int xc, int yc,int np=-1);
bool ai_uciekaj_od(lpostac *lnpc, int xc, int yc,int np=-1);
bool ai_kraz_wokol(lpostac *lnpc, int xc, int yc,int np=-1);

#endif

