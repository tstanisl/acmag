#include "glowny.h"
void intro()
{
 gotoxy(35,1);
 textattr(2);
 cprintf(" A C M A G ");
 gotoxy(1,3);
 textattr(15);
 cprintf("    Ju¾ w mˆodo˜ci fascynowaˆy Ciebie opowie˜ci, historie o poszukiwaczach przygod, tajemniczych stworach i bezcennych skarbach. Te dzieci©ce marzenia towarzyszyˆy caˆemu twojemu ¾yciu. Gdy twoje rodzinne miasto zostaˆo zaatakowane przez tajemnicze monstra, wiedziaˆa˜, ¾e nadszedˆ wˆa˜nie czas na Ciebie. Wiesz, ¾e czeka Ci© trudne zadanie lecz sˆawa i moc maj¥ swoj¥ cen©... by† mo¾e najwy¾sz¥.");
 delay(1000);
 getch();
}
void zapisz_postac(FILE* out, Tpostac postac)
{
  fprintf(out,"\n%s\n",postac.imie);
  int j;
  for(j=0;j<Lstat;j++) fprintf(out, " %i",postac.bazowe_stat[j]);
  for(j=0;j<Lumiejetnosci;j++) fprintf(out, " %i",postac.bazowe_umiejetnosc[j]);
  fprintf(out," %i %i %i %c %i %i %i", postac.poziom, postac.profesja, postac.rasa,
                                    postac.znak, postac.kolor,
                                    postac.x, postac.y);
  fprintf(out," %d %f %i",postac.dosw,postac.mnoznik_dosw,postac.sila);
  for(j=0;j<Lnasobie;j++)
    fprintf(out, " %i %i %i %i", postac.nasobie[j].typ,
                                 postac.nasobie[j].numer,
                                 postac.nasobie[j].ilosc,
                                 postac.nasobie[j].wyt);
  fprintf(out," %i", zlicz_liste(postac.przedmioty));
  lprzedmiot *obecny=postac.przedmioty;
  while (obecny)
  {
   Tprzedmiot pom=obecny->przedmiot;
   fprintf(out," %i %i %i %i",pom.typ,pom.numer,pom.ilosc,pom.wyt);
   obecny=obecny->nast;
  };
 fprintf(out," %i %i %i %i %i", postac.obr, postac.max_obr,
                                postac.mana, postac.max_mana,
                                postac.wzrok);
 for (int i=0;i<Lczar;i++) fprintf(out," %i",ksiega_czarow[i]);
}
/*
void zapisz_pietro(FILE* out, int np)
{
 fprintf(out,"\n%s\n",pietro[np].nazwa);
 fprintf(out," %i %i %i %i %i", pietro[np].dl,pietro[np].sz,pietro[np].l_przedmiotow,
                                pietro[np].l_npc, pietro[np].l_schodow);
 int x,y;
 fprintf(out,"\n");
 for(y=1;y<=pietro[np].sz;y++)
 {
  for(x=1;x<=pietro[np].dl;x++)
   fprintf(out,"%c",pietro[np].mapa[x][y]);
  fprintf(out,"\n");
 }
 for(y=1;y<=pietro[np].sz;y++)
 {
  for(x=1;x<=pietro[np].dl;x++)
   fprintf(out,"%c",pietro[np].mgla[x][y]);
  fprintf(out,"\n");
 }
 if (pietro[np].l_npc>0)
    for(int i=0;i<pietro[np].l_npc;i++) zapisz_postac(out,pietro[np].npc[i]);
 if (pietro[np].l_przedmiotow>0)
  for(int i=0;i<pietro[np].l_przedmiotow;i++)
  {
   fprintf(out," %i %i %i %i %i", pietro[np].przedmiot[i].typ,pietro[np].przedmiot[i].numer,
                                  pietro[np].przedmiot[i].x,pietro[np].przedmiot[i].y,
                                  pietro[np].przedmiot[i].wyt);
  }
 if (pietro[np].l_schodow>0)
  for(int i=0;i<pietro[np].l_schodow;i++)
  {
   fprintf(out," %i %i %i %i %i", pietro[np].schody[i].x,pietro[np].schody[i].y,
                                  pietro[np].schody[i].n_pietra,pietro[np].schody[i].n_schodow,
                                  pietro[np].schody[i].aktywne);
  }
} */
bool zapisz(char* sciezka)
{
 FILE *out=fopen(sciezka,"w");
// fprintf(out," %i %i",Lpieter,np_gr);
 zapisz_postac(out,Hero);
 fprintf(out," %i",pietro[0].flaga[0]);
// for(int i=0;i<Lpieter;i++) zapisz_pietro(out,i);
 fclose(out);
 return true;
}
void wczytaj_postac(FILE* in, Tpostac &postac)
{
  textattr(15);clrscr();
  //Tpostac postac;
  fscanf(in,"\n%[^\n]",&postac.imie);
  printf(" Wczytano nazwe :%s ",postac.imie);
//  getch();
  int j;
  fscanf(in,"\n");
  for(j=0;j<Lstat;j++) fscanf(in, " %i",&postac.bazowe_stat[j]);
  printf("\n Wczytano stats ");
  for(j=0;j<Lstat;j++) printf( " st[%i]=%i",j,postac.bazowe_stat[j]);
//  getch();
  printf("\n Wczytano umiejetnosci ");
  for(j=0;j<Lumiejetnosci;j++) fscanf(in, " %i",&postac.bazowe_umiejetnosc[j]);
  for(j=0;j<Lumiejetnosci;j++) printf( " um[%i]=%i",j,postac.bazowe_umiejetnosc[j]);
//  getch();
  fscanf(in," %i %i %i %c %i %i %i", &postac.poziom, &postac.profesja, &postac.rasa,
                                     &postac.znak, &postac.kolor,
                                     &postac.x, &postac.y);
  printf("\np=%i pr=%i r=%i z=%c k=%i x=%i y=%i", postac.poziom, postac.profesja,
                                                 postac.rasa,postac.znak, postac.kolor,
                                                 postac.x, postac.y);
  fscanf(in," %D %f %i",&postac.dosw,&postac.mnoznik_dosw,&postac.sila);
  printf(" dosw= %d mnoz= %f sila= %i",postac.dosw,postac.mnoznik_dosw,postac.sila);
  printf("\nUzbrojenie");
  for(j=0;j<Lnasobie;j++)
  {
    fscanf(in, " %i %i %i %i", &postac.nasobie[j].typ,
                               &postac.nasobie[j].numer,
                               &postac.nasobie[j].ilosc,
                               &postac.nasobie[j].wyt);
    printf("(%i,%i,%i,%i)", postac.nasobie[j].typ,
                            postac.nasobie[j].numer,
                            postac.nasobie[j].ilosc,
                            postac.nasobie[j].wyt);
  }
  int ile_przedmiotow=0;
  fscanf(in," %i", &ile_przedmiotow);
  printf("\nl_ekw=%i ",ile_przedmiotow);
//  getch();
  postac.przedmioty=NULL;
  if (ile_przedmiotow>0)
  {
   Tprzedmiot pom;
   for(j=0;j<ile_przedmiotow;j++)
   {
    fscanf(in," %i %i %i %i", &pom.typ,&pom.numer,&pom.ilosc,&pom.wyt);
    printf(" (t=%i n=%i w=%i) ",pom.typ,pom.numer,pom.wyt);
    dodaj_do_ekwipunku(pom,&postac);
   }
  }
 fscanf(in," %i %i %i %i %i", &postac.obr, &postac.max_obr,
                              &postac.mana, &postac.max_mana,
                              &postac.wzrok);
 //Hero=postac;
/*  printf("\n%s\n",postac.imie);
  printf(" na sobie :");
  for(j=0;j<Lnasobie;j++)
    printf( " %i %i %i", postac.nasobie[j].typ,
                         postac.nasobie[j].numer,
                         postac.nasobie[j].wyt);
 // printf(" Ekwipunek", postac.ekwipunek.liczba);
  if (postac.ekwipunek.liczba>0)
  {
   Tprzedmiot pom;
   for(j=0;j<postac.ekwipunek.liczba;j++)
   {
    pom=postac.ekwipunek.przedmiot[j];
    printf(" (t=%i n=%i w=%i) ",pom.typ,pom.numer,pom.wyt);
   }
  };
 printf(" obr: (%i / %i)  mana: (%i / %i) wz=%i", postac.obr, postac.max_obr,
                                postac.mana, postac.max_mana,
                                postac.wzrok);
 getch();*/

}/*
void wczytaj_pietro(FILE* in, int np)
{
 fscanf(in,"\n%[^\n]",&pietro[np].nazwa);
 fscanf(in,"\n %i %i %i %i %i", &pietro[np].dl,&pietro[np].sz,&pietro[np].l_przedmiotow,
                                &pietro[np].l_npc, &pietro[np].l_schodow);
 printf("\n dl=%i sz=%i lp=%i ln=%i ls=%i", pietro[np].dl,pietro[np].sz,
                                            pietro[np].l_przedmiotow,
                                            pietro[np].l_npc,
                                            pietro[np].l_schodow);
 getch();
 int x,y;
 //fscanf(in,"\n");
 pietro[np].mapa=new char*[pietro[np].dl+1];
 for(x=1;x<=pietro[np].dl;x++)
 {
  pietro[np].mapa[x]=new char[pietro[np].sz+1];
  for(y=1;y<=pietro[np].sz;y++)
   fscanf(in,"%c",&pietro[np].mapa[x][y]);
  //fscanf(in,"\n");
 }
 pietro[np].mgla=new char*[pietro[np].dl+1];
 for(x=1;x<=pietro[np].dl;x++)
 {
  pietro[np].mgla[x]=new char[pietro[np].sz+1];
  for(y=1;y<=pietro[np].sz;y++)
   fscanf(in,"%c",&pietro[np].mgla[x][y]);
  //fscanf(in,"\n");
 }
 if (pietro[np].l_npc>0)
 {
  pietro[np].npc=new Tpostac[pietro[np].l_npc];
  for(int i=0;i<pietro[np].l_npc;i++) wczytaj_postac(in,pietro[np].npc[i]);
 }
 printf("L_PRZED=%i ",pietro[np].l_przedmiotow);
 if (pietro[np].l_przedmiotow>0)
 {
  pietro[np].przedmiot=new Tprzedmiot[pietro[np].l_przedmiotow];
  for(int i=0;i<pietro[np].l_przedmiotow;i++)
  {
   fscanf(in," %i %i %i %i %i", &pietro[np].przedmiot[i].typ,&pietro[np].przedmiot[i].numer,
                                &pietro[np].przedmiot[i].x,&pietro[np].przedmiot[i].y,
                                &pietro[np].przedmiot[i].wyt);
   printf(" t=%i n=%i x=%i y=%i w=%i", pietro[np].przedmiot[i].typ,pietro[np].przedmiot[i].numer,
                                       pietro[np].przedmiot[i].x,pietro[np].przedmiot[i].y,
                                       pietro[np].przedmiot[i].wyt);
  }
 }
 if ( pietro[np].l_schodow>0)
 {
  pietro[np].schody=new Tschody[pietro[np].l_schodow];
  for(int i=0;i<pietro[np].l_schodow;i++)
  {
   fscanf(in," %i %i %i %i %i", &pietro[np].schody[i].x,&pietro[np].schody[i].y,
                                &pietro[np].schody[i].n_pietra,&pietro[np].schody[i].n_schodow,
                                &pietro[np].schody[i].aktywne);
  }
 }
} */
bool wczytaj (char* sciezka)
{
 FILE *in=fopen(sciezka,"r");
 if (in==NULL) {printf("brak pliku: %s",sciezka);getch();return false;}
// niszcz_lochy();
// fscanf(in," %i %i",np_gr,np_gr); //////Tu bedzie ile pieter
 wczytaj_postac(in,Hero);
 for(int i=0;i<Lstat;i++) Hero.stat[i]=Hero.bazowe_stat[i];
 for(int i=0;i<Lczar;i++) fscanf(in,"%i",&ksiega_czarow[i]);
 int np;
 fscanf(in,"%i",&np);
 printf("%i",np);
 pietro[0].flaga[0]=np;
 bool s4,s7,s10;
 s4=s7=s10=false;
 if (np>=4) s4=true;
 if (np>=7) s7=true;
 if (np>=10) s10=true;
 tworz_schody(0,1,29,4,true);
 tworz_schody(0,4,51,19,s4);
 tworz_schody(0,7,40,26,s7);
 tworz_schody(0,10,54,38,s10);/*
 tworz_schody(0,1,1,4,true);
 tworz_schody(0,1,2,19,s4);
 tworz_schody(0,1,3,26,s7);
 tworz_schody(0,1,4,38,s10);*/
// tworz_schody(0,np,true);
// getch();
// pietro=new Tpietro[Lpieter];
// for(int i=0;i<Lpieter;i++) wczytaj_pietro(in,i);
 fclose(in);
 return true;
}


void tura_Hero()
{
 //while (!kbhit());
 int x_n=Hero.x,y_n=Hero.y;
 x_gr=Hero.x;y_gr=Hero.y;
 char odp=0;
 static int pop_odp=0;
 bool koniec_tury=false;
 while (!koniec_tury)
 {
  //for(int i=0;i<20;i++) ungetch(0);
  //while (!kbhit());
  //odp=getch();
  do { odp=getch(); } while (odp==pop_odp && kbhit());pop_odp=odp;
  czysc_info();
  czysc_linie(1);
  ruch_gracza(odp,x_n,y_n);
  x_n=max( 1, min(pietro[np_gr].dl, x_n) );
  y_n=max( 1, min(pietro[np_gr].sz, y_n) );
  if (jest_npc(x_n,y_n))
  {
   lpostac* lnpc=znajdz_npc(x_n,y_n);
   if (czy_wrogowie( &Hero,&(lnpc->postac) )) atak(lHero,np_gr,x_n,y_n);
   if (czy_sprzymierzency( &Hero,&(lnpc->postac) ))
   {
    int xp=lnpc->postac.x,
        yp=lnpc->postac.y;
    idz_do(lnpc,Hero.x,Hero.y);
    idz_do(lHero,xp,yp);
    /*lnpc->postac.x=Hero.x;
    lnpc->postac.y=Hero.y;
    Hero.x=xp;
    Hero.y=yp;*/
   }
   return;
  }
  if ((x_n!=x_gr || y_n!=y_gr) && dozw_ruch(pietro[np_gr].mapa[x_n][y_n]))
  {
   x_gr=x_n;
   y_gr=y_n;
   idz_do(lHero,x_n,y_n);
   if ( jest_pulapka(x_n,y_n) ) //powinno byc w idz_do
   {
    info("Wpadasz w pulapk©.");
    lpulapka *pulapka=znajdz_pulapke( x_n, y_n);
    efekt_pulapka ( pulapka );
    usun_z_pulapek ( pulapka );
    //return;
   }
   koniec_tury=true;
  }
  else
  {
  if (odp=='e') pisz_ekwipunek();
  if (odp=='u') pisz_uzbrojenie();
  if (odp=='s') pisz_umiejetnosci();
  if (odp=='a') pisz_statystyki();
  if (odp=='n') pisz_npc();
  if (odp=='p') koniec_tury=Hero_podnies_przedmiot();
  if (odp=='z') koniec_tury=Hero_zaloz_uzbrojenie();
  if (odp=='Z') koniec_tury=Hero_zdejmij_uzbrojenie();
  if (odp=='w') koniec_tury=Hero_wyrzuc_przedmiot();
  if (odp=='>') koniec_tury=Hero_idz_schodami();
  if (odp=='A') koniec_tury=Hero_atak();
  if (odp=='c') koniec_tury=Hero_wybierz_czar();
  if (odp=='o') koniec_tury=Hero_otworz_drzwi();
  if (odp=='V') koniec_tury=Hero_zamknij_drzwi();
  if (odp=='v') koniec_tury=Hero_wywaz_drzwi();
  if (odp=='r') koniec_tury=Hero_rozbroj_pulapke();
  if (odp=='C') koniec_tury=Hero_uzyj_przedmiotu();
//  if (odp=='W') koniec_tury=Hero_wypij_miksture();
  if (odp=='k') koniec_tury=Hero_ukryj_sie();
  if (odp=='l') koniec_tury=Hero_kradziez();
  if (odp=='h') koniec_tury=Hero_handluj();
  if (odp=='m') koniec_tury=Hero_rozmawiaj();
  if (odp=='5') koniec_tury=Hero_odpoczywaj();
/*  if (odp=='`') awans_na_poziom(lHero);
  if (odp=='+')
  {
   for(int i=0;i<Lumiejetnosci;i++) Hero.bazowe_umiejetnosc[i]++;
   awans_na_poziom(lHero);
  }*/
  if (odp=='Q') wyjscie_z_gry();
  if (odp=='S')
  {
   char sciezka[]="save/";
   strcat(sciezka,Hero.imie);
   strcat(sciezka,".asv");
   zapisz(sciezka);
   info("Gra zapisana.");
  }
//  if (odp=='L') wczytaj("save/dilnah.asv");
//  if ( odp=='p' || odp=='z' || odp=='w') clrscr();
//  if ( odp=='p' || odp=='z' || odp=='w' || odp=='>') koniec_tury=true;

  x_n=x_gr;
  y_n=y_gr;
  if (odp=='x' || odp=='e' || odp=='u' || odp=='s' || odp=='a' || odp=='n' || odp=='c')
  {
   if (odp=='e' || odp=='a' || odp=='u' ) getch();
   if (odp=='c' && koniec_tury) break;
//   if (odp=='h' && !koniec_tury) break;
   clrscr();
   generuj_bufor();
   rysuj_bufor();
  }
  }
  pisz_informacje_Hero();
 }
}
void tura_npc(int np, lpostac* npc)
{
// oblicz_postac( npc ,true); //problem z niewidzialnoscia
 ai_npc( npc,  np);
// aktualizuj_czary( npc );
}
int qsort_helper(const void *e1, const void *e2)
{
 return ((const lpostac *)e2)->postac.PD - ((const lpostac*)e1)->postac.PD;
}
void tura (lpostac *lpostac)
{
 int popPD=lpostac->postac.PD;
 if (lpostac->postac.stan[STAN_NIEPRZYTOMNY]) { ai_odpoczywaj(lpostac); return; }
 if (lpostac->postac.stan[STAN_STRACH]) { ai_ruch_losowy(lpostac); return; }
 if (lpostac!=lHero) {
  if (lpostac)
   if (!lpostac->postac.martwy) tura_npc(np_gr,lpostac);
 }
 else {
  generuj_bufor();
  rysuj_bufor();
  tura_Hero();
  Hero_test_percepcji();
 }
 if (lpostac->postac.PD==popPD) lpostac->postac.PD-=100;
}
void gra()
{
 koniec=false;
 int np;
 while (!koniec)
 {
  if (nowe_np!=np_gr)
  {
   np_gr=nowe_np;
   x_gr=nowe_x;
   y_gr=nowe_y;
   idz_do(lHero,nowe_x,nowe_y);
  }
  np=np_gr;
  lpostac **tabela=new lpostac*[ zlicz_liste(pietro[np].npc) + 1];//0 to Hero
  lpostac **tabela2=new lpostac*[ zlicz_liste(pietro[np].npc) + 1];
  lpostac *obecny=pietro[np].npc;
  tabela[0]=lHero;
  int ile=1;
  while (obecny)
  {
   tabela[ile]=obecny;
   ile++;
   obecny=obecny->nast;
  }
  for(int i=0;i<ile;i++) tabela2[i]=tabela[i];
  for(int i=0;i<ile;i++) tabela[i]->postac.PD+=licz_PD(tabela[i]);
//  sortuj_npc(tabela,ile);
  //Zrob tabelke wskaznikow razem z Hero, posortuj ja jesli pozycja!=Null to tura
  //wybieraj zywych z najwiekszym PD wiekszym od sto atualizacja czaru pod koniec rundy
  //Do tego miejsca jest ok potem dopiero zaczna sie bledy
  for (int j=0;j<ile;j++)
   if (tabela[j])
   {
    oblicz_postac( tabela[j] ,true);
   }
  int PD_max;
  do
  {
   lpostac *postac_max=tabela[0];
   PD_max=postac_max->postac.PD;
   for(int i=1;i<ile;i++)
   if (tabela[i])
    if (tabela[i]->postac.PD>PD_max && !tabela[i]->postac.martwy)
    {
     postac_max=tabela[i];
     PD_max=postac_max->postac.PD;
    }
   if (PD_max>=100)
    {
     tura(postac_max);
     //postac_max->postac.PD-=100;//jesli cos sie pieprzy to wlasnie tu
     bool odswierz=false;
     for (int j=0;j<ile;j++)
      if (tabela[j])
       if (tabela[j]->postac.martwy)
       {
        zabij_npc(np,tabela[j]); //wielokrotne zabicie
        tabela[j]=NULL;
        odswierz=true;
       }
     if (odswierz)
     {
      generuj_bufor();
      rysuj_bufor();
     }
    }
  } while (PD_max>=100);
  for (int j=0;j<ile;j++)
   if (tabela2[j])
    if (tabela2[j]->postac.martwy)
     {
      usun_npc(np,tabela2[j]);
      tabela2[j]=NULL;
     }
  lczar *c_obecny=pietro[np].czary;
  while (c_obecny)
  {
   efekt_czaru(c_obecny,np);
   c_obecny=c_obecny->nast;
  }
  for (int j=0;j<ile;j++)
   if (tabela2[j])
    if (tabela2[j]->postac.niewidoczny)
     if (!skradaj_sie(tabela2[j]))
     {
      Tpostac &p=tabela2[j]->postac;
      if ( p.stan[STAN_NIEWIDZIALNOSC] && !p.stan[STAN_WYKRYCIE] )
              p.niewidoczny=true;
         else p.niewidoczny=false;
      if ( j==0 && !p.niewidoczny ) info("Wykryty.");
     }
  for (int j=0;j<ile;j++)
   if (tabela2[j])
   {
    aktualizuj_czary( tabela2[j] );
   }
  aktualizuj_czary(np);
  aktualizuj_zdarzenia(np);
  delete [] tabela;
 }
}
void sortuj_npc(lpostac **tab, int dl)
{
// printf("%i ",dl);
 if (dl<2) return;
 qsort(tab, dl, sizeof(lpostac*), qsort_helper);//Zastosuje wybor najwiekszego
}
/*
void gra()
{
 koniec=false;
 while (!koniec)
 {
  lpostac **tabela=new lpostac*[ zlicz_liste(pietro[np_gr].npc) + 1];//1 to Hero
  lpostac *obecny=pietro[np_gr].npc;
  tabela[0]=lHero;
  int ile=1;
  while (obecny)
  {
   tabela[ile]=obecny;
   ile++;
   obecny=obecny->nast;
  }
  for(int i=0;i<ile;i++) tabela[i]->postac.PD+=licz_PD(tabela[i]);
//  sortuj_npc(tabela,ile);
  //Zrob tabelke wskaznikow razem z Hero, posortuj ja jesli pozycja!=Null to tura
  //wybieraj zywych z najwiekszym PD wiekszym od sto atualizacja czaru pod koniec rundy
  int PD_max;
  do
  {
   lpostac *postac_max=tabela[0];
   PD_max=postac_max->postac.PD;
   for(int i=1;i<ile;i++)
    if (tabela[i]->postac.PD>PD_max)
    {
     postac_max=tabela[i];
     PD_max=postac_max->postac.PD;
    }
   if (PD_max>=100)
    {
     tura(postac_max);
     postac_max->postac.PD-=100;//+los(-10,10)
    }
  } while (PD_max>=100);
   for (int j=0;j<ile;j++)
    if (tabela[j])
     if (tabela[j]->postac.martwy)
     {
      zabij_npc(np_gr,tabela[j]);
      generuj_bufor();
      rysuj_bufor();
      tabela[j]=NULL;
     }
  lczar *c_obecny=pietro[np_gr].czary;
  while (c_obecny)
  {
   efekt_czaru(c_obecny,np_gr);
   c_obecny=c_obecny->nast;
  }
  aktualizuj_czary(np_gr);
  aktualizuj_zdarzenia(np_gr);
  delete [] tabela;
 }
}
*/
void wyjscie_z_gry()
{
 niszcz_lochy();
 // niszcz_Hero();
 niszcz_liste(Hero.przedmioty);
 niszcz_liste(Hero.czary);
 delete lHero;
 niszcz_zmienne_globalne();
 exit(1);
}
void koniec_gry_zwyciestwo()
{

}


