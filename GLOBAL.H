#ifndef __GLOBALNE__
#define __GLOBALNE__

#include "glowny.h"

#define min(x,y)     (((x) < (y)) ? (x) : (y))
#define max(x,y)     (((x) > (y)) ? (x) : (y))
#define abs(x)       (((x) >= 0) ? (x) : (-(x)))
#define sgn(x)       (((x) >= 0) ? ((x == 0) ? 0 : 1) : -1)
#define Hero lHero->postac

enum Temocje { WROGI=-5,OBOJETNY=0,PRZYJAZNY=5 };

struct Tlokacja {
 int x1,y1,x2,y2;
};
struct Topis_terenu {
 char nazwa[20];
 char znak,kolor;
 bool przezroczysty,dozw_ruch;
 int PD_ruch;
};  
struct Tczar {
 int rodzaj;
 int ile_do_konca;
 bool staly;
 int p[10];
 bool aktywny;
 int czas_rzucenia;
 char tryb;//0-sam 1-osoba noszaca  2-osoba trafiona
 int zrodlo;
 int moc_czaru;
};

typedef struct ogniwo_czar
{
 Tczar czar;
 struct ogniwo_czar *nast;
}  lczar;

struct Topis_przedmiotu {
 char nazwa[40];
 int p[10];
 float masa,cena;
 int wyt;
 char kolor;
 bool tworzy_stos;
 bool ma_ladunki;
 int ladunki;
 lczar *czary;
};
struct Topis_czaru {
 char nazwa[40];
 int poziom,mana;
 char lista;
 /* 0=뾠ie푟i Ochrony  1=Magiczna Energia  2=뾠ie푟i umys늱
   3=Nekromancja  4=Prawa Ognia  5=Prawa Mrozu
    6=Prawa Powietrza  7=뾠ie푟i Natury  8=뾠ie푟i Zmys닣w
   9=Przemiany  10=뾠ie푟i Ciemno쁟i//20 */
};

struct Tprzedmiot {
 int typ, //nic,b1,b2,zb,hl,tar,pociski,magic,quest,bizut,klej
     numer;
 int ilosc;
 int wyt;
 int x,y;
 //bool stan[Lstan_przedmiot];
};

typedef struct ogniwo_przedmiot
{
 Tprzedmiot przedmiot;
 struct ogniwo_przedmiot *nast;
} lprzedmiot;

struct Trasa {
 char nazwa[40];
 int stat[Lstat], //SF,ZR,WYT,INT,IN,CH,WG,
     umiejetnosc[Lumiejetnosci],
     szybkosc,obr,bazowe_dosw;
};

struct Tpostac {
 int bazowe_stat[Lstat], //SF,ZR,WYT,INT,IN,CH,WG,
     bazowe_umiejetnosc[Lumiejetnosci]; //wWr,bT,bO,bS,
 int stat[Lstat], //SF,ZR,WYT,INT,IN,CH,WG,
     umiejetnosc[Lumiejetnosci], //wWr,bT,bO,bS,
     poziom,
     rasa,
     profesja, // woj=0, zlodziej , mag , kaplan ,
     znak,kolor;
 int wzrok,
     x,y,x_p,y_p,
     obr, max_obr,
     mana,max_mana;
 int PD,KP,do_KP,PdA;
 int odpornosc_DT[Lodpornosci],
     odpornosc_DR[Lodpornosci];
 int rzut[Lrzut];//Wyt Unik Wola
 long int dosw;
 float mnoznik_dosw;
 char imie[40];
 float waga_ekwipunku,udzwig;
 lprzedmiot *przedmioty;
 Tprzedmiot nasobie[Lnasobie];
 char sila;
 Temocje emocje;
 bool martwy,niewidoczny;
 bool stan[Lstan];
 lczar *czary;
 int id;
};

struct Tschody {
 bool aktywne;
 int x,y;
 int n_pietra,n_schodow;
};
struct Tpulapka {
 bool aktywna;
 int rodzaj;
 int trudnosc;
 int wyt;
 bool znana;
 long int kod;
 int x,y;
 int *zrodlo;
};
struct Tdrzwi {
 bool otwarte,zablokowane,ukryte;
 int trudnosc,
     wyt;
 long int kod;
 int x,y;
// Tpulapka *pulapka;
};
typedef struct ogniwo_postac
{
 Tpostac postac;
 struct ogniwo_postac *nast;
} lpostac;
typedef struct ogniwo_pulapka
{
 Tpulapka pulapka;
 struct ogniwo_pulapka *nast;
} lpulapka;
typedef struct ogniwo_drzwi
{
 Tdrzwi drzwi;
 struct ogniwo_drzwi *nast;
} ldrzwi;
typedef struct ogniwo_schody
{
 Tschody schody;
 struct ogniwo_schody *nast;
} lschody;
typedef struct ogniwo_zdarzenie
{
 bool (*zdarzenie)();
 int p[10];
 struct ogniwo_zdarzenie *nast;
}  lzdarzenie;

struct Tpietro {
 char nazwa[40];
 int dl,sz;
 char** mapa;
 char** mgla;
 char ai_status[Lsil][Lsil]; //0-nikt 1-hero itd
 char flaga[Lflag];
 lprzedmiot *przedmioty;
 lpostac *npc;
 lschody *schody;
 ldrzwi *drzwi;
 lczar *czary;
 lpulapka *pulapki;
 lzdarzenie *zdarzenia;
 bool czy_istnieje,czy_na_powierzchni;
};

extern bool koniec;
extern char **bufor_z,**bufor_c;
extern int np_gr,nowe_np;
extern int x_gr,y_gr,nowe_x,nowe_y;
extern struct Trasa *rasa;
extern lpostac *lHero;
extern struct Tprzedmiot Pnic;
extern struct Tpostac Pnikt;
extern struct Topis_przedmiotu **opis_przedmiotu;
extern struct Topis_czaru opis_czaru[Lczar];
extern struct Topis_terenu opis_terenu[Lteren];
extern struct Tpostac *opis_npc[Lnpc];
extern struct Tpietro *pietro;
extern int ksiega_czarow[Lczar];
extern char* zdolnosci[];
extern char* umiejetnosci[];

char znak_terenu(char teren);
int kolor_teren(char znak);
bool dozw_ruch(char pole);
bool przezroczysty(char pole);
int PD_terenu(char teren);
char znak_odkoduj(char znak);
bool czy_puste(int x, int y, int np=-1);
bool znajdz_najblizsze_wolne_pole(int &xh,int &yh,int xp, int yp, int np=-1);
bool znajdz_najblizsze_pole(int &xh,int &yh,int xp, int yp, char pole, int np=-1);
int okolica(int x,int y,char znak,int np=-1);
int los(int a1,int a2);
int los(int n,int a1,int a2);
int wybierz_losowo(int n,...);
float odleglosc(float x1, float y1, float x2, float y2);
int odleglosc_ruch(int xp, int yp, int xc, int yc);
bool nalezy(int x1,int y1,int x2,int y2,int x,int y);
bool nalezy(int x1,int x2,int x);
void wypeln_tablice(char* tab,int x,int wartosc);
void wypeln_tablice(char **tab,int x,int y,int wartosc);
char upcase(char znak);
void dodaj_do_ekwipunku( Tprzedmiot przedmiot ,Tpostac *Postac);
void usun_z_ekwipunku( lprzedmiot *przedmiot, Tpostac *postac );
void dodaj_do_przedmiotow( int n_pietra, int x, int y, Tprzedmiot przedmiot);
void usun_z_przedmiotow( lprzedmiot *przedmiot, int np=-1 );
bool czy_widzi (int xp, int yp, int xk, int yk);
lpostac* znajdz_najblizszego_wroga(lpostac *postac);
lpostac* znajdz_najblizszego_wroga(int x,int y,int sila);
lpostac* znajdz_najblizszego_widocznego_wroga(lpostac *start);
lpostac* znajdz_najblizszego_widocznego_wroga(int x,int y,int sila);
lpostac* znajdz_najblizsza_postac(int x,int y,int sila);
lpostac* znajdz_najblizsza_postac(lpostac *postac);
bool czy_wrogowie(Tpostac *postac1, Tpostac *postac2);
bool czy_wrogowie(Tpostac *postac1, char s2);
bool czy_sprzymierzency(Tpostac *postac1, Tpostac *postac2);
bool czy_neutralni(Tpostac *postac1, Tpostac *postac2);
int pobierz_ai_status(char s1, char s2, int np=-1);
int zmien_ai_status(char s1, char s2, char ile, int np=-1);
void ustaw_ai_status(char s1, char s2, char ile, int np=-1);
bool czy_Hero(Tpostac *postac);
bool jest_ktos (int x,int y,int np=-1);
bool jest_Hero (int x,int y,int np=-1);
bool jest_pulapka(int x,int y,int np=-1);
int licz_KP(lpostac *postac);
int licz_PD(lpostac *lpostac);
int premia (int stat);
int premia_u (int umiejetnosc);
void oblicz_postac(lpostac *postac,bool licz_srodowisko=false);
bool rzut_odpornosci( lpostac *cel, int rodzaj_rzutu,int bonus);
int kostka(char *kostka);
void delay(int t);
//void przypisz(Tpostac &p1, Tpostac p2);
//void przypisz(Tczar &p1, Tczar p2);

#endif
