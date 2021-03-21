#include "glowny.h"

void niszcz_liste(lprzedmiot *lista)
{
 lprzedmiot *obecny, *do_kasacji;
 obecny=lista;
 while(obecny)
 {
  do_kasacji=obecny;
  obecny=obecny->nast;
  delete do_kasacji;
 }
 lista=NULL;
}
void niszcz_liste(lczar *lista)
{
 lczar *obecny, *do_kasacji;
 obecny=lista;
 while(obecny)
 {
  do_kasacji=obecny;
  obecny=obecny->nast;
  delete do_kasacji;
 }
 lista=NULL;
}
void niszcz_liste(lpostac *lista)
{
 lpostac *obecny, *do_kasacji;
 obecny=lista;
 while(obecny)
 {
  do_kasacji=obecny;
  niszcz_liste(obecny->postac.czary);
  niszcz_liste(obecny->postac.przedmioty);
  printf("%s  kasuje=%i ",obecny->postac.imie,do_kasacji);
  obecny=obecny->nast;
  delete do_kasacji;
 }
 lista=NULL;
}
void niszcz_liste(ldrzwi *lista)
{
 ldrzwi *obecny, *do_kasacji;
 obecny=lista;
 while(obecny)
 {
  do_kasacji=obecny;
  obecny=obecny->nast;
  delete do_kasacji;
 }
 lista=NULL;
}
void niszcz_liste(lschody *lista)
{
 lschody *obecny, *do_kasacji;
 obecny=lista;
 while(obecny)
 {
  do_kasacji=obecny;
  obecny=obecny->nast;
  delete do_kasacji;
 }
 lista=NULL;
}
void niszcz_liste(lpulapka *lista)
{
 lpulapka *obecny, *do_kasacji;
 obecny=lista;
 while(obecny)
 {
  do_kasacji=obecny;
  obecny=obecny->nast;
  delete do_kasacji;
 }
 lista=NULL;
}
int zlicz_liste(lprzedmiot *lista)
{
 lprzedmiot *obecny=lista;
 int ile=0;
 while (obecny)
 {
  obecny=obecny->nast;
  ile++;
 }
 return ile;
}
int zlicz_liste(lpostac *lista)
{
 lpostac *obecny=lista;
 int ile=0;
 while (obecny)
 {
  obecny=obecny->nast;
  ile++;
 }
 return ile;
}

void dodaj_przedmiot ( Tprzedmiot przedmiot, lprzedmiot *lista )
{
 if (przedmiot.typ==0) return;
 lprzedmiot *stary=lista;
 lista=new lprzedmiot;
 lista->przedmiot=przedmiot;
 lista->nast=stary;
}
void wczytaj_opis_przedmiotow(char* sciezka)
{
 ifstream in(sciezka);
 //getch();
 //printf("plik otwarty\n");
 //getch();
 int l_typow,l_rodzajow;
 int p1,p2,p3,wyt;
 float masa,cena;
 int kolor;
// fscanf(in,"%i",&l_typow);
 char znak;
 in >> l_typow;// >> znak;
 printf("czytam lt %i\n",l_typow);
 //getch();
 opis_przedmiotu=new Topis_przedmiotu*[l_typow];
 int i,j;
 for(i=0;i<l_typow;i++)
 {
  //fscanf(in,"%i",&l_rodzajow);
  in >> l_rodzajow;
  opis_przedmiotu[i]=new Topis_przedmiotu[l_rodzajow];
  printf("l_rodz %i\n",l_rodzajow);
  //getch();
  for(j=0;j<l_rodzajow;j++)
  {
   opis_przedmiotu[i][j].ma_ladunki=false;
   opis_przedmiotu[i][j].tworzy_stos=false;
   printf("\n t=%i n=%i",i,j);
//   fscanf(in,"\n%[^\n]",opis_przedmiotu[i][j].nazwa);
//   in.getline(opis_przedmiotu[i][j].nazwa,40);
   in.get();
   in.getline(opis_przedmiotu[i][j].nazwa,40);
   printf("\n Nazwa: %s\n",opis_przedmiotu[i][j].nazwa);
   int ile=3;
   if (i<3) ile=6;
   opis_przedmiotu[i][j].p[0]=i;
//   fscanf(in,"\n");
   for (int k=1;k<=ile;k++)
   {
    //fscanf(in,"%i",&p1);
    in >> p1;
    opis_przedmiotu[i][j].p[k]=p1;
    printf(" p[%i]=%i ",k,p1);
   }
   if (i>0)
   {
      //fscanf(in,"%f %i %f %i",&masa,&wyt,&cena,&kolor);
      in >> masa >> wyt >> cena >> kolor;
      //fscanf(in,"%f %i",&cena,&kolor);
   }
   else
   {
    //fscanf(in,"%f %i",&masa,&wyt);
    in >> masa >> wyt;
    cena=0;
    kolor=15;
   }
   int C=int(cena);
   printf("m=%f w=%i ",masa,wyt);
   printf("c=%i k=%i ",C,kolor);
   opis_przedmiotu[i][j].masa=masa;
   opis_przedmiotu[i][j].wyt=wyt;
   opis_przedmiotu[i][j].cena=cena;
   opis_przedmiotu[i][j].kolor=kolor;
   if (i==6 || i==7 || i==9) opis_przedmiotu[i][j].tworzy_stos=true;
   if (i==7 && opis_przedmiotu[i][j].p[1]==5)
   {
    opis_przedmiotu[i][j].tworzy_stos=false;
    opis_przedmiotu[i][j].ma_ladunki=true;
   }
   //getch();
   opis_przedmiotu[i][j].czary=NULL;
   int l_czarow;
//   fscanf(in," %i",&l_czarow);
   in >> l_czarow;
   printf("l_czarow=%i ",l_czarow);
   //getch();
   if (l_czarow>0)
   {
    int tryb,rodzaj,czas,l_p;
    for (int k=0;k<l_czarow;k++)
    {
     Tczar czar;
//     fscanf(in," %i %i %i %i",&tryb,&rodzaj,&czas,&l_p);
     in >> tryb >> rodzaj >> czas >> l_p;
     printf ("tryb=%i rodzaj=%i czas=%i lp=%i ",tryb,rodzaj,czas,l_p);
     //getch();
     czar.tryb=tryb;
     czar.rodzaj=rodzaj;
     czar.ile_do_konca=czas;
     czar.aktywny=true;
     czar.staly=false;
     if (l_p>0)
      for (int l=0;l<l_p;l++)
      {
       int p;
       //fscanf(in,"%i ",&p);
       in >> p;
       printf("p[%i]=%i ",l,p);
       czar.p[l]=p;
      }
     dodaj_czar(opis_przedmiotu[i][j],czar);
    }
   }
//   if (i>7) getch();
  }
  //in.get();
 }
 //fclose(in);
 //getch();
}

int kolor_przedmiotu(Tprzedmiot &p)
{
 int t=p.typ,
     n=p.numer;
 return opis_przedmiotu[t][n].kolor;
}
char kod_przedmiotu(int typ)
{
 int kod;
 switch (typ) {
  case 1:kod=27;break;
  case 2:kod=24;break;
  case 3:kod=5;break;
  case 4:kod=127;break;
  case 5:kod=31;break;
  case 6:kod=42;break;
  case 7:kod=14;break;
  case 8:kod=33;break;
  case 9:kod=4;break;
  case 10:kod=9;break;
  default:kod=17;
 }
 return kod;
}
Topis_przedmiotu opis(Tprzedmiot przedmiot)
{
 try {
 return opis_przedmiotu[przedmiot.typ]
                       [przedmiot.numer];
 } catch(...) { printf("Przedmniot r=%i n=%i\n",przedmiot.typ,przedmiot.numer);
                getch();
              }

}
int zlicz_przedmioty(int x,int y,int np)
{
 if (np<0) np=np_gr;
 int i=0;
 lprzedmiot *obecny=pietro[np].przedmioty;
 while (obecny)
 {
  if (obecny->przedmiot.x==x && obecny->przedmiot.y==y) i++;
  obecny=obecny->nast;
 }
 return i;
}
lprzedmiot* szukaj_w_ekwipunku(Tpostac* postac,int typ,int nr, int p1)
{
 int l=0;
 Topis_przedmiotu opisP;
 Tprzedmiot P;
 lprzedmiot *obecny=postac->przedmioty;
 while (obecny)
 {
   P=obecny->przedmiot;
   opisP=opis(P);
   if ( P.typ==typ && P.numer==nr && opisP.p[1]==p1) return obecny;
   l++;
   obecny=obecny->nast;
 }
 return NULL;
}
void dodaj_pulapke( int np, Tpulapka pulapka)
{
 lpulapka *lista=pietro[np].pulapki;
 lpulapka *stary=lista;
 lista=new lpulapka;
 lista->pulapka=pulapka;
 lista->nast=stary;
 pietro[np].pulapki=lista;
}
void usun_z_pulapek( lpulapka *pulapka, int np )
{
 if (np<0) np=np_gr;
 lpulapka *obecny=pietro[np].pulapki, *pop=obecny;
 if (obecny==pulapka) pietro[np].pulapki=pietro[np].pulapki->nast;
 {
  while (obecny)
  {
   if (obecny==pulapka) break;
   pop=obecny;
   obecny=obecny->nast;
  }
  pop->nast=obecny->nast;
 }
 delete obecny;
}
lpulapka* znajdz_pulapke(int x,int y,int np)
{
 if (np<0) np=np_gr;
 lpulapka *obecny=pietro[np].pulapki;
 while (obecny)
 {
  if (obecny->pulapka.x==x && obecny->pulapka.y==y)
     return obecny;
  obecny=obecny->nast;
 }
 return NULL;
}
lpulapka *znajdz_najblizsza_pulapke(int x, int y, int tryb, int np)
{
 if (np<0) np=np_gr;
 lpulapka *obecny=pietro[np].pulapki,
          *odp=NULL;
 int d=32000,dn;
 while (obecny)
 {
   dn=odleglosc_ruch(x,y,obecny->pulapka.x,obecny->pulapka.y);
   if (obecny==pietro[np].pulapki) {d=dn;odp=obecny;}
   if ( dn<d )
   {
     odp=obecny;
     d=dn;
   }
  obecny=obecny->nast;
 }
 return odp;
}

int efekt_pulapka(lpulapka *lpulapka,lpostac *cel)
{
 Tpulapka pulapka=lpulapka->pulapka;
 if (!cel) cel=znajdz_postac(pulapka.x,pulapka.y);
 if (!pulapka.aktywna) return 1;
 switch (pulapka.rodzaj)
 {
  case 1: if (cel) obrazenia(NULL,cel,OBR_NORMAL,los(2,1,6) );///BOOOM
        break;
  case 2:
        {
         Tczar efekt={ EFEKT_ZATRUCIE, 4,false,1,6,0 };
         dodaj_czar(cel , efekt ,NULL);
        }
        break;
  case 3: if (cel) obrazenia(NULL,cel,OBR_OGIEN,los(6,1,6) );///BOOOM
        break;
  case 4: if (cel) obrazenia(NULL,cel,OBR_MAGIA,los(8,1,10) );///BOOOM
        break;
  case 5:
        {
         Tczar efekt={ EFEKT_ZATRUCIE, 10,false,1,10,0 };
         dodaj_czar(cel , efekt ,NULL);
        }
        break;
 }
}
void tworz_pulapke(int x, int y, Tpulapka pulapka,int np)
{
 if (np<0) np=np_gr;
// Tdrzwi drzwi={true,false,x,y,trudnosc,10};
 pulapka.x=x;
 pulapka.y=y;
 dodaj_pulapke(np,pulapka);
}
int otworz_drzwi(ldrzwi *drzwi, int np)
{
 if (np<0) np=np_gr;
 drzwi->drzwi.otwarte=true;
 pietro[np].mapa[drzwi->drzwi.x][drzwi->drzwi.y]=DRZWI_O;
}
int zamknij_drzwi(ldrzwi *drzwi, int np)
{
 if (np<0) np=np_gr;
 drzwi->drzwi.otwarte=false;
 pietro[np].mapa[drzwi->drzwi.x][drzwi->drzwi.y]=DRZWI_Z;
}
void dodaj_drzwi( int np, Tdrzwi drzwi)
{
 ldrzwi *lista=pietro[np].drzwi;
 ldrzwi *stary=lista;
 lista=new ldrzwi;
 lista->drzwi=drzwi;
 lista->nast=stary;
 pietro[np].drzwi=lista;
}
void tworz_drzwi(int x, int y, Tdrzwi drzwi,int np)
{
 if (np<0) np=np_gr;
// Tdrzwi drzwi={true,false,x,y,trudnosc,10};
 drzwi.x=x;
 drzwi.y=y;
 dodaj_drzwi(np,drzwi);
 if (drzwi.ukryte) return;
 if (drzwi.otwarte) pietro[np].mapa[x][y]=DRZWI_O;
               else pietro[np].mapa[x][y]=DRZWI_Z;

}
ldrzwi *znajdz_najblizsze_drzwi(int x, int y, int tryb, int np)
{
 if (np<0) np=np_gr;
 ldrzwi *obecny=pietro[np].drzwi,
        *odp=NULL;
 int d=32000,dn;
/* if (obecny)
 {
  //printf("d=(%i, %i) ",obecny->drzwi.x,obecny->drzwi.y);getch();
  d=odleglosc_ruch(x,y,obecny->drzwi.x,obecny->drzwi.y);
 }*/
 while (obecny)
 {
   bool ok=false;
   switch (tryb)
   {
    case 0 : ok=true;
    case 1 : if (obecny->drzwi.otwarte) ok=true;break;
    case 2 : if (!obecny->drzwi.otwarte) ok=true;break;
   }
   if (ok)
   {
  //printf("Sa Drzwi.");getch();
    dn=odleglosc_ruch(x,y,obecny->drzwi.x,obecny->drzwi.y);
    if (obecny==pietro[np].drzwi) {d=dn;odp=obecny;}
    if ( dn<d )
    {
     odp=obecny;
     d=dn;
    }
   }
  obecny=obecny->nast;
 }
 return odp;
}
ldrzwi *znajdz_drzwi(int x, int y, int tryb, int np)//0-oba 1-otw 2-zam
{
 if (np<0) np=np_gr;
 ldrzwi *obecny=pietro[np].drzwi,
        *odp=obecny;
 while (obecny)
 {
  //printf("Sa Drzwi.");getch();
  if (!obecny->drzwi.ukryte)
  if ( x==obecny->drzwi.x )
   if ( y==obecny->drzwi.y )
    switch (tryb)
    {
     case 0 : return obecny;
     case 1 : if (obecny->drzwi.otwarte) return obecny;break;
     case 2 : if (!obecny->drzwi.otwarte) return obecny;break;
    }
  obecny=obecny->nast;
 }
 return NULL;
}
lschody *znajdz_wyjscie(lschody *wejscie)
{
 lschody *obecny=pietro[wejscie->schody.n_pietra].schody;
 while (obecny)
 {
  if (obecny->schody.n_schodow==wejscie->schody.n_schodow) return obecny;
  obecny=obecny->nast;
 }
 return NULL;
}

int dodaj_schody( int np1, int x1, int y1, int np2, int x2, int y2, bool aktywne)
{
 static int nr=0; //Kolejno coRAZ WIEKSZE NUMERY
 lschody *s1=new lschody,
         *s2=new lschody;
 s1->nast=pietro[np1].schody;
 s1->schody.n_pietra=np2;
 s1->schody.x=x1;
 s1->schody.y=y1;
 s1->schody.n_schodow=nr;
 s1->schody.aktywne=aktywne;
 s2->nast=pietro[np2].schody;
 s2->schody.n_pietra=np1;
 s2->schody.x=x2;
 s2->schody.y=y2;
 s2->schody.n_schodow=nr;
 s2->schody.aktywne=aktywne;
 pietro[np1].schody=s1;
 pietro[np2].schody=s2;
 nr++;
}
void tworz_schody(int np1, int np2, bool aktywne)
{
  int x1,y1,x2,y2;
  do
  {
    x1=los(1,pietro[np1].dl);
    y1=los(1,pietro[np1].sz);
  } while ( !dozw_ruch(pietro[np1].mapa[x1][y1]) );
  do
  {
    x2=los(1,pietro[np2].dl);
    y2=los(1,pietro[np2].sz);
  } while ( !dozw_ruch(pietro[np2].mapa[x2][y2]) );
  dodaj_schody(np1,x1,y1,np2,x2,y2,aktywne);
}
void tworz_schody(int np1, int np2, int x1, int y1, bool aktywne)
{
  int x2,y2;
  do
  {
    x2=los(1,pietro[np2].dl);
    y2=los(1,pietro[np2].sz);
  } while ( !dozw_ruch(pietro[np2].mapa[x2][y2]) );
  dodaj_schody(np1,x1,y1,np2,x2,y2,aktywne);
}
