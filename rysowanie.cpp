#include "glowny.h"
inline void piszXY(int x,int y,int col,char text)
{
 gotoxy(x,y);
 textattr(col);
 putch(text);
}
void piszXY(int x,int y,int col,char* tekst,...)
{
 gotoxy(x,y);
 textattr(col);
 va_list vl;
 static char bufor[1000];
 va_start(vl, tekst);
 vsprintf(bufor, tekst, vl);
 va_end(vl);
 cprintf("%s",bufor);
}
void czysc_linie(int linia)
{
 if (linia<1 || linia>25) return;
 gotoxy(1,linia);
 clreol();
}
static int bufor_dl=0;
void czysc_info()
{
 textattr(15);
 for(int y=19;y<22;y++) czysc_linie(y);
 bufor_dl=0;
}
void info(char* tekst, ...)
{
 va_list vl;
 static char bufor[1000];
 va_start(vl, tekst);
 vsprintf(bufor, tekst, vl);
 va_end(vl);
 if (bufor_dl>160)
 {
   piszXY(1+bufor_dl%80,19+bufor_dl/80,15+7*16,"(dalej)");
   while( getch() != SPACJA );
   czysc_info();
 }
 *bufor=upcase(*bufor);// ustaw wielka litere na poczatku
 piszXY(1+bufor_dl%80,19+bufor_dl/80,15,bufor);
 bufor_dl+=strlen(bufor)+1;
}
void pisz_do_bufora(int x,int y,int col,char text)
{
 int x_w=x-Hero.x+ekran_dl/2,y_w=y-Hero.y+ekran_wys/2;
 if (nalezy(1,1,ekran_dl,ekran_wys,x_w,y_w))
 {
  bufor_z[x_w][y_w]=text;
  bufor_c[x_w][y_w]=col;
 }
}
void rysuj_mape(int np,int x1,int y1,int x2,int y2)
{
 int x,y;
 char pole;
 for(x=x1;x<=x2;x++)
  for(y=y1;y<=y2;y++)
  {
   pole=pietro[np].mapa[x][y];
   if (pietro[np].mgla[x][y]!=NIEZNANE)
    if (pietro[np].mgla[x][y]==ZNANE) pisz_do_bufora(x,y,8,znak_terenu(pole));
                      else pisz_do_bufora(x,y,kolor_teren(pole),znak_terenu(pole));
   else pisz_do_bufora(x,y,0,32);
  }
}
void rysuj_przedmioty(int np)
{
 int typ,x,y;
 lprzedmiot *obecny=pietro[np].przedmioty;
 while (obecny)
 {
  typ=obecny->przedmiot.typ;
  x=obecny->przedmiot.x;
  y=obecny->przedmiot.y;
 // if (abs(x)>70 || abs(y)>30)
 // { cprintf("cos nie tak p=%i t=%i x=%i y=%i",i,typ,x,y);getch(); };
  if (pietro[np].mgla[x][y]==WIDOCZNE)
     pisz_do_bufora(x,y,kolor_przedmiotu(obecny->przedmiot),kod_przedmiotu(typ));
  obecny=obecny->nast;
 }
}
void rysuj_pulapki(int np)
{
 int x,y;
 lpulapka *obecny=pietro[np].pulapki;
 while (obecny)
 {
  if (obecny->pulapka.znana)
  {
   x=obecny->pulapka.x;
   y=obecny->pulapka.y;
   if (pietro[np].mgla[x][y]==WIDOCZNE)
    pisz_do_bufora(x,y,kolor_teren(PULAPKA),znak_terenu(PULAPKA));
  }
  obecny=obecny->nast;
 }
}
void rysuj_wrogow(int np)
{
 int x,y;
 lpostac *obecny=pietro[np].npc;
 while (obecny)
 {
  if (!obecny->postac.niewidoczny)
  if (!obecny->postac.martwy)
  {
   x=obecny->postac.x;
   y=obecny->postac.y;
   if (pietro[np].mgla[x][y]==WIDOCZNE)
      pisz_do_bufora(x,y,obecny->postac.kolor,obecny->postac.znak);
  }
  obecny=obecny->nast;
 }
}
void rysuj_schody(int np)
{
 int x,y;
 lschody *obecny=pietro[np].schody;
 while (obecny)
 {
  x=obecny->schody.x;
  y=obecny->schody.y;
  if (pietro[np].mgla[x][y]!=NIEZNANE)
  {
   if (pietro[np].mgla[x][y]==WIDOCZNE) pisz_do_bufora(x,y,13,SCHODY);
                                   else pisz_do_bufora(x,y,8,SCHODY);
  }
  obecny=obecny->nast;
 }
}
void rysuj_czary(int np)
{
 int x,y;
 lczar *obecny=pietro[np].czary;
 while (obecny)
 {
  Tczar czar=obecny->czar;
  x=czar.p[0];
  y=czar.p[1];
  if (pietro[np].mgla[x][y]!=NIEZNANE)
   if (pietro[np].mgla[x][y]==WIDOCZNE)
    pisz_do_bufora(czar.p[0],czar.p[1],czar.p[2],czar.p[3]);
  obecny=obecny->nast;
 }
}
/*
void pisz_inf_tekst(int x,int y,int kol, char* tekst)
{
// piszXY(x,y,kol,tekst);
// clreol();
}*/
struct {
   char tekst[20];
   char kol;
} informacja[20];
int ile_informacji=0;
void dodaj_inf(int kol, char* tekst)
{
 strcpy(informacja[ile_informacji].tekst,tekst);
 informacja[ile_informacji].kol=kol;
 ile_informacji++;
}
void pisz_informacje_Hero()
{
  textattr(15);
  gotoxy(1,22);
  cprintf( " Imie : %s  numer_pi©tra=%i  obr - %i/%i mana - %i/%i  ",
          Hero.imie,np_gr,Hero.obr,Hero.max_obr,Hero.mana,Hero.max_mana);
  gotoxy(1,23);
  cprintf( " POZIOM = %i  dosw - %i z %i  KP=%i Ciezar Ekw=%g kg",
          Hero.poziom,Hero.dosw,(Hero.poziom)*1000,Hero.KP + premia(Hero.stat[1]),Hero.waga_ekwipunku);//,Hero.PD );
  int x=ekran_X+ekran_dl+1;
  ile_informacji=0;
  if (Hero.stan[STAN_ZATRUCIE]) dodaj_inf(2,"ZATRUCIE");
  if (Hero.niewidoczny) dodaj_inf(15,"NIEWIDOCZNY");
  if (Hero.stan[STAN_NIEPRZYTOMNY]) dodaj_inf(9,"NIEPRZYTOMNY");
  if (Hero.stan[STAN_UNIERUCHOMIENIE]) dodaj_inf(9,"UNIERUCHOMIENIE");
  if (Hero.stan[STAN_PRZYSPIESZENIE]) dodaj_inf(15,"PRZYSPIESZENIE");
  if (Hero.stan[STAN_SPOWOLNIENIE]) dodaj_inf(4,"SPOWOLNIENIE");
  if (Hero.stan[STAN_NIEWRAZLIWOSC]) dodaj_inf(9,"NIEWRAZLIWOSC");
  if (Hero.stan[STAN_TARCZA]) dodaj_inf(9,"TARCZA");
  if (Hero.stan[STAN_SLEPOTA]) dodaj_inf(4,"SLEPOTA");
  if (Hero.stan[STAN_OSLABIENIE]) dodaj_inf(4,"OSLABIENIE");
  if (Hero.stan[STAN_BLOGOSLAWIENSTWO]) dodaj_inf(1,"BLOGOSLAWIENSTWO");
  if (Hero.stan[STAN_OCHRONA_SMIERC]) dodaj_inf(9,"OCHRONA_SMIERC");
  if (Hero.stan[STAN_PSALM]) dodaj_inf(1,"PSALM");
  if (Hero.stan[STAN_OCHRONA_STRACH]) dodaj_inf(9,"OCHRONA_STRACH");
  if (Hero.stan[STAN_REGENERACJA]) dodaj_inf(9,"REGENERACJA");
  for (int y=0;y<17;y++)
  {
   gotoxy(x,y+2);
   if (y<ile_informacji) piszXY(x,y+2,informacja[y].kol,informacja[y].tekst);
   textattr(0);//chowa cursor
   clreol();
  }
}
void generuj_bufor()
{
 int x1=max(Hero.x-ekran_dl/2,1),
     y1=max(Hero.y-ekran_wys/2,1),
     x2=min(Hero.x+ekran_dl/2+1,pietro[np_gr].dl),
     y2=min(Hero.y+ekran_wys/2+1,pietro[np_gr].sz);
 generuj_mgle(x1,y1,x2,y2);
 rysuj_mape(np_gr,x1,y1,x2,y2);
 rysuj_przedmioty(np_gr);
 rysuj_schody(np_gr);
 rysuj_pulapki(np_gr);
 rysuj_czary(np_gr);
 rysuj_wrogow(np_gr);
 bufor_z[ekran_dl/2][ekran_wys/2]=Hero.znak;
 bufor_c[ekran_dl/2][ekran_wys/2]=Hero.kolor;
}
void rysuj_bufor()
{
 int x1=x_gr-ekran_dl/2,y1=y_gr-ekran_wys/2;//,x2=x_gr+20,y2=y_gr+10;
 int x,y;
 /*To dziala szybciej niz na poczatku*/
 static char bufor[2*ekran_dl*ekran_wys];
 int poz=0;
 for(y=1;y<=ekran_wys;y++) {
  poz+=2;
  for(x=1;x<=ekran_dl;x++) {
   bufor[poz]=' ';
   bufor[poz+1]=15;
   if (nalezy(1,1,pietro[np_gr].dl,pietro[np_gr].sz,x1+x,y1+y)) {
    bufor[poz]=bufor_z[x][y];
    bufor[poz+1]=bufor_c[x][y];
   }
   else {
    bufor[poz]=32;
    bufor[poz+1]=0;
   }
   poz+=2;
  }
 }
 puttext(ekran_X,ekran_Y+1,ekran_X+ekran_dl,ekran_Y+ekran_wys,bufor);
 textattr(15);
 pisz_informacje_Hero();
}

#include <windows.h>

int ScreenGetChar (char &znak, char &attr, int x, int y)
{
    int i, j, n;
    SMALL_RECT r={x, y, x, y};
    CHAR_INFO buffer[25][80];
    COORD a1={80,25},a2={0,0};
    ReadConsoleOutput (GetStdHandle (STD_OUTPUT_HANDLE),
      (PCHAR_INFO) buffer, a1,a2, &r);
    znak= buffer[0][0].Char.AsciiChar;
    attr= buffer[0][0].Attributes;
    return 1;
}
void animacja (int xp, int yp, int xc, int yc, int znak, int kolor,int op)
{
 generuj_bufor();rysuj_bufor();
 float d=sqrt( float((xc-xp)*(xc-xp)+(yc-yp)*(yc-yp)));
 if (d==0) return;
 if (d<1) d=1;
 float dx=float(xc-xp)/d,dy=float(yc-yp)/d;
 float x=0,y=0;
 int sx,sy,xe,ye;
 char //xep=ekran_dl/2+ekran_X+xp-x_gr,
     //yep=ekran_wys/2+ekran_Y+yp-y_gr,
     kp,zp;
 bool koniec=false;
 while (!koniec)
 {
  if (nalezy(1,1,pietro[np_gr].dl,pietro[np_gr].sz,int(x)+xp,int(y)+yp))
  {
   sx=(int)x+xp;
   sy=(int)y+yp;
   if ( !przezroczysty(pietro[np_gr].mapa[sx][sy]) ||
       ( sx==xc && sy==yc)) koniec=true;
   if (pietro[np_gr].mgla[sx][sy]==WIDOCZNE)
   {
     xe=ekran_dl/2+ekran_X+sx-x_gr;
     ye=ekran_wys/2+ekran_Y+sy-y_gr;
     if (nalezy(ekran_X,ekran_Y,ekran_X+ekran_dl,ekran_Y+ekran_wys,xe,ye))
     {
      ScreenGetChar(zp, kp, xe-1, ye-1);
     // zp=bufor_z[sx][sy];
      //kp=bufor_c[sx][sy];
      piszXY(xe,ye,kolor,znak);
      delay(op);
      piszXY(xe,ye,kp,zp);
      //xep=xe;yep=ye;

     }
   }
   x+=dx;
   y+=dy;
  }
  else koniec=true;
 }
 rysuj_bufor();
}
void rysuj_obszar(bool obszar[Mobszar][Mobszar], int x, int y, int kolor, char znak)
{
 generuj_bufor();//rysuj_bufor();
 int xw,yw,xe,ye;
 for (int i=0;i<Mobszar;i++)
  for (int j=0;j<Mobszar;j++)
   if (obszar[i][j])
   {
     xw=x+i-Mobszar/2;
     yw=y+j-Mobszar/2;
     if ( nalezy(1,1,pietro[np_gr].dl,pietro[np_gr].sz,xw,yw) )
      if (pietro[np_gr].mgla[xw][yw]==WIDOCZNE)
      {
        xe=ekran_dl/2+ekran_X+xw-x_gr;
        ye=ekran_wys/2+ekran_Y+yw-y_gr;
        if ( nalezy(ekran_X,ekran_Y,ekran_X+ekran_dl,ekran_Y+ekran_wys,xe,ye) )
           piszXY(xe,ye,kolor,znak);
      }
   }
}
void rysuj_obszar(int *obszar, int dl, int kolor, char znak, int op)
{
 int xw,yw,xe,ye;
 for (int i=0;i<dl;i++)
 {
   xw=obszar[2*i];
   yw=obszar[2*i+1];
   if ( nalezy(1,1,pietro[np_gr].dl,pietro[np_gr].sz,xw,yw) )
    if (pietro[np_gr].mgla[xw][yw]==WIDOCZNE)
    {
      xe=ekran_dl/2+ekran_X+xw-x_gr;
      ye=ekran_wys/2+ekran_Y+yw-y_gr;
      if ( nalezy(ekran_X,ekran_Y,ekran_X+ekran_dl,ekran_Y+ekran_wys,xe,ye) )
         piszXY(xe,ye,kolor,znak);
    }
   delay(op);
 }
}
