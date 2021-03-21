#include "config.h"
#include "glowny.h"

char znak_terenu(char teren)
{
 return opis_terenu[teren].znak;
}

int kolor_teren(char teren)
{
 return opis_terenu[teren].kolor;
}
bool dozw_ruch(char teren)
{
 return opis_terenu[teren].dozw_ruch;
}
bool przezroczysty(char teren)
{
 return opis_terenu[teren].przezroczysty;
}
int PD_terenu(char teren) {
 return opis_terenu[teren].PD_ruch;
}
char znak_odkoduj(char znak)
{
 switch (znak) {
   //case
  case 44  :return  LOCHY;
  case 35  :return  MUR;
  case -15 :return  TRAWA;
  case 61  :return  WODA;
  case -98 :return  PLOT; //158
  case -80 :return  PIASEK; //176
  case 94  :return  GORY;
  case 2   :return  CZASZKA;
  case 127 :return  SKALA;
  case 126 :return  BAGNO;
  case 30  :return  DRZEWO;
  case -17 :return  POPIOL;
  case 47  :return  DRZWI_O;
  case 43  :return  DRZWI_Z;
  case 21  :return  PULAPKA;
 }
}
bool czy_puste(int x, int y, int np)
{
 if (np<0) np=np_gr;
 if (nalezy(1,1,pietro[np].dl,pietro[np].sz,x,y) )
  if ( dozw_ruch(pietro[np].mapa[x][y]) )
   if ( !jest_ktos(x,y,np) ) return true;
 return false;
}
bool znajdz_najblizsze_wolne_pole(int &xh,int &yh,int xp, int yp, int np)
{
 if (np<0) np=np_gr;
 int kierunek=los(1,4);
 int ile=1,x=xp,y=yp;
 bool parzyste=false;
 while ( (abs(xp-x)+abs(yp-y) ) < 10)
 {
  for(int i=0;i<ile;i++)
  {
   if ( czy_puste(x,y,np) )
   {
    xh=x;
    yh=y;
    return true;
   }
   switch (kierunek)
   {
    case 1: y--;break;
    case 2: x++;break;
    case 3: y++;break;
    case 4: x--;break;
   }
  }
  if (parzyste) ile++;
  parzyste=!parzyste;
  kierunek++;
  if (kierunek>4) kierunek=1;
 }
 return false;
}
bool znajdz_najblizsze_pole(int &xh,int &yh,int xp, int yp, char pole, int np)
{
 if (np<0) np=np_gr;
 int kierunek=los(1,4);
 int ile=1,x=xp,y=yp;
 bool parzyste=false;
 while ( (abs(xp-x)+abs(yp-y) ) < 10)
 {
  for(int i=0;i<ile;i++)
  {
   if (nalezy(1,1,pietro[np].dl,pietro[np].sz,x,y) )
    if ( pietro[np].mapa[x][y]==pole )
    {
     xh=x;
     yh=y;
     return true;
    }
   switch (kierunek)
   {
    case 1: y--;break;
    case 2: x++;break;
    case 3: y++;break;
    case 4: x--;break;
   }
  }
  if (parzyste) ile++;
  parzyste=!parzyste;
  kierunek++;
  if (kierunek>4) kierunek=1;
 }
 return false;
}
int okolica(int x,int y,char znak,int np)  //przenies do global
{
 if(np<0) np=np_gr;
 int i,j,licznik=0;
 for (i=-1;i<2;i++)
  for (j=-1;j<2;j++)
   if (nalezy(1,1,pietro[np_gr].dl,pietro[np_gr].sz,x+i,y+j) )
    if (pietro[np].mapa[x+i][y+j]==znak) licznik++;
 return licznik;
}
int los(int a1,int a2)
{
 return rand()%(a2-a1+1)+a1;
}
int los(int n,int a1,int a2)
{
 int odp=0;
 for(int i=0;i<n;i++) odp+=los(a1,a2);
 return odp;
}
int wybierz_losowo(int n,...) {
 va_list ap;
 int arg,i=rand()%n;
 va_start(ap, n);
 while (i>=0) {
  arg = va_arg(ap,int);
  i--;
 }
 va_end(ap);
 return arg;
}
float odleglosc(float x1, float y1, float x2, float y2)
{
 return sqrt( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );
}
int odleglosc_ruch(int xp, int yp, int xc, int yc)
{
 return max(abs(xc-xp),abs(yc-yp));
}
bool nalezy(int x1,int y1,int x2,int y2,int x,int y)
{
 if (x>=x1 && x<=x2 && y>=y1 && y<=y2) return true;
 return false;
}
bool nalezy(int x1,int x2,int x)
{
 if (x>=x1 && x<=x2) return true;
 return false;
}
void wypeln_tablice(char* tab,int x,int wartosc)
{
 for(int i=0;i<x;i++) tab[i]=wartosc;
}
void wypeln_tablice(char **tab,int x,int y,int wartosc)
{
 for (int i=0;i<x;i++)
  for (int j=0;j<y;j++) tab[i][j]=wartosc;
}
/*
void wypeln_tablice(char tab[],int wartosc)
{
 for(int i=0;i<sizeof(tab)/sizeof(*tab);i++) tab[i]=wartosc;
}
void wypeln_tablice(char tab[][],int wartosc)
{
 for (int i=0;i<sizeof(tab)/sizeof(*tab);i++)
  wypeln_tablice(tab[i],wartosc) ;
}*/
char upcase(char znak)
{
 if (znak>96) return znak-32;
 return znak;
}
void dodaj_do_ekwipunku( Tprzedmiot przedmiot ,Tpostac *Postac)
{
 if (przedmiot.typ==0) return;
 Topis_przedmiotu dane=opis(przedmiot);
 lprzedmiot *stary=NULL;
 if (dane.tworzy_stos)
     stary=szukaj_w_ekwipunku(Postac,przedmiot.typ,przedmiot.numer,dane.p[1]);
 if (stary!=NULL) stary->przedmiot.ilosc+=przedmiot.ilosc;
 else
 {
  lprzedmiot *nowy=Postac->przedmioty;
  Postac->przedmioty=new lprzedmiot;
  Postac->przedmioty->przedmiot=przedmiot;
  Postac->przedmioty->nast=nowy;
 }
}

void usun_z_ekwipunku( lprzedmiot *przedmiot, Tpostac *postac )
{
 lprzedmiot *obecny=postac->przedmioty, *pop=obecny;
 if (obecny==przedmiot) postac->przedmioty=postac->przedmioty->nast;
 {
  while (obecny)
  {
   if (obecny==przedmiot) break;
   pop=obecny;
   obecny=obecny->nast;
  }
  pop->nast=obecny->nast;
 }
 delete obecny;
}
void usun_z_przedmiotow( lprzedmiot *przedmiot, int np )
{
 if (np<0) np=np_gr;
 lprzedmiot *obecny=pietro[np].przedmioty, *pop=obecny;
 if (obecny==przedmiot) pietro[np].przedmioty=pietro[np].przedmioty->nast;
 {
  while (obecny)
  {
   if (obecny==przedmiot) break;
   pop=obecny;
   obecny=obecny->nast;
  }
  pop->nast=obecny->nast;
 }
 delete obecny;
}
void dodaj_do_przedmiotow( int np,int x, int y, Tprzedmiot przedmiot)
{
 //printf("%i %i",x,y);getch();
 if (przedmiot.typ==0) return;
 lprzedmiot *lista=pietro[np].przedmioty;
 lprzedmiot *stary=lista;
 lista=new lprzedmiot;
 lista->przedmiot=przedmiot;
 lista->przedmiot.x=x;
 lista->przedmiot.y=y;
 lista->nast=stary;
 pietro[np].przedmioty=lista;
}
bool _czy_widzi (int xp, int yp, int xk, int yk)
{
 //if (xp==xk) {textattr(15);printf("xp=%i yp=%i xk=%i yk=%i",xp,yp,xk,yk);getch();}
 if ( !nalezy(1,1,pietro[np_gr].dl,pietro[np_gr].sz,xp,yp) ||
      !nalezy(1,1,pietro[np_gr].dl,pietro[np_gr].sz,xk,yk) ) return false;
 float odl=sqrt(float( (xk-xp)*(xk-xp) + (yk-yp)*(yk-yp) ));
 if (abs(xk-xp)<2 && abs(yk-yp)<2 ) return true; /////Jesli pola obok to koncz
 float dx=float(xk-xp)/odl,dy=float(yk-yp)/odl;
 float dt=sqrt(dx*dx+dy*dy);
 //printf("dt=%f",dt);
 float x=0,y=0;
 float d=0;
 bool koniec=false;
 //if (xp==xk) printf("xp=%i yp=%i START",xp,yp);
 while (!koniec)
 {
   //if (xp==xk) {printf("\nx=%f y=%f xw=%i yw=%i  ",x,y,xp+int(x),yp+int(y));getch();}
   if (!przezroczysty(pietro[np_gr].mapa[int(x)+xp][int(y)+yp])
       || d>=odl) {
                  koniec=true;
                 }
   d+=dt;
   x+=dx;
   y+=dy;
   //textattr(15);gotoxy(int(x)+40,int(y)+13);cprintf("x");
 }
 //printf("KONIEC");getch();clrscr();
 if ((d-dt)<odl) return false;
 return true;
}
bool czy_widzi (int xp, int yp, int xk, int yk)
{
 if ( _czy_widzi(xp,yp,xk,yk) ||
      _czy_widzi(xk,yk,xp,yp) ) return true;
 return false;
}
int licz_KP(lpostac *postac)
{
 Topis_przedmiotu helm=opis(postac->postac.nasobie[NS_GLOWA]);
 Topis_przedmiotu zbroja=opis(postac->postac.nasobie[NS_TULOW]);
 Topis_przedmiotu tarcza=opis(postac->postac.nasobie[NS_L_REKA]);
 int KP=0;
 if (postac->postac.nasobie[NS_GLOWA].typ!=0 ) KP+=helm.p[1];
 if (postac->postac.nasobie[NS_TULOW].typ!=0 ) KP+=zbroja.p[1];
 if (postac->postac.nasobie[NS_L_REKA].typ!=0) KP+=tarcza.p[1];
 return KP;
}
int licz_PD(lpostac *lpostac)
{
 Tpostac p=lpostac->postac;
 int wyj=100+rasa[p.rasa].szybkosc+5*premia(p.stat[ST_ZR]);
 if (p.stan[STAN_PRZYSPIESZENIE]) wyj+=50;
 if (p.stan[STAN_SPOWOLNIENIE]) wyj-=50;
 if (wyj<0) wyj=0;
// printf("PD=%i ",wyj);
 return wyj;
}
int premia (int stat)
{
 return stat/2-5;
}
int premia_u (int umiejetnosc)
{
 if (umiejetnosc<1) return -5;
 return umiejetnosc;
}
lpostac* znajdz_najblizszego_wroga(lpostac *postac)
{
 return znajdz_najblizszego_wroga (
        postac->postac.x,postac->postac.y,postac->postac.sila );
}
lpostac* znajdz_najblizszego_wroga(int x,int y,int sila)
{
  if (!pietro[np_gr].npc) return NULL;
 long int d,d_min=1000000;
 Tpostac npc;
 lpostac *wrog=NULL;
 lpostac *obecny=pietro[np_gr].npc;
 while(obecny)
 {
  npc=obecny->postac;
 // printf("%s ",npc.imie);
  //if (!obecny->postac.niewidoczny)
  if ( czy_wrogowie(&npc,sila) )
  {
   d= (npc.x-x)*(npc.x-x) +
      (npc.y-y)*(npc.y-y);
   if (d<d_min)
   {
    d_min=d;
    wrog=obecny;
   }
  }
  obecny=obecny->nast;
 }
 //if (!Hero.niewidoczny)
 if ( czy_wrogowie(&Hero,sila) )
 {
  d= (Hero.x-x)*(Hero.x-x) +
     (Hero.y-y)*(Hero.y-y);
  if (d<d_min) wrog=lHero;
 }
 return wrog;
}
lpostac* znajdz_najblizsza_postac(lpostac *postac)
{
 int x=postac->postac.x,
     y=postac->postac.y;
 postac->postac.x=-1000;
 postac->postac.y=-1000;
 lpostac *odp=znajdz_najblizsza_postac (x,y,postac->postac.sila );
 postac->postac.x=x;
 postac->postac.y=y;
 return odp;
}
lpostac* znajdz_najblizsza_postac(int x,int y,int sila)
{
  if (!pietro[np_gr].npc) return NULL;
 long int d,d_min=1000000;
 Tpostac npc;
 lpostac *wrog=NULL;
 lpostac *obecny=pietro[np_gr].npc;
 while(obecny)
 {
  npc=obecny->postac;
 // printf("%s ",npc.imie);
  if ( !obecny->postac.niewidoczny ||
       ( czy_wrogowie(&npc,sila) && obecny->postac.niewidoczny) )
  {
   d= (npc.x-x)*(npc.x-x) +
      (npc.y-y)*(npc.y-y);
   if (d<d_min)
   {
    d_min=d;
    wrog=obecny;
   }
  }
  obecny=obecny->nast;
 }
 if (!Hero.niewidoczny ||
     (czy_wrogowie(&Hero,sila) && Hero.niewidoczny) )
 {
  d= (Hero.x-x)*(Hero.x-x) +
     (Hero.y-y)*(Hero.y-y);
  if (d<d_min) wrog=lHero;
 }
 return wrog;
}

lpostac* znajdz_najblizszego_widocznego_wroga(int x,int y,int sila)
{
 if (!pietro[np_gr].npc) return NULL;
 long int d,d_min=1000000;
// Tpostac postac=start->postac;
 Tpostac npc;
 lpostac *wrog=NULL;
 lpostac *obecny=pietro[np_gr].npc;
 while(obecny)
 {
  npc=obecny->postac;
 // printf("%s ",npc.imie);
  if (!obecny->postac.niewidoczny)
  if ( czy_wrogowie(&npc,sila) )
  {
   d= (npc.x-x)*(npc.x-x) +
      (npc.y-y)*(npc.y-y);
   if (d<d_min)
    if ( czy_widzi(npc.x,npc.y,x,y) )
    {
     d_min=d;
     wrog=obecny;
    }
  }
  obecny=obecny->nast;
 }
 if (!Hero.niewidoczny)
 if ( czy_wrogowie(&Hero,sila) )
 {
  d= (Hero.x-x)*(Hero.x-x) +
     (Hero.y-y)*(Hero.y-y);
  if (d<d_min)
   if (czy_widzi(Hero.x,Hero.y,x,y) ) wrog=lHero;
 }
 return wrog;
}
lpostac* znajdz_najblizszego_widocznego_wroga(lpostac *postac)
{
 return znajdz_najblizszego_widocznego_wroga (
        postac->postac.x,postac->postac.y,postac->postac.sila );
}
bool czy_wrogowie(Tpostac *postac1, Tpostac *postac2)
{
 if ( pobierz_ai_status( postac1->sila, postac2->sila) < -10) return true;
 return false;
}
bool czy_wrogowie(Tpostac *postac1, char sila2)
{
 if ( pobierz_ai_status( postac1->sila, sila2) < -10) return true;
 return false;
}
bool czy_wrogowie(char s1, char s2)
{
 if ( pobierz_ai_status( s1, s2) < -10) return true;
 return false;
}

bool czy_sprzymierzency(Tpostac *postac1, Tpostac *postac2)
{
 if ( pobierz_ai_status( postac1->sila, postac2->sila) > 10) return true;
 return false;
}
bool czy_neutralni(Tpostac *postac1, Tpostac *postac2)
{
 if ( nalezy( -10, 10,pobierz_ai_status( postac1->sila, postac2->sila) ) )
    return true;
 return false;
}
int pobierz_ai_status(char s1, char s2, int np)
{
 if (np<0) np=np_gr;
 return pietro[np].ai_status[s1][s2];
}
int zmien_ai_status(char s1, char s2, char ile, int np)
{
 if (np<0) np=np_gr;
 if (ile<-10) ile=-10;
 if (ile>10) ile=10;
// pietro[np].ai_status[s1][s2]+=ile;
// pietro[np].ai_status[s2][s1]+=ile;
 ustaw_ai_status(s1,s2,pietro[np].ai_status[s1][s2]+ile,np);
 return pietro[np].ai_status[s1][s2];
}
void ustaw_ai_status(char s1, char s2, char ile, int np)
{
 if (np<0) np=np_gr;
 if (ile<-30) ile=-30;
 if (ile>30) ile=30;
 pietro[np].ai_status[s1][s2]=ile;
 pietro[np].ai_status[s2][s1]=ile;
// printf("[np=%i] ai_st[%i][%i]=%i",np,s1,s2,pietro[np].ai_status[s1][s2]);//getch();
}
bool czy_Hero(Tpostac *postac)
{
 if ( postac->x==Hero.x && postac->y==Hero.y) return true;
 return false;
}
bool jest_ktos (int x,int y,int np)
{
  if (np<0) np=np_gr;
  if ( jest_Hero(x,y,np) ) return true;
   else return jest_npc(x,y,np);
}
bool jest_Hero (int x,int y,int np)
{
  if (np<0) np=np_gr;
  if (x==Hero.x && y==Hero.y) return true;
  return false;
}
bool jest_pulapka(int x,int y,int np)
{
 if (np<0) np=np_gr;
 lpulapka *obecny=pietro[np].pulapki;
 while (obecny)
 {
  if (obecny->pulapka.x==x && obecny->pulapka.y==y) return true;
  obecny=obecny->nast;
 }
 return false;
}
void oblicz_postac(lpostac *lpostac,bool licz_srodowisko)
{
 Tpostac *postac=&(lpostac->postac);
 for(int i=0;i<Lumiejetnosci;i++)
  postac->umiejetnosc[i]=postac->bazowe_umiejetnosc[i];
 for(int i=0;i<Lstat;i++) postac->stat[i]=postac->bazowe_stat[i];
 if (postac->martwy) return;
 postac->wzrok= min ( 8, 5 +
                (premia(postac->stat[ST_IN]) +
                 premia(postac->stat[ST_MD]) )/2 );
 if (postac->wzrok<1) postac->wzrok=1;
 if (postac->poziom>10)
 {
  postac->max_obr=( 3 + postac->stat[2]/2 ) * 10;
  postac->max_obr+=max(1, postac->stat[2]/2-5 )*(postac->poziom-10);
 }
 else postac->max_obr=( 3 + postac->stat[2]/2 )*postac->poziom;
 if (postac->stat[3]>10) postac->max_mana=max(0, (postac->stat[3]-8) * postac->poziom / 3 );
 else postac->max_mana=max(0, postac->poziom / (12-postac->stat[3]) );
 postac->rzut[0]=postac->stat[2]/2+postac->poziom/2;
 postac->rzut[1]=postac->stat[1]/2+postac->poziom/2;
 postac->rzut[2]=postac->stat[4]/2+postac->poziom/2;
 postac->PdA=0;
 postac->KP=licz_KP(lpostac);
 for (int i=0;i<Lodpornosci;i++)
 {
  postac->odpornosc_DR[i]=0;
  postac->odpornosc_DT[i]=0;
 }
 if (!licz_srodowisko) return;
 for (int i=0;i<Lstan;i++) postac->stan[i]=false;
 postac->waga_ekwipunku=0;
 for(int i=0;i<Lnasobie;i++)
 {
  if (postac->nasobie[i].typ!=0)
  {
   Topis_przedmiotu opisP=opis(postac->nasobie[i]);
   if (opisP.tworzy_stos) postac->waga_ekwipunku+=postac->nasobie[i].ilosc*opisP.masa;
    else postac->waga_ekwipunku+=opisP.masa;
   lczar *obecny=opisP.czary;
   while (obecny)
   {
    if (obecny->czar.tryb==1) dodaj_czar(lpostac,obecny->czar);////POPRAW problem polega na postaciach jakie przestaja istniec
    obecny=obecny->nast;
   }
  }
 }
 {
  lczar *obecny=lpostac->postac.czary;
  while (obecny)
  {
    if (obecny->czar.aktywny) efekt_czaru_modyfikator(lpostac,obecny);
    // if (!) return;//koncz gdy postac nie zyje
    obecny=obecny->nast;
  }
  obecny=lpostac->postac.czary;
  while (obecny)
  {
    if (obecny->czar.aktywny) efekt_czaru(lpostac,obecny);
    // if (!) return;//koncz gdy postac nie zyje
    obecny=obecny->nast;
  }
 }
 {
  lprzedmiot *obecny=lpostac->postac.przedmioty;
   while (obecny)
  {
    Topis_przedmiotu opisP=opis(obecny->przedmiot);
    if (opisP.tworzy_stos) postac->waga_ekwipunku+=obecny->przedmiot.ilosc*opisP.masa;
     else postac->waga_ekwipunku+=opisP.masa;
    obecny=obecny->nast;
  }
 }
}
bool rzut_odpornosci( lpostac *cel,  int rodzaj_rzutu,int bonus)
{
 int rzut=los(1,20);
 if (rzut==20) return false;
 if (rzut==1) return true;
 int P=-bonus;
 if ( rodzaj_rzutu >= Lrzut) P+=licz_KP(cel);
  else P+=cel->postac.rzut[rodzaj_rzutu];
 if (rzut<=P) return true;
 return false;
}
int kostka(char *kostka)
{
  int rzut = 0, sciany = 0, ile = 0, bonus = 0, znak = 0;
  char *c;
  c = kostka;
  while ( *c && nalezy('0','9',*c) )
  {
    ile *= 10;
    ile += *c - '0';
    c++;
  }
  if (!*c) return ile;
  if (*c != 'k')
    info("Niewˆasciwy format kostki (1)");
  c++;
  while (*c && nalezy('0','9',*c) )
  {
    sciany *= 10;
    sciany += *c - '0';
    c++;
  }
  if (*c)
  {
    if (*c == '-') znak = -1;
    else if (*c == '+') znak = +1;
         else info("Niewˆasciwy format kostki (2)");
    c++;
    while (*c && nalezy('0','9',*c) )
    {
      bonus *= 10;
      bonus += *c - '0';
      c++;
    }
    if (*c) info("Niewˆasciwy format kostki (3)");
  }
  for (int i = 0; i < ile; i++) rzut += los( 1, sciany );
  return (int) (rzut + (znak * bonus));
}
void delay(int j)
{
 struct timeb t;
 ftime(&t);
 unsigned char pop=t.millitm/10;
 int stop=(pop+j)%1000;
 int i=j/10;
 while (i>0) {
  ftime(&t);
  if (pop!=t.millitm/10) i--;
  pop=t.millitm/10;
 }
}


