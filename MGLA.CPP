#include "glowny.h"
void rysuj_linie_mgly (int x_k,int y_k)
{
 float wzrok=float(Hero.wzrok);
 float dx=float(x_k-Hero.x)/wzrok,dy=float(y_k-Hero.y)/wzrok;
 wzrok*=wzrok;
 float x=dx,y=dy;
 bool koniec=false;
 while (!koniec)
 {
  if (nalezy(1,1,pietro[np_gr].dl,pietro[np_gr].sz,int(x)+Hero.x,int(y)+Hero.y))
  {
   if (!przezroczysty(pietro[np_gr].mapa[int(x)+Hero.x][int(y)+Hero.y])
      || x*x+y*y>wzrok) koniec=true;
   //if (x*x+y*y>wzrok) koniec=true;
   pietro[np_gr].mgla[int(x)+Hero.x][int(y)+Hero.y]=WIDOCZNE;
   x+=dx;
   y+=dy;
  }
  else koniec=true;
 }
}
/*
void generuj_mgle(int x1,int y1,int x2,int y2)
{
 int x,y;
 for(x=x1;x<=x2;x++)
  for(y=y1;y<=y2;y++)
   if (pietro[np_gr].mgla[x][y]==WIDOCZNE) pietro[np_gr].mgla[x][y]=ZNANE;
 int d;
 int wzrok=Hero.wzrok;
 //rysuj_linie_mgly(x_gr+wzrok,y_gr+wzrok-3);
 for (d=-wzrok;d<=wzrok;d++)
 {
  rysuj_linie_mgly(x_gr+wzrok,y_gr+d);
  rysuj_linie_mgly(x_gr-wzrok,y_gr+d);
  rysuj_linie_mgly(x_gr+d,y_gr-wzrok);
  rysuj_linie_mgly(x_gr+d,y_gr+wzrok);
 }
} */
void generuj_mgle(int x1,int y1,int x2,int y2)
{
 int x,y;
 for(x=x1;x<=x2;x++)
  for(y=y1;y<=y2;y++)
   if (pietro[np_gr].mgla[x][y]==WIDOCZNE) pietro[np_gr].mgla[x][y]=ZNANE;
 for(x=x1;x<=x2;x++)
  for(y=y1;y<=y2;y++)
   if ( odleglosc(x_gr,y_gr,x,y)<=Hero.wzrok )
    if ( czy_widzi(x_gr,y_gr,x,y) ) pietro[np_gr].mgla[x][y]=WIDOCZNE;
}
