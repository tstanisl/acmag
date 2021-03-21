#include "glowny.h"
void wczytaj_opis_czarow(char* sciezka)
{
 printf("otwieram plik\n");
// getch();
 FILE *in=fopen(sciezka,"r");
// getch();
 printf("plik otwarty\n");
// getch();
 int l_czarow;
// getch();
 for (int i=0;i<Lczar;i++)
 {
  opis_czaru[i].poziom=32000;
  opis_czaru[i].mana=0;
  ksiega_czarow[i]=0;
 }
 fscanf(in,"%i",&l_czarow);
 printf("czytam lc %i\n",l_czarow);
// getch();
 for(int i=0;i<l_czarow;i++)
 {
   int nr;
   char nazwa[40];
   fscanf(in,"\n%[^\n]",&nazwa);
   fscanf(in,"\n%i ",&nr);
   strcpy(opis_czaru[nr].nazwa,nazwa);
   printf("\n %s: ",opis_czaru[nr].nazwa);
   printf(" nr=%i",nr);
   fscanf(in,"\n%i %i %i ", &opis_czaru[nr].poziom,
                            &opis_czaru[nr].lista,
                            &opis_czaru[nr].mana);
   printf(" poziom=%i mana=%i ",opis_czaru[nr].poziom,opis_czaru[nr].mana);
 //  getch();
 }
 fclose(in);
// getch();
}
//te funkcje mozna niezle uproscic zmieniajac je na obr_std
void obr_blyskawica(int zrodlo,lpostac *cel,int moc_czaru,int *p=NULL) {
 int obr=los( min(10,moc_czaru), 1, 6);
 if ( rzut_odpornosci(cel,RZUT_UNIK,0) ) obr/=2;
 obrazenia(zrodlo,cel, OBR_ELEKT ,obr);
}
void obr_plonace_dlonie(int zrodlo,lpostac *cel,int moc_czaru,int *p=NULL) {
 int obr=los(1,4)+moc_czaru;
 if ( rzut_odpornosci(cel,RZUT_UNIK,0) ) obr/=2;
 obrazenia(zrodlo,cel, OBR_OGIEN ,obr);
}
void obr_plomienie(int zrodlo,lpostac *cel,int moc_czaru,int *p=NULL) {
 int obr=los( 3, 1, 6);
 if ( rzut_odpornosci(cel,RZUT_UNIK,0) ) obr/=2;
 obrazenia( zrodlo,cel, OBR_OGIEN ,obr);
}
void obr_kula_ognista(int zrodlo,lpostac *cel,int moc_czaru,int *p=NULL) {
 int obr=los( min(10,moc_czaru), 1, 6);
 if ( rzut_odpornosci(cel,RZUT_UNIK,0) ) obr/=2;
 obrazenia(zrodlo,cel, OBR_OGIEN ,obr);
}
void obr_stozek_zimna(int zrodlo,lpostac *cel,int moc_czaru,int *p=NULL) {
 int obr=los( moc_czaru, 1, 6);
 if ( rzut_odpornosci(cel,RZUT_UNIK,0) ) obr/=2;
 obrazenia(zrodlo,cel, OBR_MROZ ,obr);
}
void obr_plugawy_uwiad(int zrodlo,lpostac *cel,int moc_czaru,int *p=NULL) {
 if ( czy_wrogowie(&cel->postac,zrodlo) ) {
   int obr=los( moc_czaru, 1, 8);
   if ( rzut_odpornosci(cel,RZUT_WYT,0) ) obr/=2;
   obrazenia(zrodlo,cel, OBR_MAGIA ,obr);
 }
}
void obr_lodowa_petla(int zrodlo,lpostac *cel,int moc_czaru,int *p=NULL) {
 int obr=los( min(moc_czaru,15), 1, 4);
 if ( rzut_odpornosci(cel,RZUT_UNIK,0) ) obr/=2;
 obrazenia(zrodlo,cel, OBR_MROZ ,obr);
}
void obr_skowyt_banshee(int zrodlo,lpostac *cel,int moc_czaru,int *p=NULL) {
 if ( !cel->postac.stan[STAN_OCHRONA_SMIERC] &&
      !rzut_odpornosci(cel,RZUT_WOLA,0) &&
      czy_wrogowie(&cel->postac,zrodlo) ) {
         cel->postac.martwy=true;
         info("%s umiera.",cel->postac.imie);
         if (zrodlo==1) dodaj_doswiadczenie (lHero, ile_dosw(Hero.poziom,&cel->postac));
       }
}
void obr_trzesienie_ziemi(int zrodlo,lpostac *cel,int moc_czaru,int *p=NULL) {
 int obr=los( moc_czaru,1,4);
 if ( rzut_odpornosci(cel,RZUT_UNIK,0) ) obr/=2;
 obrazenia(zrodlo,cel, OBR_NORMAL ,obr);
}
/* moc-il.kostek
 p[0] - typ_obr;
 p[1] - rodzaj kostki
 p[2] - bonus obr
 p[3] - typ_rzutu obronnego
 p[4] - bonus_rzutu
*/
void obr_standard(int zrodlo,lpostac *cel,int moc_czaru,int *p=NULL) {
 if (!p) {
  info("Cos nie tak w obr_std");
  return;
 }
 int obr=los( moc_czaru,1,p[1])+p[2];
 if (p[3]!=RZUT_BRAK) if( rzut_odpornosci(cel,p[3],p[4]) ) obr/=2;
 obrazenia(zrodlo,cel, p[0] ,obr);
}
void rzuc_plomienie(lpostac *mag, int xc, int yc)
{
 int *obszar;
 int p[10]={xc,yc};
 int dl=generuj_obszar(&obszar,mag->postac.x,mag->postac.y,OBSZAR_ODCINEK,p);
 obszar[0]=0;obszar[1]=0;
 rysuj_obszar(obszar,dl,14,37,20);delay( 1000 );
 atak_obszarowy(mag->postac.sila,obszar,dl,mag->postac.poziom,obr_plomienie);
 delete[] obszar;
}
bool rzuc_kule_ognista(lpostac *mag, int x, int y, int zasieg)
{
 animacja ( mag->postac.x,mag->postac.y,x,y,15,14,50);
 bool pole_razenia[Mobszar][Mobszar];
 int p[10]={zasieg};
 generuj_obszar(pole_razenia,x,y,OBSZAR_WYBUCH,p);
 rysuj_obszar(pole_razenia,x,y,14,15);
 delay(1000);
 obszar_efekt(pole_razenia,x,y,EFEKT_WYPAL_TRAWE);
 atak_obszarowy(mag->postac.sila,pole_razenia,x,y,mag->postac.poziom,obr_kula_ognista);
}
bool rzuc_stozek_zimna(lpostac *mag, int x, int y, int zasieg)
{
 bool pole_razenia[Mobszar][Mobszar];
 int p[10]={mag->postac.x,mag->postac.y,zasieg};
 generuj_obszar(pole_razenia,x,y,OBSZAR_STOZEK,p);
 rysuj_obszar(pole_razenia,mag->postac.x,mag->postac.y,15,37);
 delay(1000);
// obszar_efekt(pole_razenia,x,y,EFEKT_WYPAL_TRAWE);
 atak_obszarowy(mag->postac.sila,pole_razenia,mag->postac.x,mag->postac.y,mag->postac.poziom,obr_stozek_zimna);
}
bool rzuc_plonace_dlonie(lpostac *mag, int x, int y, int zasieg)
{
 bool pole_razenia[Mobszar][Mobszar];
 int p[10]={mag->postac.x,mag->postac.y,zasieg};
 generuj_obszar(pole_razenia,x,y,OBSZAR_STOZEK,p);
 rysuj_obszar(pole_razenia,mag->postac.x,mag->postac.y,14,37);
 delay(1000);
 obszar_efekt(pole_razenia,x,y,EFEKT_WYPAL_TRAWE);
 atak_obszarowy(mag->postac.sila,pole_razenia,mag->postac.x,mag->postac.y,mag->postac.poziom,obr_plonace_dlonie);
}
bool rzuc_plugawy_uwiad(lpostac *mag, int x, int y, int zasieg)
{
 animacja ( mag->postac.x,mag->postac.y,x,y,29,2,50);
 bool pole_razenia[Mobszar][Mobszar];
 int p[10]={zasieg};
 generuj_obszar(pole_razenia,x,y,OBSZAR_WYBUCH,p);
 rysuj_obszar(pole_razenia,x,y,7,1);
 delay(1000);
 atak_obszarowy(mag->postac.sila,pole_razenia,x,y,mag->postac.poziom,obr_plugawy_uwiad);
}
bool rzuc_magiczna_burza(lpostac *mag, int x, int y, int zasieg)
{
 animacja ( mag->postac.x,mag->postac.y,x,y,29,2,50);
 bool pole_razenia[Mobszar][Mobszar];
 int p[10]={zasieg};
 generuj_obszar(pole_razenia,x,y,OBSZAR_WYBUCH,p);
 //rysuj_bufor();
 rysuj_obszar(pole_razenia,x,y,7,1);
 delay(1000);
// atak_obszarowy(mag,pole_razenia,x,y,CZAR_MAGICZNA_BURZA);
}
bool rzuc_skowyt_banshee(lpostac *mag,int x, int y, int zasieg)
{
 bool pole_razenia[Mobszar][Mobszar];
 int p[10]={zasieg};
 generuj_obszar(pole_razenia,x,y,OBSZAR_WYBUCH,p);
 atak_obszarowy(mag->postac.sila,pole_razenia,x,y,mag->postac.poziom,obr_skowyt_banshee);
}
bool rzuc_trzesienie_ziemi(lpostac *mag, int x, int y, int zasieg)
{
 animacja ( mag->postac.x,mag->postac.y,x,y,15,7,50);
 bool pole_razenia[Mobszar][Mobszar];
 int p[10]={zasieg};
 generuj_obszar(pole_razenia,x,y,OBSZAR_KOLO,p);
 obszar_efekt(pole_razenia,x,y,EFEKT_WSTRZASY);
 atak_obszarowy(mag->postac.sila,pole_razenia,x,y,mag->postac.poziom,obr_trzesienie_ziemi);
}

bool rzuc_czar (lpostac *lmag, int x, int y, int czar)
{
 lpostac *lcel;
 if ( jest_Hero(x,y) ) lcel=lHero;
  else lcel=znajdz_npc(x,y);
 Tpostac cel,mag=lmag->postac;
 if (lcel) cel=lcel->postac;
 int obr=0,ile;
 lmag->postac.niewidoczny=false;
 switch (czar)
 {
  case CZAR_USPIENIE:
    if (!lcel) return false;
    animacja ( mag.x,mag.y,x,y,9,9,100);
    if ( cel.poziom<=4  && !rzut_odpornosci(lcel,RZUT_WOLA,0) )
    {
     Tczar efekt={EFEKT_SEN,3+mag.poziom,false};
     dodaj_czar(lcel , efekt );
     info("%s zasypia.",cel.imie);
    }
    else info("%s opiera si© czarowi.",cel.imie);
   break;
  case CZAR_TARCZA://cos sie sypie
    {
     Tczar efekt={EFEKT_TARCZA, min(5*mag.poziom,20), false, 3};
     dodaj_czar(lmag, efekt);
     info("%s rzuca czar Tarcza.",mag.imie);
    }
    break;
  case CZAR_BLOGOSLAWIENSTWO://cos sie sypie
    {
     Tczar efekt={EFEKT_BLOGOSLAWIENSTWO, 2+3*mag.poziom, false};
     dodaj_czar(lmag, efekt);
     info("%s rzuca czar Bˆogosˆawieästwo.",mag.imie);
    }
    break;
  case CZAR_MAGICZNY_POCISK:
    if (!lcel) return false;
    ile=min( (mag.poziom+1)/2, 5 );
    for (int i=0;i<ile;i++) obr+=( los(1,4)+1 );
    animacja ( mag.x,mag.y,x,y,42,4,70);
    obrazenia( lmag->postac.sila,lcel, OBR_MAGIA, obr );
   break;
  case CZAR_PLONACE_DLONIE:
    //if ( !czy_Hero(&cel) )
    rzuc_plonace_dlonie(lmag,x,y,1);
   break;
  case CZAR_PORAZENIE:
    if (!lcel) return false;
    for (int i=0;i<mag.poziom;i++) obr+=los(1,2);
    animacja ( mag.x,mag.y,x,y,158,11,70);
    if (rzut_odpornosci(lcel,RZUT_UNIK,0) ) obr/=2;
    obrazenia( lmag->postac.sila,lcel, OBR_ELEKT, obr );
   break;
  case CZAR_OPLATANIE :
   {
//    animacja ( mag.x,mag.y,x,y,15,7,100);
    int z=2;
    for (int xc=-z;xc<=z;xc++)
     for (int yc=-z;yc<=z;yc++)
      if (nalezy(1,1,pietro[np_gr].dl,pietro[np_gr].sz,xc+x,yc+y) )
       if ( dozw_ruch(pietro[np_gr].mapa[xc+x][yc+y]) )
        if (los(1,100)<40) {
         Tczar efekt={EFEKT_OPLATANIE,10,false,xc+x,yc+y,10,35};
         dodaj_czar(np_gr,efekt,mag.sila);
        }
   }
   break;
  case CZAR_ZAUROCZENIE:
    if (!lcel) return false;
    animacja ( mag.x,mag.y,x,y,9,9,100);
    if ( !rzut_odpornosci(lcel,RZUT_WOLA,-2) )
    {
     Tczar efekt={EFEKT_ZAUROCZENIE, 5 + mag.poziom, false, mag.sila, cel.sila};
     dodaj_czar(lcel , efekt );
     info("%s poddaje si© zauroczeniu.",cel.imie);
    }
    else info("%s opiera si© czarowi.",cel.imie);
   break;
  case CZAR_KWASOWA_STRZALA :
   {
    if (!lcel) return false;
    animacja ( mag.x,mag.y,x,y,45,2,70);
    Tczar efekt={ EFEKT_ZATRUCIE, 1+mag.poziom/3 ,false,2,4,0 };
    dodaj_czar(lcel , efekt ,mag.sila);
   }
   break;
  case CZAR_PLOMIENIE:
    if (!lcel) return false;
//    if ( !czy_Hero(&cel) )
    rzuc_plomienie(lmag,x,y);
   break;
  case CZAR_LODOWA_PETLA:
   {
    animacja ( mag.x,mag.y,x,y,15,15,50);
    bool pole_razenia[Mobszar][Mobszar];
    int p[10]={2};
    generuj_obszar(pole_razenia,x,y,OBSZAR_WYBUCH,p);
    rysuj_obszar(pole_razenia,x,y,15,37);
    delay(1000);
    atak_obszarowy(mag.sila,pole_razenia,x,y,mag.poziom,obr_lodowa_petla);
   }
   break;
  case CZAR_SILA_HEROSA:
    {
     Tczar efekt={EFEKT_ATRYBUT, mag.poziom, false, ST_SF,+2};
     dodaj_czar(lmag, efekt);
     info("%s rzuca czar Siˆa Herosa.",mag.imie);
    }
    break;
  case CZAR_PRZELAMANIE_STRACHU:
    zakoncz_czary(EFEKT_STRACH,lcel);
    {
     Tczar efekt={EFEKT_OCHRONA_STRACH, 5+mag.poziom, false};
     dodaj_czar(lmag, efekt);
    }
    info("%s uodparnia si© na strach.",mag.imie);
   break;
  case CZAR_PAJECZYNA :
   {
    animacja ( mag.x,mag.y,x,y,15,7,100);
    int z=2;
    for (int xc=-1;xc<=1;xc++)
     for (int yc=-1;yc<=1;yc++)
     if (nalezy(1,1,pietro[np_gr].dl,pietro[np_gr].sz,xc+x,yc+y) )
      if ( dozw_ruch(pietro[np_gr].mapa[xc+x][yc+y]) )
      {
       Tczar efekt={EFEKT_PAJECZYNA,15,false,xc+x,yc+y,7,35};
       dodaj_czar(np_gr,efekt,mag.sila);
      }
   }
   break;
  case CZAR_SLEPOTA :
    if (!lcel) return false;
    animacja ( mag.x,mag.y,x,y,9,9,100);
    if ( !rzut_odpornosci (lcel,RZUT_WOLA,0) )
    {
     Tczar efekt={ EFEKT_SLEPOTA, 5 , false };
     dodaj_czar(lcel , efekt );
     info("%s nic nie widzi.",cel.imie);
    }
    else info("%s opiera si© czarowi.",cel.imie);
   break;
  case CZAR_NIEWIDZIALNOSC://cos sie sypie
    {
     Tczar efekt={EFEKT_NIEWIDZIALNOSC, 0, false};
     dodaj_czar(lmag, efekt);
     info("%s staje si© niewidzialny.",mag.imie);
    }
    break;
  case CZAR_WYKRYCIE_NIEWIDZIALNYCH:
    {
     bool pole_razenia[Mobszar][Mobszar];
     int p[10]={mag.wzrok};
     generuj_obszar(pole_razenia,x,y,OBSZAR_WYBUCH,p);
     Tczar efekt={EFEKT_WYKRYCIE, 1, false};
     dodaj_czar(pole_razenia,x,y,efekt,mag.sila);
     info("%s otacza si© aur¥ wykrycia.",mag.imie);
    }
    break;
  case CZAR_OGNISTA_STRZALA:
    if (!lcel) return false;
    obr=los( min( (mag.poziom-1)/4, 6), 4, 24 );
    if ( rzut_odpornosci(lcel,RZUT_UNIK,0) ) obr/=2;
    animacja ( mag.x,mag.y,x,y,15,14,50);
    obrazenia( lmag->postac.sila,lcel, OBR_OGIEN, obr );
   break;
  case CZAR_KULA_OGNISTA:
    //if ( !czy_Hero(&cel) )
    rzuc_kule_ognista(lmag,x,y,2);
   break;
  case CZAR_BLYSKAWICA:
    {
     int *obszar;
     int p[10]={x,y,min(mag.poziom*2,20)};
     int dl=generuj_obszar(&obszar,mag.x,mag.y,OBSZAR_PROSTA,p);
     obszar[0]=0;obszar[1]=0;
     for (int i=0;i<3;i++) {
      rysuj_obszar(obszar,dl,11,158,0);delay( los(50,300) );
      rysuj_obszar(obszar,dl,15,158,0);delay( los(50,300) );
     }
     atak_obszarowy(mag.sila,obszar,dl,mag.poziom,obr_blyskawica);
     delete[] obszar;
    }
   break;
  case CZAR_SPOWOLNIENIE:
    if (!lcel) return false;
    animacja ( mag.x,mag.y,x,y,9,9,100);
    if ( !rzut_odpornosci(lcel,RZUT_WYT,-2) )
    {
     Tczar efekt={ EFEKT_SPOWOLNIENIE, 5 , false };
     dodaj_czar(lcel , efekt );
     info("%s zostaje spowolniony.",cel.imie);
    }
    else info("%s opiera si© czarowi.",cel.imie);
   break;
  case CZAR_ANTIDOTUM:
    zakoncz_czary(EFEKT_ZATRUCIE,lcel);
    info("%s oczyszcza organizm z trucizn.",mag.imie);
   break;
  case CZAR_UNIERUCHOMIENIE:
    if (!lcel) return false;
    animacja ( mag.x,mag.y,x,y,9,9,100);
    if ( !rzut_odpornosci(lcel,RZUT_WOLA,+2) )
    {
     Tczar efekt1={ EFEKT_UNIERUCHOMIENIE, 5 , false };
     Tczar efekt2={ EFEKT_NIEPRZYTOMNY, 5 , false };
     dodaj_czar(lcel , efekt1 );
     dodaj_czar(lcel , efekt2 );
     info("%s stoi w bezruchu.",cel.imie);
    }
    else info("%s opiera si© czarowi.",cel.imie);
   break;
  case CZAR_PSALM:
    {
     Tczar efekt={EFEKT_PSALM, 2*mag.poziom, false};
     dodaj_czar(lmag, efekt);
     info("%s rzuca czar Psalm.",mag.imie);
    }
    break;
  case CZAR_OCHRONA_PRZED_OGNIEM:
   {
     Tczar efekt={EFEKT_OCHRONA_2, mag.poziom,false,OBR_OGIEN};
     dodaj_czar(lmag, efekt);
     info("%s rzuca czar Ochrona Przed Ogniem.",mag.imie);
   }
   break;
  case CZAR_PRZYW_SZKIELETU:
    if ( dozw_ruch(pietro[np_gr].mapa[x][y]) )
     if ( !jest_ktos(x,y) )
     {
      tworz_npc(np_gr,x,y,15,mag.sila,mag.emocje);
      Tczar czar={ EFEKT_PRZYWOLANIE, 20, false};
      dodaj_czar(znajdz_npc(x,y),czar);
      info("%s przyzywa szkielet.",mag.imie);
      break;
     }
    return false;
   break;
  case CZAR_POCISKI_MOCY:
    if (!lcel) return false;
    ile=min( (mag.poziom-2)/5, 5 );
    for (int i=0;i<ile;i++) obr+=( los(2,1,4)+mag.poziom/2 );
    animacja ( mag.x,mag.y,x,y,42,2,70);
    obrazenia( lmag->postac.sila,lcel, OBR_MAGIA, obr );
   break;
  case CZAR_OCHRONA_ZYWIOLY:
   {
     for(int i=OBR_OGIEN;i<=OBR_ELEKT;i++)
     {
      Tczar efekt={EFEKT_OCHRONA_2, mag.poziom,false,i};
      dodaj_czar(lmag, efekt);
     }
     info("%s rzuca czar Ochrona Przed ½ywioˆami.",mag.imie);
   }
   break;
  case CZAR_KAMIENNA_SKORA:
   {
     Tczar efekt={EFEKT_OCHRONA_1, 10,false,OBR_NORMAL};
     dodaj_czar(lmag, efekt);
     info("%s otacza siebie Kamienn¥ Sk¢r¥.",mag.imie);
   }
   break;
  case CZAR_PRZYSPIESZENIE://cos sie sypie
   {
     Tczar efekt={EFEKT_PRZYSPIESZENIE, 5, false};
     dodaj_czar(lmag, efekt);
     info("%s przyspiesza swoje ruchy.",mag.imie);
   }
   break;
  case CZAR_SCIANA_OGNIA:
   {
     Tczar efekt={ EFEKT_OBRAZENIA_OBSZAR, 10,false,x,y,14,37,
                   OBR_OGIEN,mag.poziom,1,2,np_gr};
     dodaj_czar(np_gr, efekt, mag.sila);
     info("%s rozpala pˆon¥c¥ barier©.",mag.imie);
   }
   break;
  case CZAR_BURZA:
   if (pietro[np_gr].czy_na_powierzchni)
   {
     info("%s przywoˆuje burz© z piorunami.",mag.imie);
     Tczar efekt={ EFEKT_OBRAZENIA_PUNKT, 2*mag.poziom,false,x,y,9,21,
                   5,mag.sila,OBR_ELEKT,1,10,mag.poziom / 2};
     dodaj_czar(np_gr, efekt, mag.sila);
   }
   else info("Ten czar mo¾na rzuci† tylko na powierzchni."      );
   break;
  case CZAR_OSLABIENIE :
    if (!lcel) return false;
    animacja ( mag.x,mag.y,x,y,9,9,100);
//    if ( !rzut_odpornosci (lcel,lmag,RZUT_WOLA,+2) )
    {
     Tczar efekt={ EFEKT_OSLABIENIE, mag.poziom , false };
     dodaj_czar(lcel , efekt );
     info("%s traci siˆy.",cel.imie);
    }
//    else info("%s opiera si© czarowi.",cel.imie);
   break;
  case CZAR_STOZEK_ZIMNA:
    if ( !czy_Hero(&cel) ) rzuc_stozek_zimna(lmag,x,y,3);
   break;
  case CZAR_POZOGA:
  {
    animacja ( mag.x,mag.y,x,y,15,14,80);
    int z=2;
    for (int xc=-z;xc<=z;xc++)
     for (int yc=-z;yc<=z;yc++)
      if (nalezy(1,1,pietro[np_gr].dl,pietro[np_gr].sz,xc+x,yc+y) )
       if ( dozw_ruch(pietro[np_gr].mapa[xc+x][yc+y]) )
        if (los(1,100)<40) {
          Tczar efekt={ EFEKT_OBRAZENIA_OBSZAR, 10,false,xc+x,yc+y,14,37,OBR_OGIEN,
                        mag.poziom,1,2,np_gr};
          dodaj_czar(np_gr,efekt,mag.sila);
        }
    info("%s tworzy pˆon¥c¥ stref©.",mag.imie);
  }
  break;
  case CZAR_POLE_STATYCZNE:
   {
     info("%s elektryzuje powietrze.",mag.imie);
     Tczar efekt={ EFEKT_OBRAZENIA_PUNKT, mag.poziom,false,x,y,9,21,
                   3,mag.sila,OBR_ELEKT,mag.poziom / 2,4,1};
     dodaj_czar(np_gr, efekt, mag.sila);
   }
   break;
  case CZAR_OKO_MAGA:
   {
     Tczar efekt={EFEKT_OKO_MAGA, mag.poziom,false};
     dodaj_czar(lmag, efekt);
     info("%s rzuca czar Oko Maga.",mag.imie);
   }
   break;
  case CZAR_WEZWANIE_ZWIERZAT:
    if ( dozw_ruch(pietro[np_gr].mapa[x][y]) )
    {
     int ile=los(3,6);
     for(int i=0;i<ile;i++)
     {
      int xc,yc;
      char id_stwor=los(0,5);
      if ( znajdz_najblizsze_wolne_pole(xc,yc,x,y) )
      {
       tworz_npc(np_gr,xc,yc,id_stwor,mag.sila,mag.emocje);
       Tczar czar={ EFEKT_PRZYWOLANIE, 3+mag.poziom, false};
       dodaj_czar(znajdz_npc(xc,yc),czar);
      }
     }
     info("%s przyzywa wsparcie.",mag.imie);
    }
   break;
  case CZAR_TELEPORT:
    if ( dozw_ruch(pietro[np_gr].mapa[x][y]) )
     if ( !jest_ktos(x,y) )
     {
      lmag->postac.x=x;
      lmag->postac.y=y;
      if (lmag==lHero)
      {
       x_gr=x; y_gr=y;
      }
      info("%s przenika przez wymiary.",mag.imie);
      break;
     }
    return false;
   break;
  case CZAR_PRZYW_ZYWIOLAKA:
    if ( dozw_ruch(pietro[np_gr].mapa[x][y]) )
     if ( !jest_ktos(x,y) )
     {
      tworz_npc(np_gr,x,y,16,mag.sila,mag.emocje);
      Tczar czar={ EFEKT_PRZYWOLANIE, 20, false};
      dodaj_czar(znajdz_npc(x,y),czar);
      info("%s przyzywa ¾ywioˆaka.",mag.imie);
      break;
     }
    return false;
   break;
  case CZAR_PALEC_SMIERCI :
    if (!lcel) return false;
    animacja ( mag.x,mag.y,x,y,29,2,100);
    if ( !cel.stan[STAN_OCHRONA_SMIERC] && !rzut_odpornosci(lcel,RZUT_WYT,0) )
     {
      if (!cel.stan[STAN_OCHRONA_SMIERC])
         lcel->postac.martwy=true;  //problem doswiadczenia
      info("%s umiera.",cel.imie);
      if (lmag==lHero) dodaj_doswiadczenie (lHero, ile_dosw(Hero.poziom,&cel));
     }
     else obrazenia( lmag->postac.sila,lcel, OBR_MAGIA, los(mag.poziom,1,4) );
   break;
  case CZAR_CALUN_PLOMIENI :
    if (!lcel) return false;
    animacja ( mag.x,mag.y,x,y,15,14,50);
    {
     info("%s staje si© ¾yw¥ pochodni¥.",cel.imie);
     Tczar efekt={ EFEKT_OBRAZENIA, mag.poziom , false ,OBR_OGIEN,1,10,mag.poziom};
     dodaj_czar(lcel , efekt );
    }
  break;
  case CZAR_OCHRONA_SMIERC:
   {
     Tczar efekt={EFEKT_OCHRONA_SMIERC, mag.poziom,false};
     dodaj_czar(lmag, efekt);
     info("%s rzuca czar Ochrona przed Smierci¥.",mag.imie);
   }
   break;
  case CZAR_REGENERACJA:
   {
     Tczar efekt={EFEKT_REGENERACJA, 10,false,10};
     dodaj_czar(lmag, efekt);
     info("%s rzuca czar Regeneracja.",mag.imie);
   }
   break;
  case CZAR_STREFA_MROZU:
   {
     info("%s ochˆadza powietrze.",mag.imie);
     Tczar efekt={ EFEKT_OBRAZENIA_STREFA, 5,false,x,y,15,21,
                   2,mag.sila,OBR_MROZ,2,6,mag.poziom / 2 };
     dodaj_czar(np_gr, efekt, mag.sila);
   }
   break;
  case CZAR_OCHRONA_PRZED_MAGIA:
   {
     Tczar efekt={EFEKT_OCHRONA_3, mag.poziom,false,OBR_MAGIA};
     dodaj_czar(lmag, efekt);
     info("%s rzuca czar Ochrona Przed Magi¥.",mag.imie);
   }
   break;
  case CZAR_TRZESIENIE_ZIEMI:
    rzuc_trzesienie_ziemi(lmag,x,y,5);
   break;
  case CZAR_PLUGAWY_UWIAD:
    rzuc_plugawy_uwiad(lmag,x,y,2);
   break;
  case CZAR_SM_OGLUSZENIE:
   if (!lcel) return false;
   animacja ( mag.x,mag.y,x,y,29,2,100);
   if ( cel.obr<100)
   {
    int czas=1+los(1,6);
    Tczar efekt1={EFEKT_UNIERUCHOMIENIE,czas ,false};
    Tczar efekt2={EFEKT_NIEPRZYTOMNY,czas ,false};
    dodaj_czar(lcel, efekt1);
    dodaj_czar(lcel, efekt2);
    info("%s zostaje ogˆuszony.",cel.imie);
   }
   break;
  case CZAR_BRAMA_PIEKIEL:
    if ( dozw_ruch(pietro[np_gr].mapa[x][y]) )
     if ( !jest_ktos(x,y) )
     {
      tworz_npc(np_gr,x,y,23,Lsil-1,WROGI);
      Tczar czar={ EFEKT_PRZYWOLANIE, 30, false};
      dodaj_czar(znajdz_npc(x,y),czar);
      info("%s otwiera Bram© Piekieˆ.",mag.imie);
      return true;
     }
    return false;
   break;
  case CZAR_NIEWRAZLIWOSC://cos sie sypie
    {
     Tczar efekt={EFEKT_NIEWRAZLIWOSC, 5, false};
     dodaj_czar(lmag, efekt);
     info("%s rzuca czar Niewra¾liwo˜†.",mag.imie);
    }
    break;
  case CZAR_SKOWYT_BANSHEE:
    info("%s rzuca czar Skowyt Banshee.",mag.imie);
    rzuc_skowyt_banshee(lmag,mag.x,mag.y,3);
   break;
  case CZAR_LEKKIE_LECZENIE:
    cel.obr=min(cel.obr + 8, cel.max_obr);
    lcel->postac=cel;
//    info("%s rzuca czar lecz¥cy.",mag.imie);
   break;
  case CZAR_SREDNIE_LECZENIE:
    cel.obr=min(cel.obr + 17, cel.max_obr);
    lcel->postac=cel;
//    info("%s rzuca czar lecz¥cy.",mag.imie);
   break;
  case CZAR_ZAAWANSOWANE_LECZENIE:
    cel.obr=min(cel.obr + 24, cel.max_obr);
    lcel->postac=cel;
//    info("%s rzuca czar lecz¥cy.",mag.imie);
   break;
  case CZAR_UZDROWIENIE:
    cel.obr = cel.max_obr;
    lcel->postac=cel;
    info("%s rzuca czar Uzdrowienie.",mag.imie);
   break;
 }
 int od_PD= 100 + 20 * opis_czaru[czar].poziom
            - 2 * premia_u(lmag->postac.umiejetnosc[UM_M_SKUTECZNOSC]);
 //if (od_PD>100) od_PD=100;
 lmag->postac.PD-=od_PD;
 return true;
}
bool efekt_czaru_modyfikator(lpostac *postac, lczar *lczar)
{
 Tczar czar=lczar->czar;
 switch (czar.rodzaj)
 {
   case EFEKT_OCHRONA_1:
        if (czar.p[0]<Lodpornosci)
        {
         //postac->postac.odpornosc_DT[czar.p[0]]+=czar.p[1];
         if (postac->postac.odpornosc_DR[czar.p[0]]<30 )
            postac->postac.odpornosc_DR[czar.p[0]]=30;//czar.p[2];
        }
        break;
   case EFEKT_OCHRONA_2:
        if (czar.p[0]<Lodpornosci)
        {
         //postac->postac.odpornosc_DT[czar.p[0]]+=czar.p[1];
         if (postac->postac.odpornosc_DR[czar.p[0]]<70 )
            postac->postac.odpornosc_DR[czar.p[0]]=70;//czar.p[2];
        }
        break;
        break;
   case EFEKT_OCHRONA_3:
        if (czar.p[0]<Lodpornosci)
        {
         //postac->postac.odpornosc_DT[czar.p[0]]+=czar.p[1];
         if (postac->postac.odpornosc_DR[czar.p[0]]<100 )
            postac->postac.odpornosc_DR[czar.p[0]]=100;//czar.p[2];
        }
        break;
   case EFEKT_TARCZA:
        if (!postac->postac.stan[STAN_TARCZA])
        {
         postac->postac.KP+=czar.p[0];
         postac->postac.stan[STAN_TARCZA]=true;
        }
        break;
   case EFEKT_BLOGOSLAWIENSTWO:
        if (!postac->postac.stan[STAN_BLOGOSLAWIENSTWO])
        {
         postac->postac.KP+=1;
         postac->postac.rzut[RZUT_WYT]+=2;
         postac->postac.rzut[RZUT_UNIK]+=2;
         postac->postac.rzut[RZUT_WOLA]+=2;
         postac->postac.stan[STAN_BLOGOSLAWIENSTWO]=true;
        }
        break;
   case EFEKT_PSALM:
        if (!postac->postac.stan[STAN_PSALM])
        {
         postac->postac.KP+=2;
         postac->postac.rzut[RZUT_WOLA]+=2;
         postac->postac.stan[STAN_PSALM]=true;
        }
        break;
   case EFEKT_RZUTY:
        if (czar.p[0]<Lrzut) postac->postac.rzut[czar.p[0]]+=czar.p[1];
        break;
   case EFEKT_ATRYBUT:
        if (czar.p[0]<Lstat)
        {
//         int stary_stat=postac->postac.stat[czar.p[0]];
         postac->postac.stat[czar.p[0]] =
           max( postac->postac.stat[czar.p[0]],
                postac->postac.bazowe_stat[czar.p[0]] + czar.p[1] );
        }
        break;
   case EFEKT_UMIEJETNOSC:
        if (czar.p[0]<Lumiejetnosci)
        {
//         int stara_um=postac->postac.umiejetnosc[czar.p[0]];
         postac->postac.umiejetnosc[czar.p[0]]=
           max( postac->postac.umiejetnosc[czar.p[0]],
                postac->postac.bazowe_umiejetnosc[czar.p[0]]+czar.p[1]);
        }
        break;
   case EFEKT_OCHRONA_SMIERC:
        postac->postac.stan[STAN_OCHRONA_SMIERC]=true;
        break;
   case EFEKT_OCHRONA_STRACH:
        postac->postac.stan[STAN_OCHRONA_STRACH]=true;
        break;
 }
 return true;
}
bool efekt_czaru(lpostac *postac, lczar *lczar) //true jesli zyje  //false nie zyje
{
 //printf("czar na %s. ",postac->postac.imie);getch();
 Tczar czar=lczar->czar;
 switch (czar.rodzaj)
 {
   case EFEKT_ZATRUCIE:
        postac->postac.stan[STAN_ZATRUCIE]=true;
        return obrazenia(czar.zrodlo,postac,OBR_KWAS, los (czar.p[0],1,czar.p[1]) + czar.p[2] );
        break;
   case EFEKT_KRWOTOK:
        postac->postac.stan[STAN_KRWOTOK]=true;
        return obrazenia(czar.zrodlo,postac,OBR_NORMAL, los (czar.p[0],1,czar.p[1]) + czar.p[2] );
        break;
   case EFEKT_OBRAZENIA:
        return obrazenia(czar.zrodlo,postac,czar.p[0], los (czar.p[1],1,czar.p[2]) + czar.p[3] );
        break;
   case EFEKT_REGENERACJA:
        postac->postac.obr=min(postac->postac.obr+czar.p[0],postac->postac.max_obr);
//        printf("%s reg.   ",postac->postac.imie);
        postac->postac.stan[STAN_REGENERACJA]=true;
        break;
   case EFEKT_SEN:
        postac->postac.stan[STAN_NIEPRZYTOMNY]=true;
        break;
   case EFEKT_NIEPRZYTOMNY:
        postac->postac.stan[STAN_NIEPRZYTOMNY]=true;
        break;
   case EFEKT_STRACH:
        postac->postac.stan[STAN_STRACH]=true;
        break;
   case EFEKT_SLEPOTA:
        postac->postac.wzrok=1;
        postac->postac.stan[STAN_SLEPOTA]=true;
        break;
   case EFEKT_ZAUROCZENIE:
        if (czar.ile_do_konca>0) postac->postac.sila=czar.p[0];
           else postac->postac.sila=czar.p[1];
        break;
   case EFEKT_UNIERUCHOMIENIE:
        postac->postac.stan[STAN_UNIERUCHOMIENIE]=true;
        break;
   case EFEKT_NIEWIDZIALNOSC:
        if (!postac->postac.stan[STAN_WYKRYCIE])
        {
         postac->postac.stan[STAN_NIEWIDZIALNOSC]=true;
         postac->postac.niewidoczny=true;
        }
        break;
   case EFEKT_WYKRYCIE:
        postac->postac.stan[STAN_WYKRYCIE]=true;
        postac->postac.stan[STAN_NIEWIDZIALNOSC]=false;
        postac->postac.niewidoczny=false;
        break;
   case EFEKT_OSLABIENIE:
        if (!postac->postac.stan[STAN_OSLABIENIE])
        {
         postac->postac.KP-=4;
         postac->postac.rzut[RZUT_WYT]-=4;
         postac->postac.rzut[RZUT_UNIK]-=4;
         postac->postac.rzut[RZUT_WOLA]-=4;
         postac->postac.stan[STAN_OSLABIENIE]=true;
        }
        break;
   case EFEKT_PRZYSPIESZENIE:
        postac->postac.stan[STAN_PRZYSPIESZENIE]=true;
        break;
   case EFEKT_SPOWOLNIENIE:
        postac->postac.stan[STAN_SPOWOLNIENIE]=true;
        break;
   case EFEKT_NIEWRAZLIWOSC:
        postac->postac.stan[STAN_NIEWRAZLIWOSC]=true;
        break;
   case EFEKT_OKO_MAGA:
        {
         bool pole_razenia[Mobszar][Mobszar];
         int p[10]={postac->postac.wzrok};
         generuj_obszar(pole_razenia,postac->postac.x,postac->postac.y,OBSZAR_WYBUCH,p);
         Tczar efekt={EFEKT_WYKRYCIE, 1, false};
         dodaj_czar(pole_razenia,postac->postac.x,postac->postac.y,efekt,postac->postac.sila);
        }
        break;
 }
 return true;
}
bool efekt_czaru(lczar* lczar,int np)
{
 if (np<0) np=np_gr;
 Tczar czar=lczar->czar;
 switch (czar.rodzaj)
 {
   case EFEKT_ZMIANA_TERENU:
        pietro[np].mapa[czar.p[0]][czar.p[1]]=czar.p[2];
        break;
   case EFEKT_OBRAZENIA_OBSZAR:
        obrazenia(czar.zrodlo,czar.p[0],czar.p[1],czar.p[4],los(czar.p[5],czar.p[6],czar.p[7]),czar.p[8]);
        break;
   case EFEKT_OBRAZENIA_STREFA:   //x,y,zn,kol,4-zasieg,sila_zrodla,typ,7-n_kost,t_kost,bonus,10-typ_rzutu,bonus_rzutu
        {
         bool pole_razenia[Mobszar][Mobszar];
         int p1[10]={czar.p[4]},
             p2[10]={czar.p[6],czar.p[8],czar.p[9],RZUT_BRAK};
         generuj_obszar(pole_razenia,czar.p[0],czar.p[1],OBSZAR_WYBUCH,p1);
         atak_obszarowy( czar.zrodlo,pole_razenia,czar.p[0],czar.p[1],czar.p[7],obr_standard,p2);
        }
        break;
   case EFEKT_OBRAZENIA_PUNKT://x,y,zn,kol,zasieg,sila_zrodla,typ,n_kost,t_kost,bonus,typ_rzutu,bonus_rzutu
        {
         int x,y;
         lpostac *lcel=znajdz_najblizszego_widocznego_wroga(czar.p[0],czar.p[1],czar.p[5]);
         if (lcel)
          if ( odleglosc(czar.p[0],czar.p[1],lcel->postac.x,lcel->postac.y)<=czar.p[4] )
           obrazenia(czar.zrodlo,lcel->postac.x,lcel->postac.y,czar.p[6],los(czar.p[7],1,czar.p[8])+czar.p[9]);
        }
        break;
   case EFEKT_OPLATANIE:
   case EFEKT_PAJECZYNA:
        {
         lpostac* lcel=znajdz_postac(czar.p[0],czar.p[1],np);
         if (lcel)
          if (!lcel->postac.stan[STAN_UNIERUCHOMIENIE])
          if ( !rzut_odpornosci(lcel,RZUT_UNIK,0) )
          {
           Tczar efekt={EFEKT_UNIERUCHOMIENIE,5,false};
           dodaj_czar(lcel,efekt,czar.zrodlo);
           info("%s zostaje unieruchomiony.",lcel->postac.imie);
          }
        }
        break;
 }
}
void dodaj_czar( lpostac *cel, Tczar czar,int zrodlo)
{
 lczar *stary=cel->postac.czary;
 cel->postac.czary=new lczar;
 cel->postac.czary->czar=czar;////PROBLEM Z TABELKA P
 cel->postac.czary->czar.aktywny=true;
 cel->postac.czary->czar.tryb=0;
 cel->postac.czary->nast=stary;
 cel->postac.czary->czar.zrodlo=zrodlo;
 efekt_czaru(cel,cel->postac.czary);
 efekt_czaru_modyfikator(cel,cel->postac.czary);
 if (czar.ile_do_konca==0) usun_czar (cel->postac.czary,cel);
}
void dodaj_czar( Topis_przedmiotu& cel, Tczar czar)
{
 lczar *stary=cel.czary;
 cel.czary=new lczar;
 cel.czary->czar=czar;////PROBLEM Z TABELKA P
 cel.czary->czar.aktywny=true;
 cel.czary->nast=stary;
 cel.czary->czar.zrodlo=0;
}
void dodaj_czar( bool obszar[Mobszar][Mobszar], int x, int y, Tczar czar,int zrodlo)
{
 lpostac* cel;
 for (int i=0;i<Mobszar;i++)
  for (int j=0;j<Mobszar;j++)
  {
   int xn=x+i-Mobszar/2;
   int yn=y+j-Mobszar/2;
   if (obszar[i][j])
    if ( nalezy(1,1,pietro[np_gr].dl,pietro[np_gr].sz,xn,yn) )
    {
      if ( jest_Hero(xn,yn) ) dodaj_czar(lHero,czar,zrodlo);
       else {
              cel=znajdz_npc(xn,yn);
              if (cel) dodaj_czar(cel,czar,zrodlo);
            }
    }
  }
}
void dodaj_czar( int np, Tczar czar,int  zrodlo )
{
 lczar *stary=pietro[np].czary;
 pietro[np].czary=new lczar;
 pietro[np].czary->czar=czar;////PROBLEM Z TABELKA P
 pietro[np].czary->czar.aktywny=true;
 pietro[np].czary->czar.tryb=0;
 pietro[np].czary->nast=stary;
 pietro[np].czary->czar.zrodlo=zrodlo;
 efekt_czaru(pietro[np].czary,np);
}
bool czy_jest_efekt (lpostac *postac, lczar *czar)
{
 int stary_rodzaj=czar->czar.rodzaj;
 lczar *obecny=postac->postac.czary;
 while(obecny)
 {
  if (obecny->czar.rodzaj==stary_rodzaj)
   if (obecny!=czar) return true;
  obecny=obecny->nast;
 }
 return false;
}
bool efekt_koniec(lpostac *lpostac, lczar *czar)
{
 Tpostac *postac=&(lpostac->postac);
 switch (czar->czar.rodzaj)
 {
   case EFEKT_ZATRUCIE:
        if ( czy_Hero(postac) )
           info("Czujesz si© lepiej.");
        break;
   case EFEKT_SEN:
        info("%s budzi si©.",postac->imie);
        break;
   case EFEKT_STRACH:
        info("%s przeˆamuje strach.",postac->imie);
        break;
   case EFEKT_SLEPOTA:
        info("%s odzyskuje wzrok.",postac->imie);
        break;
   case EFEKT_ZAUROCZENIE:
        postac->sila=czar->czar.p[1];
        info("%s odzyskuje wˆadze umysˆowe.",postac->imie);
        break;
   case EFEKT_TARCZA:
   case EFEKT_BLOGOSLAWIENSTWO:
   case EFEKT_PSALM:
//   case EFEKT_KAMIENNA_SKORA:
        info("%s traci magiczn¥ barier© ochronn¥.",postac->imie);
        break;
   case EFEKT_OCHRONA_1:
   case EFEKT_OCHRONA_2:
   case EFEKT_OCHRONA_3:
        info("%s traci magiczn¥ barier© ochronn¥.",postac->imie);
        break;
   case EFEKT_PRZYWOLANIE:
        info("%s rozpada si© w pyˆ.",postac->imie);
        lpostac->postac.martwy=true;
        return false;
        break;
   case EFEKT_NIEWIDZIALNOSC:
        info("%s staje si© widzialny.",postac->imie);
        break;
   case EFEKT_ATRYBUT:
        info("%s - czar wzmacniaj¥cy ulega rozproszeniu.",postac->imie);
        break;
   case EFEKT_PRZYSPIESZENIE:
        {
         Tczar efekt={ EFEKT_SPOWOLNIENIE, 3 , false };
         dodaj_czar(lpostac , efekt );
         info("%s zostaje spowolniony.",postac->imie);
        }
        break;
 }
 return true;
}
void usun_efekt(lpostac *postac, int rodzaj )
{
 lczar *nowy,*obecny=postac->postac.czary;
 while(obecny)
 {
  nowy=obecny->nast;
  if (obecny->czar.rodzaj==rodzaj) zakoncz_czar(obecny,postac);
  obecny=nowy;
 }
}
bool zakoncz_czar(lczar* czar,lpostac *postac )
{
 if ( !czy_jest_efekt(postac,czar) ) efekt_koniec(postac,czar);
 usun_czar(czar,postac);
}
bool zakoncz_czary(int rodzaj,lpostac *postac )
{
 lczar *obecny=postac->postac.czary;
 while (obecny)
 {
  if (obecny->czar.rodzaj==rodzaj) obecny->czar.ile_do_konca=0;
  obecny=obecny->nast;
 }
}
void usun_czar( lczar* czar ,lpostac *postac)
{
 lczar *obecny=postac->postac.czary, *pop=obecny;
 if (obecny==czar) postac->postac.czary=postac->postac.czary->nast;
 else
 {
  while (obecny)
  {
   if (obecny==czar) break;
   pop=obecny;
   obecny=obecny->nast;
  }
  pop->nast=obecny->nast;
 }
 if (obecny==NULL) return;
 delete obecny;
}
void aktualizuj_czary(lpostac *postac)
{
 lczar *nowy,*obecny=postac->postac.czary;
 while (obecny)
 {
  nowy=obecny->nast;
  if ( !obecny->czar.staly )
  {
   obecny->czar.ile_do_konca--;
   if (obecny->czar.ile_do_konca==0) zakoncz_czar(obecny,postac);
    else if (obecny->czar.ile_do_konca<0) usun_czar(obecny,postac);
  }
  obecny=nowy;
 }
}
bool efekt_koniec(lczar *lczar,int np=-1)
{
 if (np<0) np=-1;
 Tczar czar=lczar->czar;
 switch (czar.rodzaj)
 {
   case EFEKT_ZMIANA_TERENU:
        pietro[np].mapa[czar.p[0]][czar.p[1]]=czar.p[3];
        break;
   case EFEKT_OBRAZENIA_OBSZAR:
        break;
 }
 return true;
}
void usun_czar( lczar* czar ,int np=-1 )
{
 if (np<0) np=np_gr;
 lczar *obecny=pietro[np].czary, *pop=obecny;
 if (obecny==czar) pietro[np].czary=pietro[np].czary->nast;
 else
 {
  while (obecny)
  {
   if (obecny==czar) break;
   pop=obecny;
   obecny=obecny->nast;
  }
  pop->nast=obecny->nast;
 }
 if (obecny==NULL) return;
 delete obecny;
}
bool zakoncz_czar(lczar* czar,int np=-1 )
{
 if (np<0) np=np_gr;
 usun_czar(czar,np);
 efekt_koniec(czar,np);
 return true;
}
void aktualizuj_czary(int np)
{
 if (np<0) np=np_gr;
 lczar *nowy,*obecny=pietro[np].czary;
 while (obecny)
 {
  nowy=obecny->nast;
  if ( !obecny->czar.staly )
  {
   obecny->czar.ile_do_konca--;
   if (obecny->czar.ile_do_konca==0) zakoncz_czar(obecny,np);
    else if (obecny->czar.ile_do_konca<0) usun_czar(obecny,np);
  }
  obecny=nowy;
 }
}


