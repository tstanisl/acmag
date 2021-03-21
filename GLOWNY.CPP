#include "glowny.h"
bool koniec;
char **bufor_z,**bufor_c;
int np_gr,nowe_np;
int x_gr,y_gr,nowe_x,nowe_y;
struct Trasa *rasa;
//struct Tpostac Hero;
lpostac *lHero;
struct Tprzedmiot Pnic;//={0,0,0,0,0};
struct Tpostac Pnikt;
struct Topis_przedmiotu **opis_przedmiotu;
struct Topis_czaru opis_czaru[Lczar];
struct Topis_terenu opis_terenu[Lteren];
struct Tpostac *opis_npc[Lnpc];
struct Tpietro *pietro;
//struct Tekwipunek ekwipunek;
int ksiega_czarow[Lczar]={0};
char lista_zadan[Lzadan]={0};
char* zdolnosci[]={ "ZDOLNOóCI BOJOWE","MAGIA","LISTY CZAR‡W",
                    "PODST®PY","SZTUKA SùOWA","RZEMIOSùO"};
char* stany[]={ "SEN","UNIERUCHOMIENIE","ZATRUCIE","PRZYSPIESZENIE",
                "SPOWOLNIENIE","NIEWRAZLIWOSC","SZAL","NIEPRZYTOMNY",
                "TARCZA","SLEPOTA","NIEWIDZIALNOSC","WYKRYCIE","OSLABIENIE",
                "BLOGOSLAWIENSTWO","OCHRONA_SMIERC","KRWOTOK","PSALM","STRACH",
                "OCHRONA_STRACH" };
char* umiejetnosci[]={ "Walka wrecz","Bronie tn•ce","Bronie obuchowe",
                       "Bronie strzeleckie","Bronie drzewcowe",
                       "Bronie miotane","Bronie egzotyczne", //6
                       "SkutecznoòÜ","Medytacja","Alchemia", //9
                       "ócieæki Ochrony","Magiczna Energia",
                       "ócieæki umysàu","Nekromancja","Prawa Ognia",
                       "Prawa Mrozu","Prawa Powietrza","ócieæki Natury",
                       "ócieæki Zmysà¢w","Przemiany",
                       "ócieæki Ciemnoòci","Modlitwy",//20
                       "Skradanie","Zaskakiwanie","Zàodziejstwo",
                       "Poszukiwanie", //24
                       "Elokwencja","Kupiectwo","J©zyki",  //27
                       "Naprawa","Otwieranie zamk¢w","Rozbrajanie puàapek"}; //30
                       //Naprawa zepsutych przedmiotow
int main()
{
 tworz_zmienne_globalne();
 srand(time(0));
 textattr(15);
 clrscr();
 textmode(C80);
 _setcursortype(_NOCURSOR);
 //for (int i=0;i<10;i++) printf(" %i",wybierz_losowo(4,2,4,6,8));
 intro();
 textattr(15);
 /*clrscr();*/
 wczytaj_opis_terenu("dane/teren.dat");
 wczytaj_opis_przedmiotow("dane/przed.dat");
 wczytaj_opis_czarow("dane/czary.dat");
 wczytaj_rasy("dane/rasy.dat");
 wczytaj_opis_npc("dane/npc.dat");
 tworz_lochy(Lpieter);
 tworz_Hero();
 np_gr=0;
 int x=30,y=15;
// int x=7,y=7;
/* bool stop=false;
 do
  {
   x=los(1,pietro[np_gr].dl);
   y=los(1,pietro[np_gr].sz);
   if ( dozw_ruch(pietro[np_gr].mapa[x][y]) )
    if (!jest_npc(x,y)) stop=true;
  } while (!stop);*/
 Hero.x=x_gr=x;
 Hero.y=y_gr=y;
 generuj_bufor();
 clrscr();
// printf( "lpostac=%i Tpietro=%i lprzdmiot=%i ",
//         sizeof(lpostac), sizeof(Tpietro), sizeof(lprzedmiot) );
 //getch();
 rysuj_bufor();
// piszXY(1,1,15,"%i ",kostka("1k6+2") );
// getch();
 gra();
 wyjscie_z_gry();
}
