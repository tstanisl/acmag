#include "glowny.h"
bool p0_z1()
{
 lpostac *burmistrz=szukaj_postaci(19,0);
 if (!burmistrz) return false;
 int x=burmistrz->postac.x,
     y=burmistrz->postac.y;
 Tlokacja l={x-1,y-1,x+1,y+1};
 if ( jest_postac(l,ID_HERO,0) )
 {
   info("Burmistrz m¢wi: Witaj W©drowcze ! Jestem Winnie... Mam zadanie dla ciebie. Prosz© ci© o zniszczenie zˆego demona kryj¥cego si© wewn¥trz jaskini.");
   return true;
 }
 return false;
}
bool p0_z2()
{
 pietro[0].flaga[0]=max(1,pietro[0].flaga[0]);
 pietro[0].schody->schody.aktywne=true;
 (znajdz_wyjscie(pietro[0].schody))->schody.aktywne=true;
 return true;
}
bool p4_z1()
{
 pietro[0].flaga[0]=max(4,pietro[0].flaga[0]);
 pietro[4].schody->schody.aktywne=true;
 (znajdz_wyjscie(pietro[4].schody))->schody.aktywne=true;
 return true;
}
bool p7_z1()
{
 pietro[0].flaga[0]=max(7,pietro[0].flaga[0]);
 pietro[7].schody->schody.aktywne=true;
 (znajdz_wyjscie(pietro[7].schody))->schody.aktywne=true;
 return true;
}
bool p10_z1()
{
 pietro[0].flaga[0]=max(10,pietro[0].flaga[0]);
 pietro[10].schody->schody.aktywne=true;
 (znajdz_wyjscie(pietro[10].schody))->schody.aktywne=true;
 return true;
}
bool postatni_z1()
{
 Tlokacja l={1,1,pietro[Lpieter-1].dl,pietro[Lpieter-1].sz};
 if ( !jest_postac(l,12,Lpieter-1) )
 {
  getch();
  clrscr();
  cprintf("   Brawo ! Balrog zostaˆ pokonany. Wygraˆes !!!!!");
  getch();
  delay(2000);
  wyjscie_z_gry();
  return true;
 }
 return false;
}
void dodaj_zdarzenie(int np, bool (*zdarzenie)() )
{
 lzdarzenie *stare=pietro[np].zdarzenia;
 pietro[np].zdarzenia=new lzdarzenie;
 pietro[np].zdarzenia->zdarzenie=zdarzenie;
 pietro[np].zdarzenia->nast=stare;
}
void usun_zdarzenie( lzdarzenie *zdarzenie, int np=-1 )
{
 if (np<0) np=np_gr;
 lzdarzenie *obecny=pietro[np].zdarzenia, *pop=obecny;
 if (obecny==zdarzenie) pietro[np].zdarzenia=pietro[np].zdarzenia->nast;
 {
  while (obecny)
  {
   if (obecny==zdarzenie) break;
   pop=obecny;
   obecny=obecny->nast;
  }
  pop->nast=obecny->nast;
 }
 delete obecny;
}
void ustaw_zdarzenia()
{
 dodaj_zdarzenie(0,p0_z1);
 dodaj_zdarzenie(4,p4_z1);
 dodaj_zdarzenie(7,p7_z1);
 dodaj_zdarzenie(10,p10_z1);
 dodaj_zdarzenie(Lpieter-1,postatni_z1);
}
void niszcz_zdarzenia(lzdarzenie *lista)
{
 lzdarzenie *obecny, *do_kasacji;
 obecny=lista;
 while(obecny)
 {
  do_kasacji=obecny;
  obecny=obecny->nast;
  delete do_kasacji;
 }
 lista=NULL;
}
void niszcz_zdarzenia(void)
{
 for(int i=0;i<Lpieter;i++) niszcz_zdarzenia(pietro[i].zdarzenia);
}
void aktualizuj_zdarzenia(int np)
{
 if (np<0) np=np_gr;
 lzdarzenie *nowy,*obecny=pietro[np].zdarzenia;
 while (obecny)
 {
  nowy=obecny->nast;
  if (obecny->zdarzenie)
   if ( (*obecny->zdarzenie)() ) usun_zdarzenie(obecny,np);
  //info("OK.");
  obecny=nowy;
 }
}


