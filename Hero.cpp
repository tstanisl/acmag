#include "glowny.h"

void tworz_Hero()
{
 lHero=new lpostac;
 lHero->nast=NULL;
 Hero.martwy=false;
 Hero.niewidoczny=false;
 Hero.emocje=PRZYJAZNY;//OBOJETNY;//
 Hero.sila=1;
 Hero.czary=NULL;
 Hero.przedmioty=NULL;
 Hero.PD=0;
 Hero.id=ID_HERO;
 for (int i=0;i<Lnasobie;i++)
 {
  Hero.nasobie[i].typ=0;
  Hero.nasobie[i].numer=0;
  Hero.nasobie[i].wyt=0;
  Hero.nasobie[i].ilosc=1;
 }
 for (int j=0;j<Lstan;j++) Hero.stan[j]=false;
 clrscr();
 piszXY(2,2,15,"(W)czytaj czy (T)worz now¥ posta† ?  (ESC - wyj˜cie z gry)");
 char odp=0;
 do
 {
  odp=upcase(getch());
  switch(odp)
  {
   case 'W': wczytaj_Hero();return; break;
   case ESC: wyjscie_z_gry(); break;
  }
 } while (odp!='T');
 wybierz_rase();
 ustaw_statystyki();
// Hero.waga_ekwipunku=0;
 ///Hero.PD=0;//licz_PD(lHero);
 Hero.poziom=0; //Ten kawalek do poprawki w ostatecznej wersji
 for (int i=0;i<Lumiejetnosci;i++) Hero.bazowe_umiejetnosc[i]=0;
 for (int i=0;i<Lczar;i++) ksiega_czarow[i]=0;
 for (int i=0;i<1;i++) awans_na_poziom(lHero);
 while(kbhit());
 char imie[30]="";
 int dl=0;
 clrscr();
 piszXY(10,2,15,"Podaj imi© postaci : ");
 char znak=0;
 while (dl<1 || znak!=ENTER)
 {
  znak=getch();
  if (nalezy('A','Z',upcase(znak)) && dl<20)
  {
   dl++;
   piszXY(30+dl,2,15,"%c",znak);
   imie[dl-1]=znak;
  }
  if (znak==BACK && dl>0)
  {
   piszXY(30+dl,2,15," ");
   imie[dl-1]=0;
   dl--;
  }
 }
 strcpy(Hero.imie,imie);
 //Hero.poziom=1;
 Hero.dosw=Hero.poziom*1000-1000;
 int i;
 oblicz_postac(lHero);
 Hero.obr=Hero.max_obr;
 Hero.mana=Hero.max_mana;
/*
 Hero.nasobie[0].typ=4;
 Hero.nasobie[0].numer=4;
 Hero.nasobie[1].typ=3;
 Hero.nasobie[1].numer=11;
 Hero.nasobie[2].typ=1;
 Hero.nasobie[2].numer=7;
 Hero.nasobie[3].typ=5;
 Hero.nasobie[3].numer=4;
 Hero.nasobie[4].typ=6;
 Hero.nasobie[4].numer=7;
 Hero.nasobie[4].wyt=30; */
 Hero.nasobie[1].typ=3;
 Hero.nasobie[1].numer=0;
 Hero.nasobie[2].typ=2;
 Hero.nasobie[2].numer=0;
 Hero.nasobie[7].typ=0;
 Hero.nasobie[7].numer=0;
 Hero.nasobie[7].wyt=0;
 int max_ST=ST_SF;
//     ile_max=Hero.stat[ST_SF];
 for (int i=ST_ZR;i<=ST_CH;i++)
  if (Hero.stat[i]>Hero.stat[max_ST]) max_ST=i;

 Tprzedmiot mikstura={7,0,1};
 Tprzedmiot antidotum={7,1,1};
 Tprzedmiot zbroja={3,2,1};
 Tprzedmiot luk={2,2,1};
 Tprzedmiot strzaly={6,0,15};
 Tprzedmiot miecz={1,2,1};
 Tprzedmiot rozdzka={7,4,10};
 Tprzedmiot pierscien={10,2,1};

 dodaj_do_ekwipunku(mikstura,&Hero);
 switch (max_ST)
 {
  case ST_SF: dodaj_do_ekwipunku(miecz,&Hero);
       break;
  case ST_ZR: dodaj_do_ekwipunku(luk,&Hero);
              dodaj_do_ekwipunku(strzaly,&Hero);
       break;
  case ST_KO: dodaj_do_ekwipunku(zbroja,&Hero);
              dodaj_do_ekwipunku(mikstura,&Hero);
       break;
  case ST_IN: dodaj_do_ekwipunku(rozdzka,&Hero);
       break;
  case ST_MD: dodaj_do_ekwipunku(mikstura,&Hero);
              dodaj_do_ekwipunku(antidotum,&Hero);
       break;
  case ST_CH: dodaj_do_ekwipunku(mikstura,&Hero);
              dodaj_do_ekwipunku(pierscien,&Hero);
       break;
 }

// Tprzedmiot pierscien={10,5,1};
//dodaj_do_ekwipunku(pierscien,&Hero);

// Tprzedmiot kusza={2,9,1};
// dodaj_do_ekwipunku(kusza,&Hero);
 for(i=0;i<Lodpornosci;i++)
 {
  Hero.odpornosc_DT[i]=0;
  Hero.odpornosc_DR[i]=0;
 }
 Hero.znak=38;
 Hero.kolor=los(1,15);
//
 int j=0,ile_czarow=(Hero.stat[3]+2)/4;
 pietro[0].flaga[0]=1;
 tworz_schody(0,1,29,4,true);
 tworz_schody(0,4,51,19,false);
 tworz_schody(0,7,40,26,false);
 tworz_schody(0,10,54,38,false);
/*
//Hero.stan[STAN_NIEWRAZLIWOSC]=true;
// if (ile_czarow>0)
*/
 // int pc=(Hero.stat[3]+1)/2; /// maksymalny poziom czarow
 // pc=min( (Hero.poziom + 1) / 2, pc );

// lHero->postac=Hero;

}
void Hero_smierc()
{
  info("  Twoj hero zginaˆ ! ",Hero.imie);
  delay(2000);
  getch();
  wyjscie_z_gry();
}
bool Hero_podnies_przedmiot()
{
 int i=zlicz_przedmioty(x_gr,y_gr);
 if (i==0)
 {
  piszXY(1,1,15,"Tu nie ma ¾adnych przedmiot¢w.");
  getch();
  return false;
 }
 lprzedmiot **przed=new lprzedmiot*[i];
 lprzedmiot *obecny=pietro[np_gr].przedmioty;
 int j=0;
 while (obecny)
 {
  if (obecny->przedmiot.x==x_gr && obecny->przedmiot.y==y_gr)
  {
   przed[j]=obecny;
   j++;
  }
  obecny=obecny->nast;
 }
 clrscr();
 textcolor(15);
 Topis_przedmiotu opisP;
 Tprzedmiot P;
 for(j=0;j<i;j++)
 {
  gotoxy(1,j+2);
  cprintf( "%c)",j+65);
  P=przed[j]->przedmiot;
  opisP=opis(P);
  if (opisP.tworzy_stos && P.ilosc>1) cprintf(" %i x", P.ilosc);
  cprintf(" %s", opisP.nazwa);
  if (opisP.ma_ladunki) cprintf(" ( %i ˆadunk¢w )",P.ilosc);
 }
 bool koniec=false;
 char odp=upcase(getch());
 if (odp>64 && odp<(65+i))
  {
   Tprzedmiot podn=przed[odp-65]->przedmiot;
   Topis_przedmiotu Opodn=opis(podn);
   if (Opodn.tworzy_stos && podn.ilosc>1)
   {
    Tprzedmiot nowy=przed[odp-65]->przedmiot;
    int ile;
    piszXY(1,1,15,"Podaj liczb© (1-%i Enter - wszystko ) : ",nowy.ilosc);
    if (Hero_podaj_liczbe(ile,nowy.ilosc))
     if ( nalezy(1,nowy.ilosc,ile) )
      if (nowy.ilosc==ile)
      {
       dodaj_do_ekwipunku ( przed[odp-65]->przedmiot, &Hero);
       usun_z_przedmiotow ( przed[odp-65] );
      }
      else
      {
       nowy.ilosc=ile;
       dodaj_do_ekwipunku ( nowy, &Hero);
       przed[odp-65]->przedmiot.ilosc-=ile;
      }
   }
   else
   {
    dodaj_do_ekwipunku ( przed[odp-65]->przedmiot, &Hero);
    usun_z_przedmiotow ( przed[odp-65] );
   }
   koniec=true;
  }
 else
  {
   if (nalezy('A','Z',odp) ) piszXY(1,1,15,"Zˆa litera.    ");
  }
  //u
// printf("i=%i  przed adr=%i",i,przed);getch(); ///cos z pamiecia
 delete [] przed;
 return koniec;
}
bool Hero_wyrzuc_przedmiot()
{
 int j,i=zlicz_liste(Hero.przedmioty);
 char odp;
 //ungetch(32); ////Aby pozbyc sie getcha
 pisz_ekwipunek();
 bool koniec=false;
 if (i>0)
 {
   odp=upcase(getch())-64;
   //gotoxy(1,1);cprintf("odp=%c i=%i",odp,i);
   if (odp>0 && odp<=i)
   {
    piszXY(1,1,15,"Wyrzucasz przedmiot.    ");
    lprzedmiot *obecny=Hero.przedmioty,*pop=obecny;
    int j=1;
    while (obecny)
    {
     if (j==odp) break;
     pop=obecny;
     obecny=obecny->nast;
     j++;
    }
    Tprzedmiot podn=obecny->przedmiot;
    Topis_przedmiotu Opodn=opis(podn);
    if (Opodn.tworzy_stos && podn.ilosc>1)
    {
     Tprzedmiot nowy=obecny->przedmiot;
     int ile;
     piszXY(1,1,15,"Podaj liczb© (1-%i Enter - wszystko ) : ",nowy.ilosc);
     if (Hero_podaj_liczbe(ile,nowy.ilosc))
      if ( nalezy(1,nowy.ilosc,ile) )
       if (nowy.ilosc==ile)
       {
        dodaj_do_przedmiotow ( np_gr,x_gr,y_gr,obecny ->przedmiot);
        usun_z_ekwipunku ( obecny, &Hero );
       }
       else
       {
        nowy.ilosc=ile;
        dodaj_do_przedmiotow ( np_gr,x_gr,y_gr,nowy );
        obecny->przedmiot.ilosc-=ile;
       }
   }
   else
   {
    dodaj_do_przedmiotow(np_gr,x_gr,y_gr, obecny->przedmiot );
    usun_z_ekwipunku(obecny,&Hero);
   }
    koniec=true;
   }
   else
   {
    piszXY(1,1,15,"Zˆa litera.    ");
   }
 }
 return koniec;
}

bool Hero_zdejmij_uzbrojenie()
{
  pisz_uzbrojenie();
  char odp=upcase(getch());
  bool koniec=false;
  //gotoxy(1,1);cprintf("odp=%c i=%i",odp,i);
  if (odp>64 && odp<(65+Lnasobie))
  {
   // piszXY(1,1,15,"Wyrzucam przedmiot.    ");
   dodaj_do_ekwipunku( Hero.nasobie[odp-65] , &Hero);
   Hero.nasobie[odp-65]=Pnic;
   koniec=true;
  }
  else
  {
   piszXY(1,1,15,"Zˆa litera.    ");
  }
  return koniec;
}

void zaloz_uzbrojenie_pom1(int a, lprzedmiot *uzbrojenie)
{
 Tprzedmiot pom;
 if (Hero.nasobie[a].typ==0)
                {
                  Hero.nasobie[a]=uzbrojenie->przedmiot;
                  usun_z_ekwipunku(uzbrojenie,&Hero);
                } else
                {
                  pom=uzbrojenie->przedmiot;
                  if ( pom.typ==6 &&
                       pom.numer==Hero.nasobie[a].numer )
                  {
                   Hero.nasobie[a].ilosc+=pom.ilosc;
                  }
                  else
                  {
                   dodaj_do_ekwipunku(Hero.nasobie[a],&Hero);
                   Hero.nasobie[a]=pom;
                  }
                  usun_z_ekwipunku(uzbrojenie,&Hero);
                }
}
bool zaloz_bron (lprzedmiot *bron)
{
 Tprzedmiot co_zakladam=bron->przedmiot;
 switch (co_zakladam.typ)
 {
  case 1 :  if (Hero.nasobie[2].typ>0) dodaj_do_ekwipunku(Hero.nasobie[2],&Hero);
            Hero.nasobie[2]=co_zakladam;
            break;
  case 2 :  if (Hero.nasobie[2].typ>0) dodaj_do_ekwipunku(Hero.nasobie[2],&Hero);
            if (Hero.nasobie[3].typ>0) dodaj_do_ekwipunku(Hero.nasobie[3],&Hero);
            Hero.nasobie[3]=Pnic;
            Hero.nasobie[2]=co_zakladam;
            break;
  case 5 :  if (Hero.nasobie[2].typ==2)
            {
              dodaj_do_ekwipunku(Hero.nasobie[2],&Hero);
              Hero.nasobie[2]=Pnic;
            }
            if (Hero.nasobie[3].typ==5) dodaj_do_ekwipunku(Hero.nasobie[3],&Hero);
            Hero.nasobie[3]=co_zakladam;
            break;
 }
 usun_z_ekwipunku(bron,&Hero);
}
bool zaloz_bizuterie(lprzedmiot *bizuteria)
{
 int nr=bizuteria->przedmiot.numer;
 int rodzaj=opis_przedmiotu[10][nr].p[1];
 if (rodzaj==4) zaloz_uzbrojenie_pom1(NS_SZYJA,bizuteria);
 else if (rodzaj==5)
 {
  if (Hero.nasobie[NS_P_DLON].typ==0)
     zaloz_uzbrojenie_pom1(NS_P_DLON,bizuteria);
     else
     if (Hero.nasobie[NS_L_DLON].typ==0)
        zaloz_uzbrojenie_pom1(NS_L_DLON,bizuteria);
        else
        {
         clrscr();
         piszXY(2,2,15,"Czy zdj¥† pierscieä lewy czy prawy ? (L/P)" );
         switch ( upcase(getch()) )
         {
          case 'P' : zaloz_uzbrojenie_pom1(NS_P_DLON,bizuteria); break;
          case 'L' : zaloz_uzbrojenie_pom1(NS_L_DLON,bizuteria); break;
          default  : czysc_linie(2);piszXY(2,2,15,"Zˆa litera.");break;
         }
        }
 }
}
bool Hero_zaloz_uzbrojenie()
{
 //ungetch(32); ////Aby pozbyc sie getcha

 pisz_ekwipunek();
 int i=zlicz_liste(Hero.przedmioty);
 int typ,numer;
 bool koniec=true;
 char odp;
 if (i>0)
 {
    odp=upcase(getch())-64;
    if (odp>0 && odp<=i)
    {
     //clrscr();
     lprzedmiot *obecny=Hero.przedmioty;
     int j=1;
     while(obecny)
     {
      if (j==odp) break;
      obecny=obecny->nast;
      j++;
     }
     typ=obecny->przedmiot.typ;
     numer=obecny->przedmiot.numer;
     switch (typ)
     {
       case 0 : break;
       case 1 : zaloz_bron(obecny);break;
       case 2 : zaloz_bron(obecny);break;
       case 3 : zaloz_uzbrojenie_pom1(NS_TULOW,obecny);break;
       case 4 : zaloz_uzbrojenie_pom1(NS_GLOWA,obecny);break;
       case 5 : zaloz_bron(obecny);break;
       case 6 : zaloz_uzbrojenie_pom1(NS_AMMO,obecny);break;
       case 7 :
       case 8 :
       case 9 : break;
       case 10: zaloz_bizuterie(obecny);break;
     }
     lczar *Cobecny=opis_przedmiotu[typ][numer].czary;
     while (Cobecny)
     {
      if (Cobecny->czar.tryb==1) dodaj_czar(lHero,Cobecny->czar,lHero->postac.sila);
       Cobecny=Cobecny->nast;
     }
    }
  }
 return koniec;
}

bool Hero_idz_schodami()
{
 int nr;
 lschody *obecny=pietro[np_gr].schody;
 while (obecny)
 {
  if ( obecny->schody.x==x_gr &&
       obecny->schody.y==y_gr ) break;
  obecny=obecny->nast;
 }
 if ( !obecny )
 {
  piszXY(1,1,15,"Ty nie ¾adnego przej˜cia.");
  return false;
 }
 if (!obecny->schody.aktywne)
 {
  piszXY(1,1,15,"Przej˜cie jest zablokowane.");
  return true;
 }
 lschody *s1=obecny;
 obecny=pietro[s1->schody.n_pietra].schody;
 while (obecny)
 {
  if ( obecny->schody.n_schodow==s1->schody.n_schodow ) break;
  obecny=obecny->nast;
 }
 lschody *s2=obecny;
 piszXY(1,1,15,"Przechodzisz przez przej˜cie.");
 znajdz_najblizsze_wolne_pole(nowe_x,nowe_y,s2->schody.x,s2->schody.y,s1->schody.n_pietra);
 nowe_np=s1->schody.n_pietra;
 return true;
}
bool Hero_atak()
{
 Topis_przedmiotu bron_a=opis(Hero.nasobie[NS_P_REKA]);
 int x=x_gr,
     y=y_gr;
 bool koniec=true;
 if (bron_a.p[1]==3)
 {
  if ( gdzie(x,y,Hero.wzrok,1) ) atak ( lHero, np_gr, x, y);
 }
 else  if ( Hero_pobierz_kierunek(x,y) ) atak ( lHero, np_gr, x, y);
 return koniec;
}
bool Hero_rzuc_czar(int czar)
{
 int x=x_gr,
     y=y_gr;
 bool ok=true;
 switch (czar)
 {
  case CZAR_USPIENIE :
  case CZAR_ZAUROCZENIE:
  case CZAR_MAGICZNY_POCISK :
  case CZAR_PORAZENIE :
  case CZAR_SLEPOTA :
  case CZAR_KWASOWA_STRZALA :
  case CZAR_PLOMIENIE :
  case CZAR_OGNISTA_STRZALA :
  case CZAR_UNIERUCHOMIENIE:
  case CZAR_POCISKI_MOCY:
  case CZAR_OSLABIENIE :
  case CZAR_SPOWOLNIENIE:
  case CZAR_PALEC_SMIERCI :
  case CZAR_CALUN_PLOMIENI :
  case CZAR_SM_OGLUSZENIE :
       if ( ok=gdzie(x,y,Hero.wzrok,1) )
          ok=rzuc_czar( lHero, x, y, czar );
       break;
  case CZAR_TARCZA :
  case CZAR_BLOGOSLAWIENSTWO :
  case CZAR_SILA_HEROSA:
  case CZAR_LEKKIE_LECZENIE :
  case CZAR_SREDNIE_LECZENIE :
  case CZAR_ZAAWANSOWANE_LECZENIE :
  case CZAR_UZDROWIENIE :
  case CZAR_ANTIDOTUM :
  case CZAR_PSALM:
  case CZAR_PRZELAMANIE_STRACHU:
  case CZAR_NIEWIDZIALNOSC :
  case CZAR_WYKRYCIE_NIEWIDZIALNYCH :
  case CZAR_OCHRONA_PRZED_OGNIEM :
  case CZAR_KAMIENNA_SKORA :
  case CZAR_PRZYSPIESZENIE :
  case CZAR_OCHRONA_ZYWIOLY :
  case CZAR_OKO_MAGA :
  case CZAR_OCHRONA_PRZED_MAGIA :
  case CZAR_OCHRONA_SMIERC:
  case CZAR_REGENERACJA:
  case CZAR_NIEWRAZLIWOSC :
  case CZAR_SKOWYT_BANSHEE:
       ok=rzuc_czar( lHero, x, y, czar );
       break;
  case CZAR_OPLATANIE :
  case CZAR_LODOWA_PETLA:
  case CZAR_PAJECZYNA:
  case CZAR_KULA_OGNISTA:
  case CZAR_BLYSKAWICA:
  case CZAR_PRZYW_SZKIELETU :
  case CZAR_SCIANA_OGNIA :
  case CZAR_POZOGA :
  case CZAR_POLE_STATYCZNE:
  case CZAR_WEZWANIE_ZWIERZAT:
  case CZAR_TELEPORT :
  case CZAR_PRZYW_ZYWIOLAKA :
  case CZAR_STREFA_MROZU :
  case CZAR_PLUGAWY_UWIAD :
  case CZAR_BURZA:
  case CZAR_TRZESIENIE_ZIEMI:
  case CZAR_BRAMA_PIEKIEL:
//   case : CASE_
       if ( ok=gdzie(x,y,Hero.wzrok,0) )
          ok=rzuc_czar( lHero, x, y, czar );
       break;
  case CZAR_PLONACE_DLONIE:
  case CZAR_STOZEK_ZIMNA :
       if ( ok=Hero_pobierz_kierunek(x,y) )
          ok=rzuc_czar( lHero, x, y, czar );
       break;
 }
 return ok;
}
void odswierz_menu_Hero_wybor_czaru(int poziom, int tabela[])
{
 clrscr();
 for(int i=1;i<10;i++)
 {
  int kolor=8;
  if (tabela[i]>0) kolor=15;
  piszXY(2,1+i,kolor,"%i) Czary %i Poziomu.",i,i);
 }
 int j=0;
 for (int i=0;i<Lczar;i++)
 {
  if (ksiega_czarow[i]>0)
   if ( poziom==opis_czaru[i].poziom )
   {
    int kolor=15;
    if (Hero.mana<opis_czaru[i].mana) kolor=8;
    piszXY(30,2+j,kolor,"%c) %s ( %i PM ) ",j+65, opis_czaru[i].nazwa,
                                                  opis_czaru[i].mana);
//                                                    umiejetnosci[10+opis_czaru[i].lista]);
    j++;
   }
 }
}
bool Hero_wybierz_czar()
{
 int tabela_ilosc[10]={0};
 clrscr();
 for (int i=0;i<Lczar;i++)
 {
 // printf("kc[%i]=%i oc[%i].p=%i ",i,ksiega_czarow[i],i,opis_czaru[i].poziom);
  if (ksiega_czarow[i]>0)
    tabela_ilosc[opis_czaru[i].poziom]++;
 }
 //getch();
 bool koniec=false,
      ok=true,
      start=true;
 static int poziom=1;
 int nr=0,ile_na_poziomie=0;
 int numer_czaru[30]={0};
 while (!koniec)
 {
  odswierz_menu_Hero_wybor_czaru(poziom,tabela_ilosc);
  char odp;
  if (start)
  {
   odp=poziom+'0';
   start=false;
  } else odp=upcase(getch());
  if ( nalezy('1','9',odp) )
  {
   poziom=odp-48;
   ile_na_poziomie=0;
   for (int i=0;i<Lczar;i++)
   {
     if (ksiega_czarow[i]>0)
      if ( poziom==opis_czaru[i].poziom )
      {
       ile_na_poziomie++;
       numer_czaru[ile_na_poziomie]=i;
      }
   }
  }
  else
  {
   if (ile_na_poziomie>0 && nalezy('A','A'+ile_na_poziomie-1,odp) )
   {
    Topis_czaru czar=opis_czaru[numer_czaru[odp-64]];
    if (Hero.mana<czar.mana) ok = false;
    else if ( koniec=Hero_rzuc_czar(numer_czaru[odp-64]) )
            Hero.mana-=czar.mana;
    if (!ok)
    {
     piszXY(1,1,15,"Za maˆo punkt¢w many. ");
     getch();
    }
    return koniec;
   }
   else
   {
    piszXY(2,1,15,"Zˆa litera. ");
    getch();
    return false;
   }
  }
 }
 return koniec;
}

bool Hero_uzyj_przedmiotu()
{
 int i=zlicz_liste(Hero.przedmioty);
 if (i==0)
 {
  piszXY(1,1,15,"Nie masz ¾adnych przedmiot¢w.");
  getch();
  return false;
 }
 lprzedmiot **przed=new lprzedmiot*[i];
 lprzedmiot *obecny=Hero.przedmioty;
 int j=0;
 while (obecny)
 {
  if (obecny->przedmiot.typ==7)
  {
   przed[j]=obecny;
   j++;
  }
  obecny=obecny->nast;
 }
 i=j;
 bool koniec=false;
 if (j==0)
 {
  piszXY(1,1,15,"Nie masz przy sobie ¾adnych magicznych przedmiot¢w.");
 }
 else
 {
  clrscr();
  textcolor(15);
  Topis_przedmiotu opisP;
  Tprzedmiot P;
  for(j=0;j<i;j++)
  {
   piszXY(1,j+2,15,"%c)",j+65);
   P=przed[j]->przedmiot;
   opisP=opis(P);
   if (opisP.tworzy_stos && P.ilosc>1) cprintf(" %i x", P.ilosc);
   cprintf(" %s", opisP.nazwa);
   if (opisP.ma_ladunki) cprintf(" ( %i ˆadunk¢w )",P.ilosc);
  }
  char odp=upcase(getch());
  if (odp>64 && odp<(65+i))
   {
    P=przed[odp-65]->przedmiot;
    opisP=opis(P);
    if (koniec=Hero_rzuc_czar( opisP.p[2] ))
    {
      koniec=true;
      if (!opisP.tworzy_stos && !opisP.ma_ladunki) usun_z_ekwipunku(przed[odp-65],&Hero);
      else
      {
       P.ilosc--;
       przed[odp-65]->przedmiot.ilosc--;
       if (P.ilosc<1) usun_z_ekwipunku(przed[odp-65],&Hero);
      }
    }
   }
  else
   {
    gotoxy(1,1);cprintf("Zˆa litera.    ");
   }
 }
 delete [] przed;
 return koniec;
}
bool Hero_otworz_drzwi()
{
 int ile_drzwi = okolica(Hero.x,Hero.y,DRZWI_Z);
 if (ile_drzwi==0)
 {
  info("Nie ma tu ¾adnych zamkni©tych drzwi.");
  return false;
 }
 ldrzwi *drzwi;
 if (ile_drzwi==1) drzwi=znajdz_najblizsze_drzwi(Hero.x,Hero.y,2);
 else
 {
  int x=Hero.x,
      y=Hero.y;
  if (!Hero_pobierz_kierunek(x,y)) return false;
  drzwi=znajdz_drzwi(x,y,2);
  if (!drzwi)
  {
   info("Nie ma tam ¾adnych zamkni©tych drzwi.");
   return false;
  }
 }
 if (drzwi->drzwi.zablokowane)
 {
  info("Drzwi s¥ zablokowane.");
  return false;
 }
 if ( (los(1,20)+premia(Hero.umiejetnosc[UM_ZAMKI])) > drzwi->drzwi.trudnosc )
 {
  otworz_drzwi(drzwi);
  drzwi->drzwi.zablokowane=true;
  info("Otwierasz drzwi.");
 }
 else info("Nie udaje ci si© otworzyc zamka.");

// otworz_drzwi(drzwi);
// info("Otwierasz drzwi.");
 return true;
}
bool Hero_zamknij_drzwi()
{

 int ile_drzwi = okolica(Hero.x,Hero.y,DRZWI_O);
 if (ile_drzwi==0)
 {
  info("Nie ma tu ¾adnych otwartych drzwi.");
  return false;
 }
 ldrzwi *drzwi;
 if (ile_drzwi==1) drzwi=znajdz_najblizsze_drzwi(Hero.x,Hero.y,1);
 else
 {
  int x=Hero.x,
      y=Hero.y;
  if (!Hero_pobierz_kierunek(x,y)) return false;
  drzwi=znajdz_drzwi(x,y,1);
  if (!drzwi)
  {
   info("Nie ma tam ¾adnych otwartych drzwi.");
   return false;
  }
 }
// printf("(%i, %i )",drzwi->drzwi.x,drzwi->drzwi.y);getch();
 if (drzwi->drzwi.zablokowane)
 {
  info("Drzwi nie daj¥ si© zamkn¥†.");
  return false;
 }
 /*
 if (!drzwi->drzwi.otwarte)
 {
  piszXY(1,1,15,"Drzwi s¥ ju¾ zamkni©te.");
  return false;
 }*/
 if (jest_Hero (np_gr,drzwi->drzwi.x,drzwi->drzwi.y) )
 {
  info("Nie mo¾esz zamkn¥† drzwi, w kt¢rych stoisz.");
  return false;
 }
 zamknij_drzwi(drzwi);
 info("Zamykasz drzwi.");
 return true;
}
bool Hero_wywaz_drzwi()
{
 int ile_drzwi = okolica(Hero.x,Hero.y,DRZWI_Z);
 if (ile_drzwi==0)
 {
  info("Nie ma tu ¾adnych zamkni©tych drzwi.");
  return false;
 }
 ldrzwi *drzwi;
 if (ile_drzwi==1) drzwi=znajdz_najblizsze_drzwi(Hero.x,Hero.y,2);
 else
 {
  int x=Hero.x,
      y=Hero.y;
  if (!Hero_pobierz_kierunek(x,y)) return false;
  drzwi=znajdz_drzwi(x,y,2);
  if (!drzwi)
  {
   info("Nie ma tam ¾adnych zamkni©tych drzwi.");
   return false;
  }
 }
 if ( (los(1,20)+premia(Hero.stat[0])) > drzwi->drzwi.wyt )
 {
  otworz_drzwi(drzwi);
  drzwi->drzwi.zablokowane=true;
  info("Udaje ci si© wywazyc drzwi.");
 }
 else info("Drzwi pozostaj¥ na swoim miejscu.");
 return true;
}
bool Hero_rozbroj_pulapke()
{
 lpulapka* obecny=pietro[np_gr].pulapki;
 int ile_pulapek=0;
 while (obecny)
 {
  if (obecny->pulapka.znana)
   if ( odleglosc_ruch(Hero.x,Hero.y,obecny->pulapka.x,obecny->pulapka.y) < 2 )
     ile_pulapek++;
  obecny=obecny->nast;
 }
 if (ile_pulapek==0)
 {
  info("Nie widzisz wok¢ˆ siebie ¾adnych puˆapek.");
  return false;
 }
 lpulapka *pulapka;
 if (ile_pulapek==1) pulapka=znajdz_najblizsza_pulapke(Hero.x,Hero.y);
 else
 {
  int x=Hero.x,
      y=Hero.y;
  if (!Hero_pobierz_kierunek(x,y)) return false;
  pulapka=znajdz_pulapke(x,y); ///znane,nieznane itp
  if (!pulapka)
  {
   info("Nie ma tam ¾adnej puˆapki.");
   return false;
  }
 }
 if ( (los(1,20)+Hero.umiejetnosc[UM_PULAPKI] ) > (10+pulapka->pulapka.trudnosc) )
 {
  info("Udaj© ci si© rozbroi† puˆapk©.");
  dodaj_doswiadczenie(lHero,pulapka->pulapka.rodzaj*10);
 }
 else
 {
  info("Ups! Niechc¥cy uaktywniasz puˆapk©.");
  efekt_pulapka(pulapka,lHero);
 }
 usun_z_pulapek(pulapka);
 return true;
}
int Hero_wykrywaj_pulapki()
{
 int x,y;
 lpulapka *obecny=pietro[np_gr].pulapki;
 while (obecny)
 {
  if (!obecny->pulapka.znana)
  {
   x=obecny->pulapka.x;
   y=obecny->pulapka.y;
   if (pietro[np_gr].mgla[x][y]==WIDOCZNE)
   {
    int d=int( odleglosc(Hero.x,Hero.y,x,y) );
    if (d<Hero.wzrok)
     {
      if ( (los(1,20)+ Hero.umiejetnosc[UM_POSZUKIWANIE])>(10+d) )
      {
       obecny->pulapka.znana=true;
       generuj_bufor();
       rysuj_bufor();
       info("Wykryto puˆapk©.");
      }
     }
   }
  }
  obecny=obecny->nast;
 }
}
int Hero_wykrywaj_ukryte_drzwi()
{
 int x,y;
 ldrzwi *obecny=pietro[np_gr].drzwi;
 while (obecny)
 {
  if (obecny->drzwi.ukryte)
  {
   x=obecny->drzwi.x;
   y=obecny->drzwi.y;
   if (pietro[np_gr].mgla[x][y]==WIDOCZNE)
   {
    int d=int( odleglosc(Hero.x,Hero.y,x,y) );
    if (d<=Hero.wzrok)
     {
      if ( ( los(1,20) + Hero.umiejetnosc[UM_POSZUKIWANIE] ) > ( 15 + d ) )
      {
       obecny->drzwi.ukryte=false;
       int znak=DRZWI_Z;
       if (obecny->drzwi.otwarte) znak=DRZWI_O;
       pietro[np_gr].mapa[x][y]=znak;
       generuj_bufor();
       rysuj_bufor();
       info("Zauwa¾asz sekretne drzwi.");
      }
     }
   }
  }
  obecny=obecny->nast;
 }
}
int Hero_wykrywaj_niewidzialnych()
{
 int x,y;
 lpostac *obecny=pietro[np_gr].npc;
 while (obecny)
 {
  if ( obecny->postac.niewidoczny )
  if ( !obecny->postac.stan[STAN_NIEWIDZIALNOSC] )
  {
   x=obecny->postac.x;
   y=obecny->postac.y;
   if (pietro[np_gr].mgla[x][y]==WIDOCZNE)
   {
    int d=int( odleglosc(Hero.x,Hero.y,x,y) );
    if (d<Hero.wzrok)
     {
      if ( (los(1,20) + Hero.umiejetnosc[UM_POSZUKIWANIE] -
           obecny->postac.umiejetnosc[UM_SKRADANIE] )
           > (15+d) )
      {
       obecny->postac.niewidoczny=false;
       generuj_bufor();
       rysuj_bufor();
       info("Dostrzegasz skradaj¥cego si© wroga.");
      }
     }
   }
  }
  obecny=obecny->nast;
 }
}

int Hero_test_percepcji()
{
 Hero_wykrywaj_pulapki();
 Hero_wykrywaj_ukryte_drzwi();
 Hero_wykrywaj_niewidzialnych();
}

bool Hero_ukryj_sie()
{
 if ( ukryj_sie_w_cieniu(lHero) ) info("Skrywasz sie w cieniu.");
    else info("Niepowodzenie.");
}
long int ile_dosw( int poziom, Tpostac *postac)
{
 int p2=postac->poziom;
 long int dosw=int(pow(2,p2-poziom)*rasa[postac->rasa].bazowe_dosw);
 if ( (p2-poziom)>2 ) dosw=120+(p2-poziom)*rasa[postac->rasa].bazowe_dosw;
 dosw=int(float(dosw)*Hero.mnoznik_dosw);
 if (dosw>1000) dosw=1000;
 return dosw;
}
bool Hero_odpoczywaj()
{ //Hero_czekaj();
  odpoczywaj(lHero);
  return true;
}
bool Hero_kradziez()
{
 lpostac* obecny=pietro[np_gr].npc;
 lpostac *cel;
 int ile_npc=0;
 while (obecny)
 {
  if (!obecny->postac.martwy)
   if (!obecny->postac.niewidoczny)
    if ( odleglosc_ruch(Hero.x,Hero.y,obecny->postac.x,obecny->postac.y) < 2 )
    {
     ile_npc++;
     cel=obecny;
    }
  obecny=obecny->nast;
 }
 if (ile_npc==0)
 {
  info("Nie widzisz nikogo wok¢ˆ siebie.");
  return false;
 }
 if (ile_npc==1) cel=znajdz_najblizsza_postac(lHero);
 else
 {
  int x=Hero.x,
      y=Hero.y;
  if (!Hero_pobierz_kierunek(x,y)) return false;
  cel=znajdz_postac(x,y,np_gr); ///znane,nieznane itp
  if (!cel)
  {
   info("Nie ma tam nikogo.");
   return false;
  }
 }
 int i=pisz_ekwipunek_postaci(cel);
 if (i==0)
 {
  return false;
 }
 lprzedmiot **przed=new lprzedmiot*[i];
 lprzedmiot *Pobecny=cel->postac.przedmioty;
 int j=0;
 while (Pobecny)
 {
  przed[j]=Pobecny;
  j++;
  Pobecny=Pobecny->nast;
 }
 bool koniec=false;
 char odp=upcase(getch());
 if (odp>64 && odp<(65+i))
 {
  Tprzedmiot P=przed[odp-65]->przedmiot;
  Topis_przedmiotu opisP=opis(P);
  int bonus=  premia_u(Hero.umiejetnosc[UM_ZLODZIEJSTWO])
              - cel->postac.umiejetnosc[UM_POSZUKIWANIE];
  if (Hero.niewidoczny) bonus+=5;
  if (cel->postac.niewidoczny) bonus-=10;
  if ( ( los(1,20) + bonus ) > 15 )
  {
   dodaj_do_ekwipunku(przed[odp-65]->przedmiot,&Hero);
   usun_z_ekwipunku(przed[odp-65],&cel->postac);
   info("Ukradˆe˜ %s.",opisP.nazwa);
  } else info("Nie udaˆo ci si© nic ukra˜†!");
  bonus=  premia_u( Hero.umiejetnosc[UM_ZLODZIEJSTWO] )
          + premia( Hero.stat[ST_CH] )
          - cel->postac.umiejetnosc[UM_POSZUKIWANIE];
  if (Hero.niewidoczny) bonus+=5;
  if (Hero.sila!=cel->postac.sila)
   if ( ( los(1,20) + bonus ) < 10 )
   {
    info("Zostaˆe˜ zauwa¾ony.",opisP.nazwa);
    zmien_ai_status(Hero.sila,cel->postac.sila,-5);
    Hero.niewidoczny=false;
   }
  koniec=true;
 }
 else
 {
   gotoxy(1,1);cprintf("Zˆa litera.    ");
   koniec=false;
 }
 delete [] przed;
 return koniec;
}
bool Hero_handluj()
{
 lpostac* obecny=pietro[np_gr].npc;
 lpostac *cel;
 int ile_npc=0;
 while (obecny)
 {
  if (!obecny->postac.martwy)
   if (!obecny->postac.niewidoczny)
    if ( odleglosc_ruch(Hero.x,Hero.y,obecny->postac.x,obecny->postac.y) < 2 )
    {
     ile_npc++;
     cel=obecny;
    }
  obecny=obecny->nast;
 }
 if (ile_npc==0)
 {
  info("Nie widzisz nikogo wok¢ˆ siebie.");
  return false;
 }
 if (ile_npc==1) cel=znajdz_najblizsza_postac(lHero);
 else
 {
  int x=Hero.x,
      y=Hero.y;
  if (!Hero_pobierz_kierunek(x,y)) return false;
  cel=znajdz_postac(x,y,np_gr); ///znane,nieznane itp
  if (!cel)
  {
   info("Nie ma tam nikogo.");
   return false;
  }
 }
 if ( pobierz_ai_status(Hero.sila,cel->postac.sila) < 0 )
 {
  info("%s nie chce z tab¥ handlowa†.",cel->postac.imie);
  return false;
 }
 int i=pisz_ekwipunek_postaci(cel);
 if (i==0)
 {
  return false;
 }
 lprzedmiot **przed=new lprzedmiot*[i];
 lprzedmiot *Pobecny=cel->postac.przedmioty;
 int j=0;
 while (Pobecny)
 {
  przed[j]=Pobecny;
  j++;
  Pobecny=Pobecny->nast;
 }
 bool koniec=false;
 char odp=upcase(getch());
 if (odp>64 && odp<(65+i))
 {
  Tprzedmiot P=przed[odp-65]->przedmiot;
  Topis_przedmiotu opisP=opis(P);
  int bonus=  premia_u(Hero.umiejetnosc[UM_ZLODZIEJSTWO])
              - cel->postac.umiejetnosc[UM_POSZUKIWANIE];
  if (Hero.niewidoczny) bonus+=5;
  if (cel->postac.niewidoczny) bonus-=10;
  if ( ( los(1,20) + bonus ) > 15 )
  {
   dodaj_do_ekwipunku(przed[odp-65]->przedmiot,&Hero);
   usun_z_ekwipunku(przed[odp-65],&cel->postac);
   info("Kupujesz : %s.",opisP.nazwa);
  } else info("Nie udaˆo ci si© nic kupi†!");
  koniec=true;
 }
 else
 {
   gotoxy(1,1);cprintf("Zˆa litera.    ");
   koniec=false;
 }
 delete [] przed;
 return koniec;
}
bool Hero_rozmawiaj()
{
 lpostac* obecny=pietro[np_gr].npc;
 lpostac *cel;
 int ile_npc=0;
 while (obecny)
 {
  if (!obecny->postac.martwy)
   if (!obecny->postac.niewidoczny)
    if ( odleglosc_ruch(Hero.x,Hero.y,obecny->postac.x,obecny->postac.y) < 2 )
    {
     ile_npc++;
     cel=obecny;
    }
  obecny=obecny->nast;
 }
 if (ile_npc==0)
 {
  info("Nie widzisz nikogo wok¢ˆ siebie.");
  return false;
 }
 if (ile_npc==1) cel=znajdz_najblizsza_postac(lHero);
 else
 {
  int x=Hero.x,
      y=Hero.y;
  if (!Hero_pobierz_kierunek(x,y)) return false;
  cel=znajdz_postac(x,y,np_gr); ///znane,nieznane itp
  if (!cel)
  {
   info("Nie ma tam nikogo.");
   return false;
  }
 }
 if ( pobierz_ai_status(Hero.sila,cel->postac.sila) < -10 )
 {
  info("%s nie chce z tab¥ rozmawia†.",cel->postac.imie);
  return false;
 }
 info("%s nie ma nic ciekawego do powiedzenia.",cel->postac.imie);
 return true;
}

