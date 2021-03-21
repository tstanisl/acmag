#include "glowny.h"

void wczytaj_rasy(char* sciezka)
{
// clrscr();
 printf("otwieram plik\n");
// getch();
 FILE *in=fopen(sciezka,"r");
// getch();
 printf("plik otwarty\n");
// getch();
 int l_ras;
 fscanf(in,"%i",&l_ras);
 //l_ras=3;
 printf("czytam lras=%i\n",l_ras);
 rasa=new Trasa[l_ras];
 int i,j;
 for(i=0;i<l_ras;i++)
 {
//  getch();
  printf("\n rasa %i",i);
  fscanf(in,"\n%[^\n]",rasa[i].nazwa);
//  getch();
  printf("\n %s \n",rasa[i].nazwa);
 // fscanf(in,"\n");
  for(j=0;j<Lstat;j++)
  {
   fscanf(in,"%i",&rasa[i].stat[j]);
   printf(" stat[%i]=%i",j,rasa[i].stat[j]);
//   getch();
  }
  fscanf(in,"\n%i %i",&rasa[i].szybkosc,&rasa[i].bazowe_dosw);
  printf(" szybkosc=%i  B_dosw=%i",rasa[i].szybkosc,rasa[i].bazowe_dosw);
 }
 fclose(in);
// getch();
}

void wczytaj_opis_npc(char* sciezka)
{
 clrscr();
 printf("otwieram plik opis NPC\n");
 //getch();
 FILE *in=fopen(sciezka,"r");
 //getch();
 printf("plik otwarty\n");
 //getch();
 int nr,i,j;
 for (i=0;i<Lnpc;i++) opis_npc[i]=NULL;
 while(1)//for(nr=0;nr<l_npc;nr++)
 {
  //getch();
  char imie[40];
  fscanf(in,"\n%i",&i); if(i<0) break;
  printf("\n npc %i",i);//getch();
  if (!opis_npc[i]) opis_npc[i]=new Tpostac;
   else break;
  fscanf(in," %[^\n]",imie); //dopisz \n na poczatku
  strcpy(opis_npc[i]->imie,imie);
  //getch();
  printf("\n imie: %s \n",opis_npc[i]->imie);
  fscanf(in,"\n");
  for (j=0;j<Lumiejetnosci;j++) opis_npc[i]->umiejetnosc[j]=0;
  for(j=0;j<5;j++)
  {
   fscanf(in,"%i",&opis_npc[i]->umiejetnosc[j]);
   printf(" umiej[%i]=%i",j,opis_npc[i]->umiejetnosc[j]);
   //getch();
  }
  fscanf(in,"\n%i %i %i %c %i", &opis_npc[i]->poziom,
                                &opis_npc[i]->rasa,
                                &opis_npc[i]->profesja,
                                &opis_npc[i]->znak,
                                &opis_npc[i]->kolor);
  printf(   "\n%i %i %i %c %i", opis_npc[i]->poziom,
                                opis_npc[i]->rasa,
                                opis_npc[i]->profesja,
                                opis_npc[i]->znak,
                                opis_npc[i]->kolor);
  fscanf(in,"\n");
  Topis_przedmiotu dane;
  for(j=0;j<Lnasobie;j++)
  {
   int typ,nr=0,ilosc=1;
   fscanf(in," %i",&typ);
   if (typ==0)
   {
    if (j==NS_P_REKA) fscanf(in," %i",&nr);
   }
   else
   {
    fscanf(in," %i",&nr);
    if (j==NS_AMMO) fscanf(in," %i",&ilosc);
   }
   Tprzedmiot przedmiot={typ,nr,ilosc};
   dane=opis(przedmiot);
   przedmiot.wyt=dane.wyt;
   opis_npc[i]->nasobie[j]=przedmiot;
   printf(" I (%i , %i) : ", opis_npc[i]->nasobie[j].typ,opis_npc[i]->nasobie[j].numer);
   printf(opis_przedmiotu[opis_npc[i]->nasobie[j].typ][opis_npc[i]->nasobie[j].numer].nazwa);
  }
//  fscanf(in,"%i ",&opis_npc[i]->nasobie[Lnasobie-1].wyt);
  int ile_ekw;
  fscanf(in,"\n%i",&ile_ekw);
  printf("\nekw=%i ", ile_ekw);
  opis_npc[i]->przedmioty=NULL;
  if (ile_ekw>0)
  {
   Tprzedmiot pom;
   for(j=0;j<ile_ekw;j++)
   {
    fscanf(in,"%i %i ", &pom.typ,&pom.numer);
    dane=opis(pom);
    if (dane.ma_ladunki || dane.tworzy_stos) fscanf(in,"%i ", &pom.ilosc);
    else pom.ilosc=1;
    pom.wyt=dane.wyt;
    printf(" %s",dane.nazwa);
    dodaj_do_ekwipunku( pom , opis_npc[i] );
   }
  };
  opis_npc[i]->czary=NULL;
  int l_czarow;
  fscanf(in,"\n%i",&l_czarow);
  printf("\nl_czarow=%i ",l_czarow);
  if (l_czarow>0)
  {
   int tryb,rodzaj,czas,l_p;
   lpostac* opis=new lpostac;
   opis->postac=*opis_npc[i];
   for (int k=0;k<l_czarow;k++)
   {
    Tczar czar;
    fscanf(in," %i %i",&rodzaj,&l_p);
    printf ("rodzaj=%i lp=%i ",rodzaj,l_p);
     //getch();
    czar.tryb=0;
    czar.rodzaj=rodzaj;
    czar.ile_do_konca=1000;
    czar.aktywny=true;
    czar.staly=true;
    if (l_p>0)
     for (int l=0;l<l_p;l++)
     {
      int p;
      fscanf(in,"%i ",&p);
      printf("p[%i]=%i ",l,p);
      czar.p[l]=p;
    }
    dodaj_czar(opis,czar);
   }
   *opis_npc[i]=opis->postac;
  }
  //getch();
 }
 fclose(in);
 for(i=0;i<Lnpc;i++)  if (opis_npc[i]) printf(" st[%i]=%s",i,opis_npc[i]->imie);
   //getch();
}
void tworz_npc(int np, int n, int sila, Temocje emocje)
{
  int x,y;
  bool stop=false;
  do
  {
   x=los(1,pietro[np].dl);
   y=los(1,pietro[np].sz);
   if ( dozw_ruch(pietro[np].mapa[x][y]) )
    if (!jest_npc(x,y,np)) stop=true;
  } while (!stop);
  tworz_npc(np, x, y, n, sila,emocje);
}
void tworz_npc(int np,int x, int y, int n, int sila, Temocje emocje)
{
  Tpostac npc;
  if (!opis_npc[n]) { printf("BLAD w tworz npc!!! brak stwora %i.",n);return;}
  strcpy(npc.imie,opis_npc[n]->imie);
  npc.id=n;
  npc.znak=opis_npc[n]->znak;
  npc.kolor=opis_npc[n]->kolor;
  npc.rasa=opis_npc[n]->rasa;
  npc.profesja=opis_npc[n]->profesja;
  npc.x=x;
  npc.y=y;
  npc.poziom=opis_npc[n]->poziom;
  npc.martwy=false;
  for (int j=0;j<Lstat;j++) npc.stat[j]=los(3,1,6)+rasa[npc.rasa].stat[j];
  for (int j=0;j<Lumiejetnosci;j++)
  {
   npc.umiejetnosc[j]=opis_npc[n]->umiejetnosc[j];
   npc.bazowe_umiejetnosc[j]=npc.umiejetnosc[j];
   if (npc.bazowe_umiejetnosc[j]==0) npc.bazowe_umiejetnosc[j]=npc.poziom/3;
  }
  switch (npc.profesja)
  {
   case 1: npc.stat[ST_SF]=los(11,18)+rasa[npc.rasa].stat[ST_SF];
           npc.stat[ST_KO]=los(11,18)+rasa[npc.rasa].stat[ST_KO];
           npc.stat[ST_ZR]=los(11,18)+rasa[npc.rasa].stat[ST_ZR];
         break;
   case 2: npc.stat[ST_ZR]=los(15,18)+rasa[npc.rasa].stat[ST_ZR];
           npc.bazowe_umiejetnosc[UM_ZASKAKIWANIE]=2*npc.poziom;
           npc.bazowe_umiejetnosc[UM_SKRADANIE]=2*npc.poziom;
           npc.bazowe_umiejetnosc[UM_POSZUKIWANIE]=2*npc.poziom;
         break;
   case 3: npc.stat[ST_IN]=los(15,18)+rasa[npc.rasa].stat[ST_IN];
           npc.stat[ST_MD]=los(11,18)+rasa[npc.rasa].stat[ST_MD];
           npc.bazowe_umiejetnosc[UM_M_SKUTECZNOSC]=2*npc.poziom;
           npc.bazowe_umiejetnosc[UM_M_MEDYTACJA]=2*npc.poziom;
         break;
   case 4: npc.stat[ST_IN]=los(13,18)+rasa[npc.rasa].stat[ST_IN];
           npc.stat[ST_MD]=los(15,18)+rasa[npc.rasa].stat[ST_MD];
           npc.bazowe_umiejetnosc[UM_M_SKUTECZNOSC]=1*npc.poziom;
           npc.bazowe_umiejetnosc[UM_M_MEDYTACJA]=2*npc.poziom;
           npc.bazowe_umiejetnosc[UM_POSZUKIWANIE]=1*npc.poziom;
         break;
  }
  for (int j=0;j<Lstat;j++)
  {
   npc.stat[j]=max(1,npc.stat[j]);
   npc.bazowe_stat[j]=npc.stat[j];
  }
  //getch();
  for (int k=0;k<Lnasobie;k++)
  {
/*   npc.nasobie[k].typ   = opis_npc[n]->nasobie[k].typ;
   npc.nasobie[k].numer = opis_npc[n]->nasobie[k].numer;
   npc.nasobie[k].wyt   = opis_npc[n]->nasobie[k].wyt;*/
   npc.nasobie[k] = opis_npc[n]->nasobie[k];
  }
  for(int k=0;k<Lodpornosci;k++)
  {
   npc.odpornosc_DT[k]=0;//los(0,20);
   npc.odpornosc_DR[k]=0;//los(0,100);
  }
  npc.przedmioty=NULL;
  lprzedmiot *obecny=opis_npc[n]->przedmioty;
  while ( obecny)
  {
//   cprintf("typ=%i ",obecny->przedmiot.typ); //getch();
   dodaj_do_ekwipunku( obecny->przedmiot, &npc);
   obecny=obecny->nast;
  }
  //getch();
  lpostac *lnpc=new lpostac;
  npc.czary=NULL;
  lnpc->postac=npc;
  oblicz_postac(lnpc);
 // lnpc->postac.PD=0;//licz_PD(lnpc);
  lczar *c_obecny=opis_npc[n]->czary;
  while (c_obecny)
  {
   Tczar czar=c_obecny->czar;
   dodaj_czar(lnpc,czar);
   c_obecny=c_obecny->nast;
  }
  npc=lnpc->postac;
  delete lnpc;
  npc.PD=0;
  npc.obr=npc.max_obr;
  npc.mana=npc.max_mana;
  npc.emocje=emocje;
  npc.sila=sila;
  npc.niewidoczny=false;
  for (int j=0;j<Lstan;j++) npc.stan[j]=false;
  dodaj_npc(np,npc);
}
void pisz_npc()
{
 clrscr();
 Tpostac npc;
 int j;
 lpostac *obecny=pietro[np_gr].npc;
 while(obecny)
 {
  npc=obecny->postac;
  char imie[25];
  printf( "\n %s hp(%i / %i) mn(%i / %i)  rasa=%s adrs=%i sila=%i",
          npc.imie,npc.obr,npc.max_obr,npc.mana,npc.max_mana,rasa[npc.rasa].nazwa,obecny,npc.sila);
  printf ("   KP=%i \n",npc.KP + premia (npc.stat[1]) );
  for(j=0;j<Lumiejetnosci;j++)
  {
   printf(" %s=%i",umiejetnosci[j],npc.umiejetnosc[j]);
  }
  printf ( "\nSF=%i ZR=%i WT=%i IG=%i IT=%i CH=%i",
           npc.stat[0],npc.stat[1],npc.stat[2],
           npc.stat[3],npc.stat[4],npc.stat[5]);
  printf(   "\np=%i r=%i p=%i z=%c k=%i wz=%i", npc.poziom,
                                                npc.rasa,
                                                npc.profesja,
                                                npc.znak,
                                                npc.kolor,
                                                npc.wzrok);
  for(j=0;j<Lnasobie;j++)
  {
   printf(" I (%i , %i) : ", npc.nasobie[j].typ,npc.nasobie[j].numer);
   printf(opis_przedmiotu[npc.nasobie[j].typ][npc.nasobie[j].numer].nazwa);
  }
  printf("\nEkwipunek : ");
  lprzedmiot *p_obecny=npc.przedmioty;
  while (p_obecny)
  {
   Tprzedmiot pom=p_obecny->przedmiot;
    //printf(" (%i , %i) ",pom.typ,pom.numer);
   printf("%s ",opis_przedmiotu[pom.typ][pom.numer].nazwa);
   p_obecny=p_obecny->nast;
  }
  printf("\nEmocje :");
  switch (npc.emocje)
  {
    case PRZYJAZNY : printf(" przyjazny.  ");break;
    case OBOJETNY  : printf(" obojetny.  ");break;
    case WROGI     : printf(" wrogi.  ");break;
  }
  printf(" czyli %i \n",pobierz_ai_status(npc.sila,Hero.sila,np_gr));
  if (npc.stan[STAN_UNIERUCHOMIENIE]) printf(" nieruchomy.  ");
  if (npc.stan[STAN_NIEPRZYTOMNY]) printf(" spi.  ");
  printf("\nRzuty : ");
  for (int j=0;j<Lrzut;j++) printf(" %i",npc.rzut[j]);
  printf("\nCzary : ");
  lczar *c_obecny=npc.czary;
  while (c_obecny)
  {
   Tczar pom=c_obecny->czar;
   printf(" Efekt (%i) %i tur ", pom.rodzaj,
                                 pom.ile_do_konca);
   c_obecny=c_obecny->nast;
  }
  getch();
  obecny=obecny->nast;
 }
// clrscr();
}
bool jest_npc(int x,int y,int np)
{
 if (np<0) np=np_gr;
 lpostac *obecny=pietro[np].npc;
 while (obecny)
 {
  if (obecny->postac.x==x && obecny->postac.y==y) return true;
  obecny=obecny->nast;
 }
 return false;
}
bool jest_postac(Tlokacja l,int id,int np)
{
 if (np<0) np=np_gr;
 lpostac *obecny=pietro[np].npc;
 if (id==ID_HERO) obecny=lHero;
 while (obecny)
 {
  if (obecny->postac.id==id)
   if ( nalezy( l.x1, l.y1, l.x2, l.y2,
        obecny->postac.x,obecny->postac.y) ) return true;
  obecny=obecny->nast;
 }
 return false;
}
lpostac* znajdz_postac(int x, int y, int np)
{
 if (np<0) np=np_gr;
 if ( jest_Hero(x,y,np) ) return lHero;
 return znajdz_npc(x,y,np);
}
lpostac* znajdz_npc(int x,int y,int np)
{
 if (np<0) np=np_gr;
 lpostac *obecny=pietro[np].npc;
 while (obecny)
 {
  if (obecny->postac.x==x && obecny->postac.y==y)
     return obecny;
  obecny=obecny->nast;
 }
 return NULL;
}
lpostac* szukaj_postaci(int n, int np)
{
 if (np<0) np=np_gr;
 if (n==ID_HERO) return lHero;
 lpostac *obecny=pietro[np].npc;
 while (obecny)
 {
  if (obecny->postac.id==n) return obecny;
  obecny=obecny->nast;
 }
 return NULL;
}
lpostac* szukaj_postaci(int n, Tlokacja l, int np)
{
 if (np<0) np=np_gr;
 if (n==ID_HERO) return lHero;
 lpostac *obecny=pietro[np].npc;
 while (obecny)
 {
  if (obecny->postac.id==n)
   if ( nalezy( l.x1, l.y1, l.x2, l.y2,
        obecny->postac.x,obecny->postac.y) ) return obecny;
  obecny=obecny->nast;
 }
 return NULL;
}
void usun_npc( int np, lpostac *npc)
{
 niszcz_liste(npc->postac.przedmioty);
 niszcz_liste(npc->postac.czary);
 lpostac *obecny=pietro[np].npc, *pop=obecny;
 if (obecny==npc) pietro[np].npc=pietro[np].npc->nast;
 else
 {
  while (obecny)
  {
   if (obecny==npc) break;
   pop=obecny;
   obecny=obecny->nast;
  }
  pop->nast=obecny->nast;
 }
 //jeszcze usuwanie czarow i przedmiotow
 delete obecny;
}
void dodaj_npc( int np, Tpostac npc) //Tu moze cos sie sypnac , zrob proc przypisz
{
 lpostac *lista=pietro[np].npc,
         *stary=lista;
 lista=new lpostac;
 lista->postac=npc;
 for(int i=0;i<Lstat;i++) lista->postac.stat[i]=npc.stat[i];
 lista->nast=stary;
 pietro[np].npc=lista;
}
void zabij_npc( int np , lpostac *npc)
{
 int x=npc->postac.x,
     y=npc->postac.y;
 for (int k=0;k<Lnasobie;k++)
     dodaj_do_przedmiotow(np,x,y,npc->postac.nasobie[k]);
 lprzedmiot *obecny=npc->postac.przedmioty;
 while (obecny)
 {
  dodaj_do_przedmiotow(np,x,y,obecny->przedmiot);
  obecny=obecny->nast;
 }
 npc->postac.martwy=true;
 npc->postac.x=-50;
 npc->postac.y=-50;
}

void ai_npc(lpostac *npc, int np)
{
 if (npc->postac.martwy) return;
 if (np<0) np=np_gr;
 int popPD=npc->postac.PD;//rozwiazanie tym czasowe
 int profesja=npc->postac.profesja;
 switch (profesja)
 {
  case 1 :
  case 2 : ai_wojownik(npc,np);break;
  case 3 : ai_mag(npc,np);break;
  case 4 : ai_kaplan(npc,np);break;
  default: ai_ruch_losowy(npc,np);  //poprawke tu wsadz
 }
 if (npc->postac.PD==popPD) npc->postac.PD-=100;
}
void ai_wojownik(lpostac *lnpc, int np)
{
 if (np<0) np=np_gr;
 Tpostac& npc=lnpc->postac;
 Topis_przedmiotu bron=opis(npc.nasobie[NS_P_REKA]);
 if ( bron.p[1]==3 ) { ai_lucznik(lnpc,np);return;}
 int xp=npc.x, yp=npc.y;
 lpostac *lwrog=znajdz_najblizszego_widocznego_wroga(lnpc);
 if (!lwrog)
 {
  if (los(1,2)==1) ai_ruch_losowy(lnpc,np);
     else ai_odpoczywaj(lnpc,np);
  return;
 }
 Tpostac& wrog=lwrog->postac;
 int xc=wrog.x, yc=wrog.y;
 int d = odleglosc_ruch(xc,yc,xp,yp);
 int ds = int(odleglosc( xc,yc,xp,yp));
 if (ds>npc.wzrok) ai_ruch_losowy(lnpc,np);
 //gotoxy(1,15);textattr(15);cprintf("(%i,%i) - (%i,%i) czyli %i widzi Hero  ",xp,yp,xc,yc,nr);
 if (d<2) atak ( lnpc,np,xc,yc );
     else ai_idz_do(lnpc,xc,yc,np);
}
void ai_lucznik (lpostac *lnpc, int np)
{
 if (np<0) np=np_gr;
 Tpostac& npc=lnpc->postac;
 int xp=npc.x, yp=npc.y;
 lpostac *lwrog=znajdz_najblizszego_widocznego_wroga(lnpc);
 if (!lwrog)
 {
  if (los(1,2)==1) ai_ruch_losowy(lnpc,np);
     else ai_odpoczywaj(lnpc,np);
  return;
 }
 Tpostac& wrog=lwrog->postac;
 int xc=wrog.x, yc=wrog.y;
 int d = odleglosc_ruch(xc,yc,xp,yp);
 int ds = int(odleglosc( xc,yc,xp,yp));
 if ( !czy_widzi(xp,yp,xc,yc) || !czy_widzi(xc,yc,xp,yp) )
 {
  ai_ruch_losowy(lnpc,np);
  return;
 }
 if (d<3)
 {
  if (ai_uciekaj_od(lnpc,xc,yc,np)) return;
 }
 if ( ds<=npc.wzrok && npc.nasobie[NS_AMMO].wyt>0 )
 {
   atak ( lnpc,np,xc,yc );
   return;
 }
 ai_ruch_losowy(lnpc,np);
 // else zaloz bron do walki wrecz i atakuj
}
void ai_kaplan(lpostac *lnpc, int np)
{
 if (np<0) np=np_gr;
 Tpostac& npc=lnpc->postac;
 int xp=npc.x, yp=npc.y;
 lpostac *lwrog=znajdz_najblizszego_widocznego_wroga(lnpc);
 if (!lwrog)
 {
   if (los(1,2)==1) ai_ruch_losowy(lnpc,np);
      else ai_odpoczywaj(lnpc,np);
   return;
 }
 Tpostac& wrog=lwrog->postac;
 int xc=wrog.x, yc=wrog.y;
 int dr = odleglosc_ruch(xc,yc,xp,yp);
 int ds = int( odleglosc(xc,yc,xp,yp) );
 if ( !czy_widzi(xp,yp,xc,yc) || !czy_widzi(xc,yc,xp,yp) )
 {
   if ( npc.obr <= (npc.max_obr/2) && npc.mana>-10)
  {
    npc.mana-=opis_czaru[CZAR_SREDNIE_LECZENIE].mana;
    rzuc_czar(lnpc,npc.x,npc.y,CZAR_SREDNIE_LECZENIE);
    //ai_wezwij_pomoc();
    return;
  }
   ai_odpoczywaj(lnpc,np);
   return;
 }
 if ( npc.mana>-10 )
 {
  if ( npc.obr <= (npc.max_obr/2) )
  {
    npc.mana-=opis_czaru[CZAR_SREDNIE_LECZENIE].mana;
    rzuc_czar(lnpc,npc.x,npc.y,CZAR_SREDNIE_LECZENIE);
    //ai_wezwij_pomoc();
    return;
  }
  if (ds>npc.wzrok)
  {
    ai_wojownik(lnpc,np);
    return;
  }
  if (dr<2 && !npc.stan[STAN_TARCZA] )
  {
    npc.mana-=opis_czaru[CZAR_TARCZA].mana;
    rzuc_czar(lnpc,npc.x,npc.y,CZAR_TARCZA);
    return;
  }
  if (dr<3 && los(0,1)==0)
  {
    if (ai_uciekaj_od (lnpc,xc,yc,np)) return;
  }
  if ( !wrog.stan[STAN_SLEPOTA] && los(0,4)==0)
  {
    npc.mana-=opis_czaru[CZAR_SLEPOTA].mana;
    rzuc_czar(lnpc,wrog.x,wrog.y,CZAR_SLEPOTA);
    return;
  }
  switch ( los(1,3) )
  {
   case 1:
   // generuj_bufor();rysuj_bufor();
    npc.mana-=opis_czaru[CZAR_KWASOWA_STRZALA].mana;
    rzuc_czar(lnpc,wrog.x,wrog.y,CZAR_KWASOWA_STRZALA);
   break;
   case 2:
    npc.mana-=opis_czaru[CZAR_MAGICZNY_POCISK].mana;
    rzuc_czar(lnpc,wrog.x,wrog.y,CZAR_MAGICZNY_POCISK); ///Magiczny pocisk
   break;
   case 3:
    npc.mana-=opis_czaru[CZAR_NIEWIDZIALNOSC].mana;
    rzuc_czar(lnpc,npc.x,npc.y,CZAR_NIEWIDZIALNOSC);
   break;
  }
 }
 else ai_wojownik(lnpc,np);
}
void ai_mag(lpostac *lnpc, int np)
{
 if (np<0) np=np_gr;
 Tpostac& npc=lnpc->postac;
 //Tpostac& mag=lnpc->postac;
 int xp=npc.x, yp=npc.y;
 lpostac *lwrog=znajdz_najblizszego_widocznego_wroga(lnpc);
 if (!lwrog) {
   lwrog=znajdz_najblizszego_wroga(lnpc);
   if (lwrog) {
    Tpostac& wrog=lwrog->postac;
    if ( wrog.niewidoczny && los(0,1)==0 && odleglosc(wrog.x,wrog.y,xp,yp)<npc.wzrok)
    {
     npc.mana-=opis_czaru[CZAR_WYKRYCIE_NIEWIDZIALNYCH].mana;
     rzuc_czar(lnpc,wrog.x,wrog.y,CZAR_WYKRYCIE_NIEWIDZIALNYCH);
     return;
    }
   } else if (los(1,2)==1) ai_ruch_losowy(lnpc,np);
      else ai_odpoczywaj(lnpc,np);
  return;
 }
 Tpostac& wrog=lwrog->postac;
 int xc=wrog.x, yc=wrog.y;
 int dr = odleglosc_ruch(xc,yc,xp,yp);
 int ds = int( odleglosc(xc,yc,xp,yp) );
 if ( !czy_widzi(xc,yc,xp,yp) || dr>npc.wzrok) {
  if ( npc.obr <= (2*npc.max_obr/3) && npc.mana>-50)
  {
    npc.mana-=opis_czaru[CZAR_UZDROWIENIE].mana;
    rzuc_czar(lnpc,npc.x,npc.y,CZAR_UZDROWIENIE); ///Uzdrowienie
    //ai_wezwij_pomoc();
    return;
  }
  if (los(0,1)==0) ai_odpoczywaj(lnpc,np);
   else ai_wojownik(lnpc,np);
  return;
 }
 if ( npc.mana<-50 ) {
  ai_uciekaj_od(lnpc,xc,yc,np);
  return;
 }
 if ( npc.obr <= (npc.max_obr/3) )
  {
    npc.mana-=opis_czaru[CZAR_UZDROWIENIE].mana;
    rzuc_czar(lnpc,npc.x,npc.y,CZAR_UZDROWIENIE); ///Uzdrowienie
    //ai_wezwij_pomoc();
    return;
  }
  if (dr<2 && npc.odpornosc_DR[OBR_NORMAL]<30 && npc.obr <= (2*npc.max_obr/3))
  {
    npc.mana-=opis_czaru[CZAR_KAMIENNA_SKORA].mana;
    rzuc_czar(lnpc,npc.x,npc.y,CZAR_KAMIENNA_SKORA);
    return;
  } //lepszy bylby teleport
  if ( npc.obr <= (npc.max_obr/2) && npc.odpornosc_DR[OBR_ELEKT]<70)
  {
    npc.mana-=opis_czaru[CZAR_OCHRONA_ZYWIOLY].mana;
    rzuc_czar(lnpc,npc.x,npc.y,CZAR_OCHRONA_ZYWIOLY); ///Uzdrowienie
    //ai_wezwij_pomoc();
    return;
  }
  if ((dr<3 || npc.niewidoczny) && los(0,1)==0)
   if (ai_uciekaj_od (lnpc,xc,yc,np)) return;
  if ( !wrog.stan[STAN_OSLABIENIE] && los(0,4)==0)
  {
    npc.mana-=opis_czaru[CZAR_OSLABIENIE].mana;
    rzuc_czar(lnpc,wrog.x,wrog.y,CZAR_OSLABIENIE);
    return;
  }
  if ( !wrog.stan[STAN_SPOWOLNIENIE] && los(0,3)==0)
  {
    npc.mana-=opis_czaru[CZAR_SPOWOLNIENIE].mana;
    rzuc_czar(lnpc,wrog.x,wrog.y,CZAR_SPOWOLNIENIE);
    return;
  }
  switch ( los(1,7) )
  {
   case 1:
    npc.mana-=opis_czaru[CZAR_KWASOWA_STRZALA].mana;
    rzuc_czar(lnpc,wrog.x,wrog.y,CZAR_KWASOWA_STRZALA);
   break;
   case 2:
    npc.mana-=opis_czaru[CZAR_MAGICZNY_POCISK].mana;
    rzuc_czar(lnpc,wrog.x,wrog.y,CZAR_MAGICZNY_POCISK);
   break;
   case 3:
    npc.mana-=opis_czaru[CZAR_NIEWIDZIALNOSC].mana;
    rzuc_czar(lnpc,npc.x,npc.y,CZAR_NIEWIDZIALNOSC);
   break;
   case 4:
    npc.mana-=opis_czaru[CZAR_OGNISTA_STRZALA].mana;
    rzuc_czar(lnpc,wrog.x,wrog.y,CZAR_OGNISTA_STRZALA);
   break;
   case 5:
    npc.mana-=opis_czaru[CZAR_POCISKI_MOCY].mana;
    rzuc_czar(lnpc,wrog.x,wrog.y,CZAR_POCISKI_MOCY);
   break;
   case 6:
    npc.mana-=opis_czaru[CZAR_PRZYW_ZYWIOLAKA].mana;
    {
     int xn,yn;
     if ( znajdz_najblizsze_wolne_pole(xn,yn,wrog.x,wrog.y,np) )
        rzuc_czar(lnpc,xn,yn,CZAR_PRZYW_ZYWIOLAKA);
    }
   break;
   case 7:
    npc.mana-=opis_czaru[CZAR_PORAZENIE].mana;
    rzuc_czar(lnpc,wrog.x,wrog.y,CZAR_PORAZENIE);
   break;

  }

}
bool ai_ruch_losowy( lpostac *lnpc,int np)
{
 if (np<0) np=np_gr;
 Tpostac& npc=lnpc->postac;
 if (npc.stan[STAN_UNIERUCHOMIENIE]) return false;
 int x,y;
 bool koniec=false;
 int n=0;
 do {
   x=npc.x;
   y=npc.y;
   x+=los(-1,1);
   y+=los(-1,1);
   if (x<1) x=1;
   if (x>pietro[np].dl) x=pietro[np].dl;
   if (y<1) y=1;
   if (y>pietro[np].sz) y=pietro[np].sz;
   if (dozw_ruch(pietro[np].mapa[x][y]))
    if (!jest_ktos(x,y,np)) koniec=true;
   n++; if (n>20) return false; ////Na wszelki wypadek by gra sie nie zaciela
 } while (!koniec);
 if (npc.x==x && npc.y==y) return false;
 idz_do(lnpc,x,y,np);
// npc.x=x;
// npc.y=y;
 return true;
}
bool ai_idz_do(lpostac *lnpc, int xc, int yc,int np)
{
 if (np<0) np=np_gr;
 Tpostac& npc=lnpc->postac;
 if (npc.stan[STAN_UNIERUCHOMIENIE]) return false;
 int xp=npc.x,
     yp=npc.y;
 int x,y,d,kx=xp,ky=yp;
 d=odleglosc_ruch(xc,yc,xp,yp);
 for (x=max(1,xp-1); x < min(pietro[np].dl,xp+2) ;x++)
  for (y=max(1,yp-1); y < min(pietro[np].sz,yp+2) ;y++)
   if ( dozw_ruch(pietro[np].mapa[x][y]) )
    if ( !jest_ktos(x,y,np) )
     //if (x!=xc || y!=yc)
      if ( odleglosc_ruch(xc,yc,x,y) < d )
      {
        kx=x;
        ky=y;
        d= odleglosc_ruch(xc,yc,x,y);
      }
 if (kx==xp && ky==yp) return ai_ruch_losowy(lnpc,np);
  else
  {
   idz_do(lnpc,kx,ky,np);
   //npc.x=kx;
   //npc.y=ky;
   return true;
  }
}
bool ai_uciekaj_od(lpostac *lnpc, int xc, int yc,int np)
{
 if (np<0) np=np_gr;
 Tpostac& npc=lnpc->postac;
 if (npc.stan[STAN_UNIERUCHOMIENIE]) return false;
 int xp=npc.x,
     yp=npc.y;
 int x,y,d,kx=xp,ky=yp;
 d=odleglosc_ruch(xc,yc,xp,yp);
 for (x=max(1,xp-1); x < min(pietro[np].dl,xp+2) ;x++)
  for (y=max(1,yp-1); y < min(pietro[np].sz,yp+2) ;y++)
   if ( dozw_ruch(pietro[np].mapa[x][y]) )
    if ( !jest_ktos(x,y,np) )
     if ( odleglosc_ruch(xc,yc,x,y) > d )
      if ( nalezy(1,1,pietro[np_gr].dl,pietro[np_gr].sz,x,y) )
      {
        kx=x;
        ky=y;
        d= odleglosc_ruch(xc,yc,x,y);
      }
 if (kx==xp && ky==yp) return ai_ruch_losowy(lnpc,np);
  else
  {
   idz_do(lnpc,kx,ky,np);
   //npc.x=kx;
   //npc.y=ky;
   return true;
  }
}
bool ai_kraz_wokol(lpostac *lnpc, int xc, int yc,int np)
{
 if (np<0) np=np_gr;
 int x=lnpc->postac.x,
     y=lnpc->postac.y;
 if (lnpc->postac.stan[STAN_UNIERUCHOMIENIE]) return false;
 int d=odleglosc_ruch(xc,yc,x,y);
 if ( los(1,10)<d ) return ai_idz_do(lnpc,xc,yc,np);
               else return ai_ruch_losowy(lnpc,np);
}
void ai_odpoczywaj(lpostac *lnpc, int np)
{
  if (np<0) np=np_gr;
  odpoczywaj(lnpc);
}
