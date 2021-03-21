#include "glowny.h"
void ruch_gracza(char odp,int &x,int &y)
{
 int x_n=x,y_n=y;
 switch (odp) {
   case 49:x_n--;y_n++;break;
   case 80:
   case 50:y_n++;break;
   case 51:x_n++;y_n++;break;
   case 75:
   case 52:x_n--;break;
   case 77:
   case 54:x_n++;break;
   case 55:x_n--;y_n--;break;
   case 72:
   case 56:y_n--;break;
   case 57:y_n--;x_n++;break;
  }
 x=x_n;
 y=y_n;
}
bool gdzie(int &xh, int &yh, int zasieg, int tryb)
{
 int x=0,y=0;
 if (tryb==1)
 {
  lpostac *wrog=znajdz_najblizszego_widocznego_wroga(lHero) ;
  //cprintf("najblizszy wrog to %s ",wrog->postac.imie);getch();
  if (wrog)
   if (pietro[np_gr].mgla[wrog->postac.x][wrog->postac.y]==WIDOCZNE)
   {
    x=wrog->postac.x-Hero.x;
    y=wrog->postac.y-Hero.y;
   }
 }
 int xp,yp;
 if (zasieg>1)
 {
  char odp=0;
  int kolor=15;
  bool ok=true;
  zasieg++;
//  generuj_bufor();
  rysuj_bufor();
  piszXY(ekran_dl/2+ekran_X+x,ekran_wys/2+ekran_Y+y,kolor,"X");
  do
  {
   odp=getch();
   xp=x;
   yp=y;
   ruch_gracza(odp,x,y);
   kolor=15;
   if ( !nalezy(1,1,pietro[np_gr].dl,pietro[np_gr].sz,x+x_gr,y+y_gr) )
   {
     x=xp;
     y=yp;
   }
   y=max(y,-ekran_wys/2+1);
   y=min(y,ekran_wys/2+1);
   x=max(x,-ekran_dl/2+1);
   x=min(x,ekran_dl/2+1);
   if ((x!=xp || y!=yp))
   {
    if ( pietro[np_gr].mgla[x_gr+x][y_gr+y]!=WIDOCZNE)//(x*x+y*y) >= (zasieg*zasieg+1))
    {
     x=xp;
     y=yp;
    }
   // generuj_bufor();
    rysuj_bufor();
    piszXY(ekran_dl/2+ekran_X+x,ekran_wys/2+ekran_Y+y,kolor,"X");
   }
   if (odp==ESC) return false;
   if (odp==ENTER && pietro[np_gr].mgla[x_gr+x][y_gr+y]==WIDOCZNE)
   {
    xh=x_gr+x;
    yh=y_gr+y;
   // generuj_bufor();
    rysuj_bufor();
//    cprintf("gdzie2 ");rysuj_przedmioty(np_gr);
    return true;
   }
//   cprintf("gdzie1 ");rysuj_przedmioty(np_gr);
  } while (true);
 }
}
bool Hero_pobierz_kierunek(int &x, int &y)
{
 rysuj_bufor();
 piszXY(1,1,15,"Gdzie ? (78946123)");
 int xp=x,yp=y;
 ruch_gracza(getch(),x,y);
 //printf("(%i, %i) ",x,y);getch();
 if (xp==x && yp==y)
 {
  piszXY(1,1,15,"Niewˆa˜ciwy klawisz.");
  return false;
 }
 return true;
}
bool Hero_podaj_liczbe(int &ile,int max)
{
 int x=wherex(),y=wherey();
 //ScreenGetCursor(&x,&y);
 char liczba[20]="";
 char znak=0;
 int dl=0;
 while (znak!=ENTER && znak!=ESC)
 {
  znak=getch();
  if (nalezy('0','9',upcase(znak)) && dl<20)
  {
   dl++;
   piszXY(x+dl,y,15,"%c",znak);
   liczba[dl-1]=znak;
  }
  if (znak==BACK && dl>0)
  {
   piszXY(x+dl,y,15," ");
   liczba[dl-1]=' ';
   dl--;
  }
 }
 if (znak==ESC) return false;
 ile=atoi(liczba);
 if (ile==0)
  if (znak==ENTER)
  {
   ile=max;
   return true;
  }
  else return false;
 return true;
}
int pisz_ekwipunek(int typ)
{
 if (!Hero.przedmioty)
 {
  piszXY(1,1,15,"Nie masz ¾adnych przedmiot¢w.");
//  getch();
//  piszXY(1,1,15,"                             ");
  return 0;
 };
 Topis_przedmiotu opisP;
 Tprzedmiot P;
 clrscr();
 textattr(15);//printf("Przedmioty");getch();
 lprzedmiot *obecny=Hero.przedmioty;
 int i=0,j;
 while (obecny)
 {
  //printf("Przedmioty");getch();
  P=obecny->przedmiot;
  opisP=opis(P);
  if (typ<0 || P.typ==typ)
  {
   gotoxy(2,i+2);
   cprintf( "%c)",i+65);
   if (opisP.tworzy_stos && P.ilosc>0) cprintf(" %i x", P.ilosc);
   cprintf(" %s", opisP.nazwa);
   if (opisP.ma_ladunki) cprintf(" ( %i ˆadunk¢w )", P.ilosc);
/*   if (P.typ==7 && opisP.p[1]==5) cprintf(" %s ( %i ˆadunk¢w )", opisP.nazwa,P.wyt);
    else if (P.ilosc>1) cprintf(" %i x %s", P.ilosc, opisP.nazwa);
     else cprintf(" %s", opisP.nazwa);*/
   i++;
  }
  obecny=obecny->nast;
 }
// getch();
 return i;
}
int pisz_ekwipunek_postaci(lpostac* lpostac, int typ)
{
// if (!lpostac) lpostac=lHero;
 Tpostac postac=lpostac->postac;
 if (!postac.przedmioty)
 {
  piszXY(1,1,15,"%s nie ma ¾adnych przedmiot¢w.",postac.imie);
  return 0;
 };
 Topis_przedmiotu opisP;
 Tprzedmiot P;
 clrscr();
 textattr(15);//printf("Przedmioty");getch();
 lprzedmiot *obecny=postac.przedmioty;
 int i=0,j;
 while (obecny)
 {
  //printf("Przedmioty");getch();
  P=obecny->przedmiot;
  opisP=opis(P);
  if (typ<0 || P.typ==typ)
  {
   gotoxy(2,i+2);
   cprintf( "%c)",i+65);
   if (opisP.tworzy_stos && P.ilosc>0) cprintf(" %i x", P.ilosc);
   cprintf(" %s", opisP.nazwa);
   if (opisP.ma_ladunki) cprintf(" ( %i ˆadunk¢w )", P.ilosc);
/*   if (P.typ==6 /*|| (P.typ==7 && opisP.p[1]!=5)) cprintf(" %i x %s", P.wyt, opisP.nazwa);
    else if (P.typ==7 && opisP.p[1]==5) cprintf(" %s ( %i ˆadunk¢w )", opisP.nazwa,P.wyt);
     else cprintf(" %s", opisP.nazwa);*/
   i++;
  }
  obecny=obecny->nast;
 }
// getch();
 return i;
}
/*
int pisz_ekwipunek_postaci(lpostac* lpostac, int typ=-1)
{
// if (!lpostac) lpostac=lHero;

 Tpostac postac=lpostac->postac;
 if (!postac.przedmioty)
 {
  piszXY(1,1,15,"%s nie ma ¾adnych przedmiot¢w.",postac.imie);
  return 0;
 };
 lprzedmiot **przed=new lprzedmiot*[i];
 Topis_przedmiotu opisP;
 Tprzedmiot P;
 clrscr();
 textattr(15);//printf("Przedmioty");getch();
 lprzedmiot *obecny=postac.przedmioty;
 int i=0,j;
 while (obecny)
 {
  //printf("Przedmioty");getch();
  P=obecny->przedmiot;
  opisP=opis(P);
  if (typ<0 || P.typ==typ)
  {
   gotoxy(2,i+2);
   cprintf( "%c)",i+65);
   if (P.typ==6 /*|| (P.typ==7 && opisP.p[1]!=5)) cprintf(" %i x %s", P.wyt, opisP.nazwa);
    else if (P.typ==7 && opisP.p[1]==5) cprintf(" %s ( %i ˆadunk¢w )", opisP.nazwa,P.wyt);
     else cprintf(" %s", opisP.nazwa);
   i++;
  }
  obecny=obecny->nast;
 }
// getch();
 return i;
} */
void pisz_uzbrojenie()
{
  clrscr();
  piszXY(2,1,15,"A) Gˆowa    :");
  piszXY(2,2,15,"B) Tuˆ¢w    :");
  piszXY(2,3,15,"C) Pr©ka    :");
  piszXY(2,4,15,"D) Lr©ka    :");
  piszXY(2,5,15,"E) Szyja    :");
  piszXY(2,6,15,"F) Pdˆoä    :");
  piszXY(2,7,15,"G) Ldˆoä    :");
  piszXY(2,8,15,"H) Amunicja :");
/*  piszXY(2,1,15,"A) Heˆm       :");
  piszXY(2,2,15,"B) Zbroja     :");
  piszXY(2,3,15,"C) Pr©ka      :");
  piszXY(2,4,15,"D) Lr©ka      :");
  piszXY(2,5,15,"E) Amulet     :");
  piszXY(2,6,15,"F) Lpier˜cieä :");
  piszXY(2,7,15,"G) Ppier˜cieä :");
  piszXY(2,8,15,"H) Amunicja   :");*/

//  piszXY(1,4,15,"F) R©kawice :");
//  piszXY(1,4,15,"F) Pas     :");
//  piszXY(1,4,15,"F) Buty     :");
  int i;
  for( i=0;i<Lnasobie;i++ ) //cprintf("t=%i  n=%i\n",Hero.nasobie[i].typ,Hero.nasobie[i].numer);
  {
    piszXY(16,i+1,15, opis_przedmiotu[Hero.nasobie[i].typ]
                      [Hero.nasobie[i].numer].nazwa);
    if (i==NS_AMMO)
     if (Hero.nasobie[NS_AMMO].typ>0)
      cprintf(" ( %i )",Hero.nasobie[NS_AMMO].ilosc);
  }
// getch();
}
void pisz_umiejetnosci()
{

  int z=0;
  char odp=0;
  clrscr();
  for (int i=0;i<6;i++) piszXY( 5, 2+i, 15, zdolnosci[i]);
  while (odp!=ESC)
  {
   piszXY(2,2+z,1,"  ");
   if (odp==GORA) z=max(0,z-1);
   if (odp==DOL)  z=min(5,z+1);
   piszXY(2,2+z,10,"->");
   int u1,u2;
   switch (z)
   {
     case 0: u1=UM_W_WRECZ; u2=UM_B_EGZOTYCZNE; break;
     case 1: u1=UM_M_SKUTECZNOSC; u2=UM_M_ALCHEMIA; break;
     case 2: u1=UM_LC_OCHRONA; u2=UM_LC_MODLITWY; break;
     case 3: u1=UM_SKRADANIE; u2=UM_POSZUKIWANIE; break;
     case 4: u1=UM_ELOKWENCJA; u2=UM_JEZYKI; break;
     case 5: u1=UM_NAPRAWA; u2=UM_PULAPKI; break;
/*     case 0: u1=0; u2=6; break;
     case 1: u1=7; u2=9; break;
     case 2: u1=10; u2=20; break;
     case 3: u1=21; u2=24; break;
     case 4: u1=25; u2=27; break;
     case 5: u1=28; u2=30; break;*/
   }
   for (int u=u1;u<=(u1+11);u++)
   {
    if (u>u2)
    {
     gotoxy(30,2+u-u1);
     clreol();
    }
    else piszXY( 30,2+u-u1,15,"%s : %i                 ",
                 umiejetnosci[u],Hero.umiejetnosc[u]);
   }
   odp=getch();
  }
}
void pisz_statystyki() //SF,ZR,WYT,INT,IN,CH,PS,WG,
{
  clrscr();
  piszXY(1,1,15,"Siˆa fizyczna  :");
  piszXY(1,2,15,"Zr©czno˜†      :");
  piszXY(1,3,15,"Wytrzymaˆo˜†   :");
  piszXY(1,4,15,"Inteligencja   :");
  piszXY(1,5,15,"Intuicja       :");
  piszXY(1,6,15,"Charyzma       :");
  textcolor(15);
  for(int i=0;i<Lstat;i++ ) //cprintf("t=%i  n=%i\n",Hero.nasobie[i].typ,Hero.nasobie[i].numer);
  {
   int kolor=15;
   if ( Hero.stat[i]>Hero.bazowe_stat[i] ) kolor=9;
   if ( Hero.stat[i]<Hero.bazowe_stat[i] ) kolor=4;
   piszXY(18,1+i,kolor,"%i",Hero.stat[i]);
   if ( kolor!=15 )
   {
    textattr(15);
    cprintf(" ( %i ) ",Hero.bazowe_stat[i]);
   }
  }
  piszXY(1,8 ,15,"Odp. na ciosy : %i",Hero.odpornosc_DR[OBR_NORMAL]);
  piszXY(1,9 ,15,"Odp. na magie : %i",Hero.odpornosc_DR[OBR_MAGIA]);
  piszXY(1,10,15,"Odp. na ogien : %i",Hero.odpornosc_DR[OBR_OGIEN]);
  piszXY(1,11,15,"Odp. na mroz  : %i",Hero.odpornosc_DR[OBR_MROZ]);
  piszXY(1,12,15,"Odp. na kwas  : %i",Hero.odpornosc_DR[OBR_KWAS]);
  piszXY(1,13,15,"Odp. na elekt : %i",Hero.odpornosc_DR[OBR_ELEKT]);
  piszXY(1,14,15,"Rzuty  Wytrzymaˆo˜† : %i   Uniki : %i    Siˆa Woli : %i  ",
                         Hero.rzut[0],       Hero.rzut[1], Hero.rzut[2]);

   //  getch();
}

void dodaj_doswiadczenie(lpostac *cel, long int ile)
{
 if (!cel) return;
 int nowy_poziom=0;
 int p;//=postac.poziom;
 Tpostac *postac=&(cel->postac);
 postac->dosw+=ile;
 while (nowy_poziom<=postac->dosw)
 {
  p=postac->poziom;
  if (p>30) return;
  nowy_poziom=p*1000;
  //cprintf("Test doscwiadczenie %i %i.  ",postac.dosw,nowy_poziom);
  if ( postac->dosw>=nowy_poziom )
  {
   postac->dosw=nowy_poziom;
   awans_na_poziom(cel);
  }
 }
}
void awans_na_poziom(lpostac *postac)
{
 Hero.poziom++; ///Hmmmmm...
 oblicz_postac(lHero);
 Hero.obr=Hero.max_obr;
 Hero.mana=Hero.max_mana;
// for (int i=0;i<Lumiejetnosci;i++) Hero.umiejetnosc[i]++;
 clrscr();
 piszXY(5,1,15,"Awansowaˆe˜ na poziom %i.    ",Hero.poziom);
 int punkty[6];
 punkty[0] = (2*Hero.bazowe_stat[1] + Hero.bazowe_stat[0]) / 9 ;//maks 6
 punkty[1] = Hero.bazowe_stat[3]/6;
// punkty[2] = (Hero.bazowe_stat[3] + Hero.bazowe_stat[4] - 1) / 7;
 {
  int p1=(Hero.bazowe_stat[3]-10)/3,
      p2=(Hero.bazowe_stat[4]-10)/3;
  if (Hero.bazowe_stat[3]<11) p1=0;
  if (Hero.bazowe_stat[4]<11) p2=0;
  punkty[2] = p1 + p2;
 }
 punkty[3] = (2*Hero.bazowe_stat[1] + Hero.bazowe_stat[4] - 6) / 16;
 punkty[4] = ( Hero.bazowe_stat[3] + Hero.bazowe_stat[4] + 2 * Hero.bazowe_stat[5] - 12) / 20;
 punkty[5] = (Hero.bazowe_stat[1] + Hero.bazowe_stat[3] - 3) / 11;
    for(int i=0;i<6;i++)
   {
    int kolor=15;
    if (punkty[i]<1)
    {
     punkty[i]=0;
     kolor=8;
    }
    piszXY(4,3+i,kolor,zdolnosci[i]);
    piszXY(21,3+i,kolor,": %i  ",punkty[i]);
   }
 int j=0;
 int nowa_umiejetnosc[Lumiejetnosci];
 for (int i=0;i<Lumiejetnosci;i++)
 {
  nowa_umiejetnosc[i]=Hero.bazowe_umiejetnosc[i];
  Hero.umiejetnosc[i]=Hero.bazowe_umiejetnosc[i];
  //printf("Hum[%i]=%i ",i,Hero.bazowe_umiejetnosc[i]);getch();
 }
 /*(
 for (int i=0;i<Lumiejetnosci;i++)
 {
  printf("Hum[%i]=%i ",i,Hero.bazowe_umiejetnosc[i]);getch();
 }*/
 int z=0,u=0;
 char odp=0;
// clrscr();
// for (int i=0;i<6;i++) piszXY( 5, 2+i, 15, zdolnosci[i]);
 enum {W_zd,W_um} tryb=W_zd;
 int u1=0,u2;
 bool koniec=false;
 while (!koniec)
 {
   switch (tryb)
   {
    case W_zd:
     if (odp==GORA) z=max(0,z-1);
     if (odp==DOL)  z=min(5,z+1);
    break;
    case W_um:
     switch (odp)
     {
      case GORA: u=max(u1,u-1);break;
      case DOL:  u=min(u2,u+1);break;
      case PLUS:
       if (punkty[z]>0)
        if (nowa_umiejetnosc[u]<(Hero.umiejetnosc[u]+2))
        {
         nowa_umiejetnosc[u]++;
         punkty[z]--;
        }
      break;
      case MINUS:
       if (nowa_umiejetnosc[u]>Hero.umiejetnosc[u])
       {
        nowa_umiejetnosc[u]--;
        punkty[z]++;
       }
      break;
     }
    break;
   }
   if (odp==PRAWO)
   {
    tryb=W_um;
    u=u1;
   }
   if (odp==LEWO)
   {
    tryb=W_zd;
    piszXY(27,3+u-u1,10,"  ");
   }
   if (odp==ENTER)
   {
        int s=0;
        for (int i=0;i<6;i++) s+=punkty[i];
        if (s==0)//Udznacz aby dzialalo jak trzeba
        {
         for (int i=0;i<Lumiejetnosci;i++) Hero.bazowe_umiejetnosc[i]=nowa_umiejetnosc[i];
         koniec=true;
        }
    }
//    if (odp==ESC) koniec=true;
    switch (z)
    {
     case 0: u1=UM_W_WRECZ; u2=UM_B_EGZOTYCZNE; break;
     case 1: u1=UM_M_SKUTECZNOSC; u2=UM_M_ALCHEMIA; break;
     case 2: u1=UM_LC_OCHRONA; u2=UM_LC_MODLITWY; break;
     case 3: u1=UM_SKRADANIE; u2=UM_POSZUKIWANIE; break;
     case 4: u1=UM_ELOKWENCJA; u2=UM_JEZYKI; break;
     case 5: u1=UM_NAPRAWA; u2=UM_PULAPKI; break;
    }
   for (int i=u1;i<=(u1+11);i++)
   {
    if (i>u2)
    {
     gotoxy(30,3+i-u1);
     clreol();
    }
    else
    {
     int kolor=8;
     if (nowa_umiejetnosc[i]<(Hero.umiejetnosc[i]+2)) kolor=15;
     piszXY( 30,3+i-u1,kolor,"%s : %i                 ",
                 umiejetnosci[i],nowa_umiejetnosc[i]);
    }
   }
   if (tryb==W_um) piszXY(27,3+u-u1,10,"->");
              else piszXY(2,3+z,10,"->");
   for(int i=0;i<6;i++)
   {
    int kolor=15;
    if (punkty[i]<1) kolor=8;
    piszXY(23,3+i,kolor,"%i  ",punkty[i]);
   }
   if (!koniec) odp=getch();
   if (tryb==W_um) piszXY(27,3+u-u1,10,"  ");
              else piszXY(2,3+z,1,"  ");
 }
 int pc=Hero.stat[3]-10; /// maksymalny poziom czarow
 pc=min( (Hero.poziom + 1) / 2, pc );
// info("%i",pc);getch();
 for (int i=0;i<Lczar;i++)
  if ( opis_czaru[i].poziom<=pc &&
      (Hero.bazowe_umiejetnosc[10+opis_czaru[i].lista]+1)/2>=opis_czaru[i].poziom ) ksiega_czarow[i]=1;
 for (int i=0;i<Lumiejetnosci;i++)
     Hero.umiejetnosc[i]=Hero.bazowe_umiejetnosc[i];
}
/*
void ustaw_statystyki()
{
 textattr(15);
 clrscr();
 int Xmenu=10,
     Ymenu=3;

 piszXY(Xmenu+15,Ymenu,15, " MNO½NIK DO—WIADCZENIA :");
 piszXY(Xmenu,Ymenu,  15, " STATYSTYKI ");
 piszXY(Xmenu,Ymenu+1,15, " Siˆa fizyczna :");
 piszXY(Xmenu,Ymenu+2,15, " Zr©czno˜†     :");
 piszXY(Xmenu,Ymenu+3,15, " Wytrzymaˆo˜†  :");
 piszXY(Xmenu,Ymenu+4,15, " Inteligencja  :");
 piszXY(Xmenu,Ymenu+5,15, " Intuicja      :");
 piszXY(Xmenu,Ymenu+6,15, " Charyzma      :");

 piszXY(Xmenu,Ymenu+8,15, " Percepcja      :");
 piszXY(Xmenu,Ymenu+9,15, " Punkty obra¾eä :");
 piszXY(Xmenu,Ymenu+10,15," Punkty many    :");

 int kursor=1,
     pozostalo=0;
 bool koniec=false;
 char odp;
 for(int i=0;i<Lstat;i++)
 Hero.stat[i]=10+i%2+rasa[Hero.rasa].stat[i];
// Hero.stat[i]=max(1,los(3,1,6)+rasa[Hero.rasa].stat[i]);
 Hero.poziom=1;
 while (!koniec)
 {
  piszXY(Xmenu-1,Ymenu+kursor,9,"*");
  piszXY(Xmenu+40,Ymenu,15,"%i ",pozostalo);
  for (int i=0;i<Lstat;i++)
  {
    gotoxy(Xmenu+17,Ymenu + 1 + i);
    cprintf("%i ",Hero.stat[i]);
  }
  for(int i=0;i<Lstat;i++) Hero.bazowe_stat[i]=Hero.stat[i];
  oblicz_postac(lHero);
  piszXY(Xmenu+17,Ymenu + 8,15,"%i ",Hero.wzrok);
  piszXY(Xmenu+17,Ymenu + 9,15,"%i ",Hero.max_obr);
  piszXY(Xmenu+17,Ymenu + 10,15,"%i ",Hero.max_mana);
  piszXY(Xmenu,Ymenu + 11,15, " Rzuty :   Wytrzymaˆo˜† : %i   Uniki : %i   Siˆa Woli : %i  ",
                              Hero.rzut[0], Hero.rzut[1], Hero.rzut[2]);
  piszXY(Xmenu,Ymenu + 14,15,"Naci˜nij R aby ponownie losowa†.");
  odp=getch();
  switch (odp)
  {
    case GORA  : piszXY(Xmenu-1,Ymenu+kursor,9," ");
                 kursor= max (1,kursor-1);
               break;
    case DOL   : piszXY(Xmenu-1,Ymenu+kursor,9," ");
                 kursor= min (Lstat,kursor+1);
               break;
    case PLUS  : if (pozostalo>0 && Hero.stat[kursor-1] < (18 + rasa[Hero.rasa].stat[kursor-1]) )
                 {
                    Hero.stat[kursor-1]++;
                    pozostalo--;
                 }
               break;
    case MINUS : if ( Hero.stat[kursor-1] > max (1,1+rasa[Hero.rasa].stat[kursor-1]))
                 {
                    Hero.stat[kursor-1]--;
                    pozostalo++;
                 }
               break;
    case ENTER : if (pozostalo==0)
                 koniec=true;
               break;
    case 'R'   : for(int i=0;i<Lstat;i++)
                  Hero.stat[i]=max(1,los(3,1,6)+rasa[Hero.rasa].stat[i]);
                 pozostalo=0;
               break;
  }
 }
 for(int i=0;i<Lstat;i++) Hero.bazowe_stat[i]=Hero.stat[i];
}*/
int premia_dosw(int n)
{
// int wyj=100;
 if (n>40) return 20*n-400;
 if (n>20) return 10*n;
 if (n>=0) return 100+5*n;
 //if (n>=-20)
 return 100+2*n;
// if (n>=-40) return 120+2*n;
 return 240+5*n;
}
/*
int premia_dosw(int n)
{
// int wyj=100;
 if (n>40) return 20*n-400;
 if (n>20) return 10*n;
 if (n>=0) return 100+5*n;
 if (n>=-20) return 100+n;
 if (n>=-40) return 120+2*n;
 return 240+5*n;
}
*/
void ustaw_statystyki()
{
 textattr(15);
 clrscr();
 int Xmenu=10,
     Ymenu=3;

 piszXY(Xmenu+15,Ymenu,15, " MNO½NIK DO—WIADCZENIA :");
 piszXY(Xmenu,Ymenu,  15, " STATYSTYKI ");
 piszXY(Xmenu,Ymenu+1,15, " Siˆa fizyczna :");
 piszXY(Xmenu,Ymenu+2,15, " Zr©czno˜†     :");
 piszXY(Xmenu,Ymenu+3,15, " Wytrzymaˆo˜†  :");
 piszXY(Xmenu,Ymenu+4,15, " Inteligencja  :");
 piszXY(Xmenu,Ymenu+5,15, " Intuicja      :");
 piszXY(Xmenu,Ymenu+6,15, " Charyzma      :");

 piszXY(Xmenu,Ymenu+8,15, " Percepcja      :");
 piszXY(Xmenu,Ymenu+9,15, " Punkty obra¾eä :");
 piszXY(Xmenu,Ymenu+10,15," Punkty many    :");

 int kursor=1,
     pozostalo=0;
 bool koniec=false;
 char odp;
 for(int i=0;i<Lstat;i++)
 Hero.stat[i]=10+i%2+rasa[Hero.rasa].stat[i];
// Hero.stat[i]=max(1,los(3,1,6)+rasa[Hero.rasa].stat[i]);
 Hero.poziom=1;
 while (!koniec)
 {
  piszXY(Xmenu-1,Ymenu+kursor,9,"*");
  piszXY(Xmenu+40,Ymenu,15,"%i %",premia_dosw(pozostalo));
  for (int i=0;i<Lstat;i++)
  {
    gotoxy(Xmenu+17,Ymenu + 1 + i);
    cprintf("%i ",Hero.stat[i]);
  }
  for(int i=0;i<Lstat;i++) Hero.bazowe_stat[i]=Hero.stat[i];
  oblicz_postac(lHero);
  piszXY(Xmenu+17,Ymenu + 8,15,"%i ",Hero.wzrok);
  piszXY(Xmenu+17,Ymenu + 9,15,"%i ",Hero.max_obr);
  piszXY(Xmenu+17,Ymenu + 10,15,"%i ",Hero.max_mana);
  piszXY(Xmenu,Ymenu + 11,15, " Rzuty :   Wytrzymaˆo˜† : %i   Uniki : %i   Siˆa Woli : %i  ",
                              Hero.rzut[0], Hero.rzut[1], Hero.rzut[2]);
  piszXY(Xmenu,Ymenu + 14,15,"Naci˜nij R aby zresetowa†.");
  odp=upcase(getch());
  switch (odp)
  {
    case GORA  : piszXY(Xmenu-1,Ymenu+kursor,9," ");
                 kursor= max (1,kursor-1);
               break;
    case DOL   : piszXY(Xmenu-1,Ymenu+kursor,9," ");
                 kursor= min (Lstat,kursor+1);
               break;
    case PLUS  : if (Hero.stat[kursor-1] < (18 + rasa[Hero.rasa].stat[kursor-1]) )
                 {
                    Hero.stat[kursor-1]++;
                    pozostalo--;
                 }
               break;
    case MINUS : if ( Hero.stat[kursor-1] > max (1,3+rasa[Hero.rasa].stat[kursor-1]))
                 {
                    Hero.stat[kursor-1]--;
                    pozostalo++;
                 }
               break;
    case ENTER : koniec=true;
               break;
    case 'R'   : for(int i=0;i<Lstat;i++)
                  Hero.stat[i]=10+i%2+rasa[Hero.rasa].stat[i];
                 pozostalo=0;
               break;
  }
 }
 for(int i=0;i<Lstat;i++) Hero.bazowe_stat[i]=Hero.stat[i];
 Hero.mnoznik_dosw=float(premia_dosw(pozostalo))/100;
// printf("MD=%f ",Hero.mnoznik_dosw);getch();
}
void wybierz_rase()
{
 clrscr();
 piszXY(20,2,15,"Wybierz ras© :");
 piszXY(22,3,15,"1 - czˆowiek");
 piszXY(22,4,15,"2 - krasnolud");
 piszXY(22,5,15,"3 - elf");
 piszXY(22,6,15,"4 - nizioˆek");
 char odp;
 Hero.rasa=0;
 do
 {
  odp=getch();
  switch(odp)
  {
   case '1': Hero.rasa=3;break;
   case '2': Hero.rasa=4;break;
   case '3': Hero.rasa=7;break;
   case '4': Hero.rasa=10;break;
  }
 } while (Hero.rasa==0);
}
void wczytaj_Hero()
{
 clrscr();
 struct ffblk lista[30];
 struct ffblk f;
 int ile=0;
 int koniec = findfirst("save/*.asv", &f,FA_RDONLY);
 if (koniec)
 {
  piszXY(20,1,15,"Nie masz ¾adnych zapisanych postaci :");
  getch();
  delay(1000);
  wyjscie_z_gry();
 }
 else piszXY(20,1,15,"Wybierz wcze˜niej zapisan¥ posta† :");
 lista[1]=f;
 while (!koniec)
 {
  ile++;
  piszXY(15,2+ile,15,"%c) %s",64+ile,lista[ile].ff_name);
  koniec = findnext(&f);
  lista[ile+1]=f;
  if (ile>20) koniec=1;
 }
 char odp=0;
 while (!nalezy('A','A'+ile-1,odp)) odp=upcase(getch());
 char sciezka[]="save/";
 strcat(sciezka,lista[odp-64].ff_name);
 wczytaj(sciezka);
}

