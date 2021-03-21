//#include <conio.h>
#include <stdio.h>
#include "glowny.h"
//={"cos","kos","ros","rus"};

void generuj_przedmioty(int n_pietra,int l_przedmiotow)
{
 if (l_przedmiotow>100) return;
 int x,y;
 for(int i=0;i<l_przedmiotow;i++)
 {
  //pietro[n_pietra].przedmiot[i]=new TPrzedmiot();
  //pietro[n_pietra].przedmiot[i].numer=0;
  //przedmioty[i].nazwa="mlotek";
  //do {
   x=los(3,mapa_dl-3);
   y=los(3,mapa_dl-3);
  //} while(mapa[x][y]!=LOCHY);
 // pietro[n_pietra].przedmiot[i].x=x;
//  pietro[n_pietra].przedmiot[i].y=y;
 }
}
void wczytaj_opis_przedmiotow(char* sciezka)
{
 /*
 //printf("otwieram plik\n");
 //getch();
 FILE *in=fopen(sciezka,"r");
 //getch();
 //printf("plik otwarty\n");
 //getch();
 int l_typow,l_rodzajow;
 int p1,p2,p3,wyt;
 float masa;
 fscanf(in,"%i",&l_typow);
 //printf("czytam lt %i\n",l_typow);
 //getch();
 opis_przedmiotu=new Topis_przedmiotu*[l_typow];
 int i,j;
 for(i=0;i<l_typow;i++)
 {
  fscanf(in,"%i",&l_rodzajow);
  opis_przedmiotu[i]=new Topis_przedmiotu[l_rodzajow];
  //printf("l_rodz %i\n",l_rodzajow);
  for(j=0;j<l_rodzajow;j++)
  {
   fscanf(in,"\n%[^\n]",opis_przedmiotu[i][j].nazwa);
   //printf("\n %s \n",opis_przedmiotow[i][j].nazwa);
   fscanf(in,"\n%i %i %i %f %i",&p1,&p2,&p3,&masa,&wyt);
   //printf("\n %i %i %i %f %i",p1,p2,p3,masa,wyt);
   //getch();
   opis_przedmiotu[i][j].p1=p1;
   opis_przedmiotu[i][j].p2=p2;
   opis_przedmiotu[i][j].p3=p3;
   opis_przedmiotu[i][j].masa=masa;
   opis_przedmiotu[i][j].wyt=wyt;
  }
 }
 fclose(in);
 */
}

int kolor_przedmiotu(int typ)
{
 int kol;
 switch (typ) {
  case 1:kol=8;break;
  case 2:kol=2;break;
  case 3:kol=6;break;
  case 4:kol=1;break;
  default:kol=15;
 }
 return kol;
}
char kod_przedmiotu(int typ)
{
 int kod;
 switch (typ) {
  case 1:kod=18;break;
  case 2:kod=19;break;
  case 3:kod=20;break;
  case 4:kod=21;break;
  default:kod=17;
 }
 return kod;
}

