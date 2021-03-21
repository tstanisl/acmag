#include "glowny.h"

#define LOCHY_B 0
#define LOCHY_X 6
#define LOCHY_Y 4
#define LOCHY_DX 4
#define LOCHY_DY 4

void wczytaj_opis_terenu(char* sciezka)
{
 clrscr();
 printf("otwieram plik opis terenu\n");
 FILE *in=fopen(sciezka,"r");
 printf("plik otwarty\n");
 int i;
 Topis_terenu proznia={false,false,' ',0,0};
 for (i=0;i<Lteren;i++) opis_terenu[i]=proznia;
 while(1) {
  char nazwa[40];
  fscanf(in,"\n%i",&i); if(i<0) break;
  printf("\n teren %i",i);//getch();
  fscanf(in,"%[^\n]",nazwa); //dopisz \n na poczatku
  strcpy(opis_terenu[i].nazwa,nazwa);
  //getch();
  printf("\n nazwa: %s \n",opis_terenu[i].nazwa);
  fscanf(in,"\n");
  int pom,j;
  for(j=0;j<5;j++) {
   fscanf(in,"%i",&pom);
   printf(" op[%i]=%i",j,pom);
   switch(j) {
    case 0:opis_terenu[i].znak=pom;break;
    case 1:opis_terenu[i].kolor=pom;break;
    case 2:opis_terenu[i].przezroczysty=pom;break;
    case 3:opis_terenu[i].dozw_ruch=pom;break;
    case 4:opis_terenu[i].PD_ruch=pom;break;
   }
  }
  //getch();
 }
 fclose(in);
//   getch();
}

struct {
 int x,y;
 int dx,dy;
 bool czy_istnieje;
 bool czy_drzwi[4];
 int kor_drzwi[4];
 //    0
 //   3 1
 //    2
} loch[ LOCHY_X ][ LOCHY_Y ];

int budyn=0;
void generuj_pomieszczenie(int np, int x1, int y1, int x2, int y2, char znak)
{
 int x,y;
 for (x=x1;x<x2;x++)
  for (y=y1;y<y2;y++)
   pietro[np].mapa[x][y]=znak;
}
void generuj_tunel_poziomo(int np, int x1, int y1, int x2, int y2, char znak)
{
 int p,xs;
 if (x1>x2) { p=x1;x1=x2;x2=p;
              p=y1;y1=y2;y2=p; }
 xs=x1+(x2-x1)/2;
 int x,y;
 y=y1;
 for (x=x1;x<xs;x++) pietro[np].mapa[x][y]=znak;
 if (y2>y1) for (y=y1;y<y2;y++) pietro[np].mapa[x][y]=znak;
       else for (y=y1;y>y2;y--) pietro[np].mapa[x][y]=znak;
 y=y2;
 for (x=xs;x<=x2;x++) pietro[np].mapa[x][y]=znak;
}
void generuj_tunel_pionowo(int np, int x1, int y1, int x2, int y2, char znak)
{
 int p,ys;
 if (y1>y2) { p=x1;x1=x2;x2=p;
              p=y1;y1=y2;y2=p; }
 ys=y1+(y2-y1)/2;
 int x,y;
 x=x1;
 for (y=y1;y<ys;y++) pietro[np].mapa[x][y]=znak;
 y=ys;
 if (x2>x1) for (x=x1;x<x2;x++) pietro[np].mapa[x][y]=znak;
       else for (x=x1;x>x2;x--) pietro[np].mapa[x][y]=znak;
 x=x2;
 for (y=ys;y<=y2;y++) pietro[np].mapa[x][y]=znak;
}
void generuj_tunel(int np, int x1, int y1, int x2, int y2, char znak,bool poziomo,bool czy_sa_drzwi,char tlo)
{
 if (poziomo) generuj_tunel_poziomo( np, x1, y1, x2, y2, znak);
         else generuj_tunel_pionowo( np, x1, y1, x2, y2, znak);
 if (!czy_sa_drzwi) return;
 Tdrzwi drzwi={false,false,false, max( 0, 5+los(-15,15) ), 10+los(-8,8) };
 if (los(0,3)==0) drzwi.ukryte=true;
 if (los(0,1)) drzwi.otwarte=true;
 if (!drzwi.otwarte) if (los(0,1)) drzwi.zablokowane=true;
 if (los(0,1))
 {
  pietro[np].mapa[x1][y1]=tlo;
  tworz_drzwi(x1,y1,drzwi,np);
 }
 if (los(0,1))
 {
  pietro[np].mapa[x2][y2]=tlo;
  tworz_drzwi(x2,y2,drzwi,np);
 }
}
void tworz_tabele_pomieszczen(int np)
{
 int x,y,i,j;
 int dl=pietro[np].dl ,
     sz=pietro[np].sz ;
 float sx=dl/LOCHY_X,
       sy=sz/LOCHY_Y;
 int kx,ky;
 for (x=0;x<LOCHY_X;x++)
  for (y=0;y<LOCHY_Y;y++)
  {
   kx=int( sx*(float(x)+0.5) + 1)+los(-2,2);
   ky=int( sy*(float(y)+0.5) + 1)+los(-2,2);
   loch[x][y].dx=los(2,LOCHY_DX);//(int) sx-1);
   loch[x][y].dy=los(2,LOCHY_DY);//(int) sy-1);
   loch[x][y].x=kx-loch[x][y].dx/2;
   loch[x][y].y=ky-loch[x][y].dy/2;
   loch[x][y].czy_istnieje=true;
   for (i=0;i<4;i++) loch[x][y].czy_drzwi[i]=true;
   if (y==0) loch[x][y].czy_drzwi[0]=false;
   if (x==LOCHY_X) loch[x][y].czy_drzwi[1]=false;
   if (y==LOCHY_Y) loch[x][y].czy_drzwi[2]=false;
   if (x==0) loch[x][y].czy_drzwi[3]=false;
   loch[x][y].kor_drzwi[0]=loch[x][y].x+los(0,loch[x][y].dx-1);
   loch[x][y].kor_drzwi[2]=loch[x][y].x+los(0,loch[x][y].dx-1);
   loch[x][y].kor_drzwi[1]=loch[x][y].y+los(0,loch[x][y].dy-1);
   loch[x][y].kor_drzwi[3]=loch[x][y].y+los(0,loch[x][y].dy-1);
  }
}
void generuj_pomieszczenia(int np,char znak,char tlo,bool czy_sa_drzwi)
{
 int x1,x2,y1,y2;
 int x,y;
 for (x=0;x<LOCHY_X;x++)
  for (y=0;y<LOCHY_Y;y++)
  {
   x1=loch[x][y].x;
   x2=loch[x][y].x+loch[x][y].dx;
   y1=loch[x][y].y;
   y2=loch[x][y].y+loch[x][y].dy;
   generuj_pomieszczenie(np,x1,y1,x2,y2,znak);
  }
 //printf("generuje tunele.");getch();
 for (x=0;x < LOCHY_X ;x++)
  for (y=0;y < (LOCHY_Y-1) ;y++)
  {
   generuj_tunel( np,
                  loch[x][y].kor_drzwi[2],
                  loch[x][y].y+loch[x][y].dy ,
                  loch[x][y+1].kor_drzwi[0],
                  loch[x][y+1].y-1 ,
                  znak, false ,czy_sa_drzwi,tlo);
  }
 for (x=0;x < (LOCHY_X-1) ;x++)
  for (y=0;y < LOCHY_Y ;y++)
  {
   generuj_tunel( np,
                  loch[x][y].x+loch[x][y].dx,
                  loch[x][y].kor_drzwi[1] ,
                  loch[x+1][y].x-1 ,
                  loch[x+1][y].kor_drzwi[3] ,
                  znak, true ,czy_sa_drzwi,tlo);
  }

   //if (loch[x][y].czy_drzwi[0]) generuj_tunel(np,
}
int Nmax ( int max_n, int np )
{
 int out=int(float(np+1)/float(Lpieter)*float(max_n))+los(-1,1);
 //printf("%i ",out);
 return min(max_n,max(1,out));
}
void generuj_przedmioty(int np,int l_przedmiotow)
{
 int x,y;
 Tprzedmiot przedmiot;
 for(int i=0;i<l_przedmiotow;i++)
 {
  przedmiot.typ=los(1,10);
  switch (przedmiot.typ)
  {
    case 1 : przedmiot.numer=los(1,Nmax(15,np))-1;
           break;
    case 2 : przedmiot.numer=los(1,Nmax(11,np))-1;
           break;
    case 3 : przedmiot.numer=los(1,Nmax(12,np))-1;
           break;
    case 4 : przedmiot.numer=los(1,Nmax(5,np))-1;
           break;
    case 5 : przedmiot.numer=los(1,Nmax(5,np))-1;
           break;
    case 6 : przedmiot.numer=los(1,Nmax(8,np))-1;
           break;
    case 7 : przedmiot.numer=los(1,Nmax(10,np))-1;
           break;
    case 8  : przedmiot.numer=los(1,Nmax(6,np))-1;
            break;
    case 9  : przedmiot.numer=los(1,Nmax(8,np))-1;
            break;
    case 10 : przedmiot.numer=los(1,Nmax(5,np))-1;
            break;
  }
  przedmiot.wyt=opis_przedmiotu[przedmiot.typ][przedmiot.numer].wyt;
  przedmiot.ilosc=1;
  //przedmiot.ladunki=0;
  if (opis_przedmiotu[przedmiot.typ][przedmiot.numer].ma_ladunki)
     przedmiot.ilosc=los(2,8);
  if (przedmiot.typ==6) przedmiot.ilosc=los(5,15);
  int x,y;
  do
  {
   x=los(1,pietro[np].dl);
   y=los(1,pietro[np].sz);
  } while ( !dozw_ruch( pietro[np].mapa[x][y] ) );
//  przedmiot.x=x;
//  przedmiot.y=y;
  dodaj_do_przedmiotow(np,x,y,przedmiot);
 }
}

void generuj_pulapki(int np, int n)
{
//  pulapka.znana=false;
  int x,y;
  printf("Pulapki p%i :",np);
  for (int j=0;j<n;j++)
  {
   int rodzaj=los(1,Nmax(5,np));
   Tpulapka pulapka={true,rodzaj,los(1,3*rodzaj),los(1,2*rodzaj),false};
   do
   {
    x=los(1,pietro[np].dl);
    y=los(1,pietro[np].sz);
   } while ( !dozw_ruch(pietro[np].mapa[x][y]) );
   tworz_pulapke(x,y,pulapka,np);
   printf("%i ",rodzaj);
  }
  //getch();
}
void tworz_miasto()
{
 wczytaj_pietro(0,"mapy/base.map");
 pietro[0].czy_na_powierzchni=true;
 for(int j=0;j<10;j++) tworz_npc(0, 14, 2,OBOJETNY);//14
 for(int j=0;j<10;j++) tworz_npc(0, 18, 2,OBOJETNY);
 tworz_npc(0, 13, 2,OBOJETNY);
 tworz_npc(0, 45,4, 19,2,OBOJETNY);
 for(int j=0;j<3;j++) tworz_npc(0, 20, 2,OBOJETNY);
 tworz_npc(0, 21, 2,OBOJETNY);
 tworz_npc(0, 40,26,22, 2,OBOJETNY);
 //for(int j=0;j<3;j++) tworz_npc(0, 10, 3,WROGI);//14

}
void tworz_lochy( int n)
{
 pietro=new Tpietro[Lpieter];
 for (int i=0;i<Lpieter;i++)
 {
  pietro[i].czy_istnieje=false;
  for(int x=0;x<Lsil;x++)
   for(int y=0;y<Lsil;y++)
    pietro[i].ai_status[x][y]=0;
  for(int x=0;x<Lsil;x++) pietro[i].ai_status[x][x]=30;
  for(int x=0;x<Lflag;x++) pietro[i].flaga[x]=0;
 }
 /*KOD BITWY
 tworz_pietro( 0, 15,15, 2);//for(int i=0;i<1;i++)
// tworz_npc(0, 6, PRZYJAZNY );//5,WROGI);//

 for(int i=0;i<10;i++) tworz_npc(0, los(6,8), 1,PRZYJAZNY );//12-mala
 for(int i=0;i<4;i++) tworz_npc(0, los(11,11) , 2,WROGI);
 ustaw_ai_status(0,1,30,0);
 ustaw_ai_status(0,2,30,0);
 ustaw_ai_status(1,2,-30,0);*/
 ////Powierzchnia////////
// tworz_pietro( 0, 25, 20, 3);//mapa_dl, mapa_wys,typ_pietra);
// ustaw_ai_status(
 //generuj_przedmioty( 0, 8);
 //utworz_schody(n,n+1,true);
// for (int i=0;i<3;i++) tworz_npc(0, los(12,13), PRZYJAZNY );
 tworz_miasto();
 ustaw_ai_status(1,3,-20,0);
 ustaw_ai_status(1,2,30,0);
 for (int i=0;i<Lsil;i++)  ustaw_ai_status(i,Lsil-1,-30,0);
 for(int x=0;x<Lsil;x++) pietro[0].ai_status[x][x]=30;
 /////Tunele 1-6////////
 int typ_poziomu=0;
 for (int i=1;i<(Lpieter-1);i++)
 {
   if (i>=4) typ_poziomu=1;
   if (i>=7) typ_poziomu=2;
   if (i>=10) typ_poziomu=3;
   tworz_pietro( i, 60,40,typ_poziomu);
   pietro[i].czy_na_powierzchni=false;
//   tworz_pietro( i, 20,20,2);
   for(int j=0;j<30;j++) tworz_npc(i, los(max(0,i-2),min(11,i+1) ), 3,WROGI);
   generuj_przedmioty( i, los(20,50) );
   generuj_pulapki(i,los(4,8) );
   ustaw_ai_status(1,3,-20,i);
   for (int j=0;j<Lsil;j++)  ustaw_ai_status(j,Lsil-1,-30,i);
   for(int x=0;x<Lsil;x++)
    pietro[i].ai_status[x][x]=30;
 }
 //weze sie zepsuly
 ///LAS powierz2
 /*tworz_pietro( 7, mapa_dl, mapa_wys,3);
 for(int j=0;j<15;j++) tworz_npc(7, los(1,5), WROGI);
 generuj_przedmioty( 7, los(15,25));
 /////Tunele 8-10////////
 for (int i=8;i< (Lpieter-1) ;i++)
 {
   tworz_pietro( i, mapa_dl, mapa_wys,1);
   for(int j=0;j<6;j++) tworz_npc(i, los(max(0,i-2),min(9,i) ), WROGI);
   generuj_przedmioty( i, los(5,15));
 }*/
 /////Bitwa finaˆowa//////
 tworz_pietro( Lpieter-1, 30,20, 4);//mapa_dl, mapa_wys,typ_pietra);
 tworz_npc(Lpieter-1, 12, 3, WROGI);
 ustaw_ai_status(1,3,-20,Lpieter-1);
 for(int x=0;x<Lsil;x++) pietro[Lpieter-1].ai_status[x][x]=30;
// generuj_przedmioty( Lpieter-1, 20);*/
 ustaw_zdarzenia();
 for (int i=1;i<(Lpieter-1);i++) tworz_schody(i,i+1,true);
}
void wczytaj_pietro(int n,char *sciezka)
{
 printf("otwieram plik\n");
 FILE *in=fopen(sciezka,"r");
 printf("plik otwarty\n");
 int dl,sz;
 fscanf(in,"%i %i\n",&dl,&sz);
 printf("dl=%i sz=%i\n",dl,sz);
 tworz_pietro(n,dl,sz);
 char znak;
 for(int y=1;y<=sz;y++)
 {
  for(int x=1;x<=dl;x++)
  {
   fscanf(in,"%c",&znak);
   pietro[n].mapa[x][y]=znak_odkoduj(znak);
   printf("%c",pietro[n].mapa[x][y]);
  }
  fscanf(in,"\n");
  printf("\n");
 // getch();
 }
// getch();
 fclose(in);
}
void tworz_pietro(int n, int dlugosc, int szerokosc, int typ)
{
 strcpy(pietro[n].nazwa,"Pietro");
 pietro[n].dl=dlugosc;
 pietro[n].sz=szerokosc;
 pietro[n].mapa=new char*[dlugosc+1];
 pietro[n].mgla=new char*[dlugosc+1];
 for (int x=0;x<=dlugosc;x++)
 {
  pietro[n].mapa[x]=new char[szerokosc+1];
  pietro[n].mgla[x]=new char[szerokosc+1];
 }
 pietro[n].przedmioty=NULL;
 pietro[n].npc=NULL;
 pietro[n].schody=NULL;
 pietro[n].drzwi=NULL;
 pietro[n].pulapki=NULL;
 pietro[n].czary=NULL;
 pietro[n].zdarzenia=NULL;
 pietro[n].czy_istnieje=true;
 wypeln_tablice(pietro[n].mgla,dlugosc+1,szerokosc+1,NIEZNANE);
 if (typ<0) return;
 switch (typ)
 {

  case 0 : //STANDARDOWE LOCHY W SKALE
           wypeln_tablice(pietro[n].mapa,dlugosc+1,szerokosc+1,MUR);
           tworz_tabele_pomieszczen(n);
           generuj_pomieszczenia(n,LOCHY,MUR,true);
         break;
  case 1 : //JASKINIE
           wypeln_tablice(pietro[n].mapa,dlugosc+1,szerokosc+1,SKALA);
           tworz_tabele_pomieszczen(n);
           generuj_pomieszczenia(n,PIASEK,SKALA,true);
         break;
  case 2 : //MROCZNA JUNGLA
          {
           wypeln_tablice(pietro[n].mapa,dlugosc+1,szerokosc+1,JUNGLA);
           tworz_tabele_pomieszczen(n);
           generuj_pomieszczenia(n,TRAWA,JUNGLA,false);
           for(int x=1;x<=dlugosc;x++)
            for(int y=1;y<=szerokosc;y++) {
             if ( pietro[n].mapa[x][y]==TRAWA && los(1,100)<20 )
              pietro[n].mapa[x][y]=DRZEWO;
             if ( pietro[n].mapa[x][y]==TRAWA && los(1,100)<20 )
              pietro[n].mapa[x][y]=BAGNO;
            }  
          }
         break;
  case 3 : //STAROZYTNE CMENTARZYSKO
          {
           wypeln_tablice(pietro[n].mapa,dlugosc+1,szerokosc+1,MARMUR);
           tworz_tabele_pomieszczen(n);
           generuj_pomieszczenia(n,POPIOL,MARMUR,true);
           for(int x=1;x<=dlugosc;x++)
            for(int y=1;y<=szerokosc;y++)
             if ( los(1,100)<20 && pietro[n].mapa[x][y]==MARMUR) pietro[n].mapa[x][y]=CZASZKA;
          }
         break;
  case 4 : /// KOMANATA BALROGA ( KAMIENNA WYSPA NA JEZIORZE )
          {
           wypeln_tablice(pietro[n].mapa,dlugosc+1,szerokosc+1,WODA);
           int xp,yp,xs,ys,xk,yk;
           xs=dlugosc/2;ys=szerokosc/2;
           xp=xs-6;xk=xs+6;
           yp=ys-4;yk=ys+4;
           for (int x=xp;x<xk;x++)
            for (int y=yp;y<yk;y++)
             pietro[n].mapa[x][y]=LOCHY;
          }
         break;
  case 5 : /////  ¤KA
           wypeln_tablice(pietro[n].mapa,dlugosc+1,szerokosc+1,TRAWA);
         break;
  case 6 : /////  LAS
          {
           wypeln_tablice(pietro[n].mapa,dlugosc+1,szerokosc+1,TRAWA);
           for(int x=1;x<=dlugosc;x++)
            for(int y=1;y<=szerokosc;y++)
             if ( los(1,100)<20 ) pietro[n].mapa[x][y]=DRZEWO;
          }
         break;
  case 7 : //LODOWIEC
           wypeln_tablice(pietro[n].mapa,dlugosc+1,szerokosc+1,LODOWIEC);
           tworz_tabele_pomieszczen(n);
           generuj_pomieszczenia(n,SNIEG,LODOWIEC,false);
         break;

 }
}
void niszcz_pietro(int np)
{
 for (int x=0;x<=pietro[np].dl;x++)
 {
  delete[] pietro[np].mgla[x];
  delete[] pietro[np].mapa[x];
 }
 delete[] pietro[np].mgla;
 delete[] pietro[np].mapa;
 niszcz_liste (pietro[np].przedmioty) ;
 niszcz_liste (pietro[np].npc) ;
 niszcz_liste (pietro[np].czary) ;
 niszcz_liste (pietro[np].drzwi) ;
 niszcz_liste (pietro[np].pulapki) ;
 niszcz_liste (pietro[np].schody) ;
 niszcz_zdarzenia (pietro[np].zdarzenia);
}
void niszcz_lochy()
{
 clrscr();
 for(int i=0;i<Lpieter;i++)
 {
  printf ("Niszcze pietro - %i  \n",i);
  //getch();
  niszcz_pietro(i);
 }
 delete [] pietro;
}

