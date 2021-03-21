#include "glowny.h"
void tworz_zmienne_globalne()
{
 bufor_z=new char*[ekran_dl+1];
 bufor_c=new char*[ekran_dl+1];
 for (int x=0;x<=ekran_dl;x++)
 {
  bufor_z[x]=new char[ekran_wys+1];
  bufor_c[x]=new char[ekran_wys+1];
 }
 Pnic.typ=0;Pnic.numer=0;Pnic.wyt=0;Pnic.x=0;Pnic.y=0;
 //for(int i;i<Lstat;i++) Pnikt.stat[i]=0;
 //for(int i;i<Lumiejetnosci;i++) Pnikt.umiejetnosc[i]=0;
 Pnikt.x=0;Pnikt.y=0;
 Pnikt.przedmioty=NULL;
}
void niszcz_zmienne_globalne()
{
 for (int x=0;x<=ekran_dl;x++)
 {
  delete[] bufor_z[x];
  delete[] bufor_c[x];
 }
 delete[] bufor_z;
 delete[] bufor_z;
 delete[] rasa;
 for (int i=0;i<15;i++)
  if (opis_npc[i]) {
   niszcz_liste(opis_npc[i]->przedmioty);
   delete opis_npc[i];
  }
 for (int i=0;i<11;i++) delete[] opis_przedmiotu[i];
 delete[] opis_przedmiotu;
 //drobne zwiechy
}
