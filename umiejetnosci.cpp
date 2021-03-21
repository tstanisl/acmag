#include "glowny.h"

//UMIEJETNOSCI
int wykrywaj_niewidzialnych(lpostac* postac,int np)
{
 if (np<0) np=np_gr;
 lpostac* obecny=lHero;
 if (postac==lHero) obecny=pietro[np].npc;
 int x,y,ile=0;
 while (obecny)
 {
  if ( obecny->postac.niewidoczny )
  if ( !obecny->postac.stan[STAN_NIEWIDZIALNOSC] )
  {
   x=obecny->postac.x;
   y=obecny->postac.y;
   if (pietro[np_gr].mgla[x][y]==WIDOCZNE)
   {
    int d=int( odleglosc(Hero.x,Hero.y,x,y) );
    if (d<Hero.wzrok)
     {
      if ( (los(1,20) + Hero.umiejetnosc[UM_POSZUKIWANIE] -
           obecny->postac.umiejetnosc[UM_SKRADANIE] )
           > (15+d) )
      {
       obecny->postac.niewidoczny=false;
       ile++;
      }
     }
   }
  }
  if (obecny==lHero) obecny=pietro[np].npc;
                else obecny=obecny->nast;
 }
 return ile;
}
bool ukryj_sie_w_cieniu(lpostac* postac,int np)
{
 if (np<0) np=np_gr;
 lpostac* obecny=lHero;
 if (postac==lHero) obecny=pietro[np].npc;
 while (obecny)
 {
  int odl=odleglosc( obecny->postac.x, obecny->postac.y,
                 postac->postac.x, postac->postac.y );
  if ( czy_widzi ( obecny->postac.x, obecny->postac.y,
                   postac->postac.x, postac->postac.y )
                   && odl<obecny->postac.wzrok) return false;
  if ( (los(1,20) + premia_u(obecny->postac.umiejetnosc[UM_POSZUKIWANIE])) >
       (premia_u(postac->postac.umiejetnosc[UM_SKRADANIE])+odl) ) return false;

  if (obecny==lHero) obecny=pietro[np].npc;
                else obecny=obecny->nast;
 }
 postac->postac.niewidoczny=true;
 return true;
}
bool skradaj_sie(lpostac* postac,int np)
{
 if (np<0) np=np_gr;
 lpostac* obecny=lHero;
 if (postac==lHero) obecny=pietro[np].npc;
 while (obecny)
 {
  if ( (los(1,20) + premia_u(obecny->postac.umiejetnosc[UM_POSZUKIWANIE])) >
       (5+premia_u(postac->postac.umiejetnosc[UM_SKRADANIE])) )
       if ( czy_widzi ( obecny->postac.x, obecny->postac.y,
                        postac->postac.x, postac->postac.y ) )
                        return false;
  if (obecny==lHero) obecny=pietro[np].npc;
                else obecny=obecny->nast;
 }
 postac->postac.niewidoczny=true;
 return true;
}
bool otworz_drzwi(lpostac *postac,int x, int y, int np)
{
 if (np<0) np=np_gr;
 ldrzwi *drzwi=znajdz_drzwi(x,y,2);
 if (drzwi->drzwi.zablokowane) return false;
 postac->postac.PD-=100;
 if ( (los(1,20)+premia(postac->postac.umiejetnosc[UM_ZAMKI]) )
      > drzwi->drzwi.trudnosc )
 {
  otworz_drzwi(drzwi);
  drzwi->drzwi.zablokowane=true;
  return true;
 }
 else return false;
}
bool wywaz_drzwi(lpostac* postac, int x, int y, int np)
{
 if (np<0) np=np_gr;
 ldrzwi *drzwi=znajdz_drzwi(x,y,2);
 postac->postac.PD-=100;
 if ( (los(1,20)+premia(postac->postac.stat[0])) > drzwi->drzwi.wyt )
 {
  otworz_drzwi(drzwi);
  drzwi->drzwi.zablokowane=true;
  return true;
 }
 else return false;
}
void odpoczywaj(lpostac *postac)
{
 int do_mana=los(0,max(1,min(los(10,20),postac->postac.umiejetnosc[8]/2))),
     do_obr=los(0,max(1,premia(postac->postac.stat[2])));
 postac->postac.mana=min(postac->postac.mana+do_mana,postac->postac.max_mana);
 postac->postac.obr=min(postac->postac.obr+do_obr,postac->postac.max_obr);
 postac->postac.PD-=100;
}
bool idz_do(lpostac *lpostac,int xc, int yc, int np) {
 if (np<0) np=np_gr;
 lpostac->postac.x=xc;
 lpostac->postac.y=yc;
 lpostac->postac.PD-=PD_terenu(pietro[np].mapa[xc][yc]);
 //uruchom jakas fajna procedure np obr od ognia w lawie
 //toniecie w wodzie
}
