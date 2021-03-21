#include "glowny.h"
void atak_wrecz(lpostac *atakujacy, lpostac *obronca)
{
// Tpostac &atakujacy, Tpostac &obronca
 Topis_przedmiotu bron_a=opis(atakujacy->postac.nasobie[NS_P_REKA]);
 Topis_przedmiotu bron_o=opis(obronca->postac.nasobie[NS_P_REKA]);
 Topis_przedmiotu zbroja=opis(obronca->postac.nasobie[NS_TULOW]);
 Topis_przedmiotu tarcza=opis(obronca->postac.nasobie[NS_L_REKA]);
 Topis_przedmiotu helm=opis(obronca->postac.nasobie[NS_GLOWA]);
// if (atakujacy->postac.nasobie[NS_P_REKA].typ==0) {bron_a.p[1]=0;bron_a.p[2]=4;bron_a.p[3]=0;}
// if (obronca->postac.nasobie[NS_P_REKA].typ==0) {bron_o.p[1]=0;bron_o.p[2]=4;bron_o.p[3]=0;}
 int skill_a= premia_u(atakujacy->postac. umiejetnosc[bron_a.p[1]]);
 int skill_o= premia_u(obronca->postac. umiejetnosc[bron_o.p[1]]);

 ////////KOD WALKI/////////////

 //cprintf("liczba z=%i t=%i h=%i ",zbroja.p[1],tarcza.p[1],helm.p[1]);
 int PO = 10 + obronca->postac.KP + premia(obronca->postac.stat[1]);
 if ( obronca->postac.stan[STAN_NIEWIDZIALNOSC] ) PO+=5;
 int PA = skill_a/2 +
     premia(atakujacy->postac.stat[ST_ZR]) +
     atakujacy->postac.PdA+
     bron_a.p[5] +
     bron_a.p[6];
 if (atakujacy->postac.niewidoczny)
  PA+=premia_u(atakujacy->postac.umiejetnosc[UM_ZASKAKIWANIE]);
 //piszXY(1,1,15,"%s na %s ska=%i skb=%i PO=%i  PA=%i  ",atakujacy->postac.imie,obronca->postac.imie,skill_a,skill_o,PO,PA);
 int rzut=los(1,20);
 //cprintf(" rzut = %i ",rzut);
 atakujacy->postac.PD-=100;
 if ( (rzut==20 || rzut>(PO-PA)) && rzut>1)
 {
  int obr=0;
  //for (int i=0;i<bron_a.p2;i++)
  obr=los(1, max(1,bron_a.p[3])) +bron_a.p[4]+bron_a.p[6];
  obr+= los(0, skill_a/2) + premia(atakujacy->postac.stat[0]);
  if ( los(1,20) < 2 ) obr*=2;
  if (atakujacy->postac.niewidoczny)
   obr+=atakujacy->postac.umiejetnosc[UM_ZASKAKIWANIE];
  obr-=zbroja.p[2];
  if (obr<0) obr=0;
  atakujacy->postac.niewidoczny=false;
  obrazenia (atakujacy->postac.sila,obronca,OBR_NORMAL,obr);
  if (bron_a.czary)
  {
   lczar *obecny=bron_a.czary;
   while (obecny)
   {
    if (obecny->czar.tryb==2) dodaj_czar(obronca,obecny->czar,atakujacy->postac.sila);
    obecny=obecny->nast;
   }
  }
 }
 else info("%s nie trafiˆ !",atakujacy->postac.imie);
 //getch();
}
void atak_dystans(lpostac *atakujacy, lpostac *obronca)
{
 if (atakujacy->postac.nasobie[NS_AMMO].typ==0)
 {
  piszXY(1,1,15,"Nie masz juz amunicji.");
  getch();
  return;
 }
 Topis_przedmiotu bron_a=opis(atakujacy->postac.nasobie[NS_P_REKA]);
 Topis_przedmiotu ammo=opis(atakujacy->postac.nasobie[NS_AMMO]);
 if (ammo.p[1]!=bron_a.p[2])
 {
  info("Ta broä wymaga innej amunicji.");
  return;
 }
 int skill_a= premia_u(atakujacy->postac. umiejetnosc[bron_a.p[1]]);
// int skill_o= obronca. umiejetnosc[bron_o.p[1]];
 //cprintf("skill %i",skill);
 Topis_przedmiotu zbroja=opis(obronca->postac.nasobie[1]);
 Topis_przedmiotu tarcza=opis(obronca->postac.nasobie[3]);
 Topis_przedmiotu helm=opis(obronca->postac.nasobie[0]);
 int PO=10+obronca->postac.KP;//stat[1] + zbroja.p[1] + tarcza.p[1] + helm.p[1] ;
 if ( obronca->postac.stan[STAN_NIEWIDZIALNOSC] ) PO+=5;
 int d= atakujacy->postac.x-obronca->postac.x +
        atakujacy->postac.y-obronca->postac.y;
 int PA= skill_a +
     premia(atakujacy->postac.stat[1]) +
     atakujacy->postac.PdA+
     bron_a.p[5] +
     bron_a.p[6] +
     ammo.p[3]
     -d;
 //cprintf("ska %i PO=%i  PA=%i",skill_a,PO,PA);
 int rzut=los(1,20);
 atakujacy->postac.PD-=100;
 atakujacy->postac.nasobie[NS_AMMO].ilosc--;
 animacja( atakujacy->postac.x,atakujacy->postac.y,
           obronca->postac.x,obronca->postac.y,45,15,70);
 if (atakujacy->postac.nasobie[NS_AMMO].ilosc<1) atakujacy->postac.nasobie[NS_AMMO]=Pnic;
 if ( (rzut==20 || rzut>(PO-PA)) && rzut>1)
 {
  int obr=0;
  obr=los(1,ammo.p[2])+ammo.p[3] + bron_a.p[4] + bron_a.p[6];
  obr+=los(0 , skill_a / 4);
  if ( los(1,20) < 2 ) obr*=2;
  obr-=zbroja.p[2];
  if (obr<0) obr=0;
  //cprintf(" rzut = %i  strzal za %i hp! ",rzut,obr);
  atakujacy->postac.niewidoczny=false;
  obrazenia (atakujacy->postac.sila,obronca, OBR_NORMAL, obr);
  if (ammo.czary)
  {
   lczar *obecny=ammo.czary;
   while (obecny)
   {
    if (obecny->czar.tryb==2) dodaj_czar(obronca,obecny->czar,atakujacy->postac.sila);
    obecny=obecny->nast;
   }
  }
 }
 else info("%s nie trafiˆ !",atakujacy->postac.imie);

}
void atak(lpostac *atakujacy, int np, int x, int y)
{
 lpostac *obronca=znajdz_postac(x,y,np);
 if (obronca==NULL) return;
 Topis_przedmiotu bron_a=opis(atakujacy->postac.nasobie[NS_P_REKA]);
 if (bron_a.p[1]==3) atak_dystans ( atakujacy, obronca );
                else atak_wrecz ( atakujacy, obronca );
}
bool obrazenia (int zrodlo, int x, int y, int rodzaj, int obr, int np)
{
 if (np<0) np=np_gr;
 lpostac *cel=NULL;
 if ( nalezy(1,1,pietro[np].dl,pietro[np].sz,x,y) )
      if ( jest_Hero(x,y,np) ) obrazenia(zrodlo,lHero,rodzaj,obr);
       else
       {
        cel=znajdz_npc(x,y,np);
        if (cel) obrazenia(zrodlo,cel,rodzaj,obr);
       }
}
bool obrazenia (int zrodlo, lpostac *cel, int rodzaj, int obr) //1 normal magic fire ice acid
{
 if (cel->postac.martwy) return true;
 textcolor(15);
 if ( cel->postac.stan[STAN_NIEWRAZLIWOSC] )
 {
   info("%s - atak nieskuteczny.",cel->postac.imie);
   return true;
 }
 if ( cel->postac.stan[STAN_NIEPRZYTOMNY] )
 {
   usun_efekt(cel,EFEKT_SEN);// atak budzi obronce
   //postac.stan[STAN_SEN]=false;
 }
 bool odp=true;
 int obr_s=cel->postac.obr;
 switch (rodzaj)
 {
   case OBR_NORMAL : normal_obr (cel,obr);break;
   case OBR_MAGIA  : magic_obr  (cel,obr);break;
   case OBR_OGIEN  : ogien_obr  (cel,obr);break;
   case OBR_MROZ   : mroz_obr   (cel,obr);break;
   case OBR_KWAS   : kwas_obr   (cel,obr);break;
   case OBR_ELEKT  : elekt_obr  (cel,obr);break;
 }
 if (zrodlo) if ( (cel->postac.obr<obr_s) && cel->postac.sila!=zrodlo )
 {
  zmien_ai_status(cel->postac.sila,zrodlo,-5);
 }
 if ( cel==lHero)
 {
  Hero=cel->postac;
  if (cel->postac.obr<=0) Hero_smierc();
  return true;
 }
 if (cel->postac.obr<=0 )   ///cos trza poprawic chyba w tabeli w grze
 {
//   info("%s",zrodlo->postac.imie);
   if ( zrodlo==1) dodaj_doswiadczenie (lHero, ile_dosw( Hero.poziom, &cel->postac ) );
   info("%s nie ¾yje ! ",cel->postac.imie);
   zmien_ai_status(cel->postac.sila,zrodlo,-10);
   cel->postac.martwy=true;
   odp=false;
 }
// generuj_bufor();
// rysuj_bufor();
 return odp;
// getch();
}
void normal_obr (lpostac *cel, int obr)
{
// Topis_przedmiotu zbroja = opis (postac.nasobie[1]);
 gotoxy(1,14);
// cprintf("obr1 %i DT=%i  DR=%i ",obr,postac.odpornosc_DT[0],postac.odpornosc_DR[0]);
 obr=(int) (float (obr * (100-cel->postac.odpornosc_DR[0])) / 100 + 0.5 );
 obr-=cel->postac.odpornosc_DT[0];
 if (obr<0) obr=0;
 cel->postac.obr-=obr;
 if (obr>0) info("%s otrzymuje %i obra¾eä. ",cel->postac.imie,obr);
 if (obr<1) info("%s nie otrzymuje obra¾eä. ",cel->postac.imie);
}
void magic_obr (lpostac *cel, int obr)
{
 obr=(int) (float (obr * (100-cel->postac.odpornosc_DR[1])) / 100 + 0.5 );
 obr-=cel->postac.odpornosc_DT[1];
 if (obr<0) obr=0;
 cel->postac.obr-=obr;
 gotoxy(1,14);
 if (obr>0) info("%s otrzymuje %i obra¾eä od magii. ",cel->postac.imie,obr);
 if (obr<1) info("%s nie otrzymuje obra¾eä. ",cel->postac.imie);
}
void ogien_obr (lpostac *cel, int obr)
{
 obr=(int) (float (obr * (100-cel->postac.odpornosc_DR[2])) / 100 + 0.5 );
 obr-=cel->postac.odpornosc_DT[2];
 if (obr<0) obr=0;
 cel->postac.obr-=obr;
 gotoxy(1,14);
 if (obr>0) info("%s otrzymuje %i obra¾eä od ognia. ",cel->postac.imie,obr);
 if (obr<1) info("%s nie otrzymuje obra¾eä. ",cel->postac.imie);
}
void mroz_obr (lpostac *cel, int obr)
{
 obr=(int) (float (obr * (100-cel->postac.odpornosc_DR[3])) / 100 + 0.5 );
 obr-=cel->postac.odpornosc_DT[3];
 if (obr<0) obr=0;
 cel->postac.obr-=obr;
 gotoxy(1,14);
 if (obr>0) info("%s otrzymuje %i obra¾eä od mrozu. ",cel->postac.imie,obr);
 if (obr<1) info("%s nie otrzymuje obra¾eä. ",cel->postac.imie);
}
void kwas_obr (lpostac *cel, int obr)
{
 obr=(int) (float (obr * (100-cel->postac.odpornosc_DR[4])) / 100 + 0.5 );
 obr-=cel->postac.odpornosc_DT[4];
 if (obr<0) obr=0;
 cel->postac.obr-=obr;
 gotoxy(1,14);
 if (obr>0) info("%s otrzymuje %i obra¾eä od kwasu. ",cel->postac.imie,obr);
 if (obr<1) info("%s nie otrzymuje obra¾eä. ",cel->postac.imie);
}
void elekt_obr (lpostac *cel, int obr)
{
 obr=(int) (float (obr * (100-cel->postac.odpornosc_DR[5])) / 100 + 0.5 );
 obr-=cel->postac.odpornosc_DT[5];
 if (obr<0) obr=0;
 cel->postac.obr-=obr;
 gotoxy(1,14);
 if (obr>0) info("%s otrzymuje %i obra¾eä od pr¥du. ",cel->postac.imie,obr);
 if (obr<1) info("%s nie otrzymuje obra¾eä. ",cel->postac.imie);
}
bool _obrazenia (int zrodlo, lpostac *postac, int rodzaj,int obr)
{
 if (obr>=0) return obrazenia(zrodlo,postac, rodzaj ,obr);
 int ile=0;
 obr=0;
}
//typedef bool** Tobszar;
int generuj_obszar( bool obszar[Mobszar][Mobszar],int x, int y, int rodzaj, int p[10])
{
// bool obszar[Mobszar][Mobszar];
 for (int i=0;i<Mobszar;i++)
  for (int j=0;j<Mobszar;j++)
   obszar[i][j]=false;
 switch (rodzaj)
 {
  case OBSZAR_WYBUCH :
       {
         int xw,yw,d,zasieg=p[0]*p[0];
         for (int i=0;i<Mobszar;i++)
          for (int j=0;j<Mobszar;j++)
          {
            xw=x+i-Mobszar/2;
            yw=y+j-Mobszar/2;
            if ( nalezy(1,1,pietro[np_gr].dl,pietro[np_gr].sz,xw,yw) )
            if ( przezroczysty(pietro[np_gr].mapa[xw][yw]) )
            {
              d=(xw-x)*(xw-x) + (yw-y) * (yw-y)  ;
              if (d<=zasieg)
              if ( czy_widzi(xw,yw,x,y) ) obszar[i][j]=true;
            }
          }
       }
       break;
  case OBSZAR_KOLO :
       {
         int xw,yw,d,zasieg=p[0]*p[0];
         for (int i=0;i<Mobszar;i++)
          for (int j=0;j<Mobszar;j++)
          {
            xw=x+i-Mobszar/2;
            yw=y+j-Mobszar/2;
            if ( nalezy(1,1,pietro[np_gr].dl,pietro[np_gr].sz,xw,yw) )
            {
              d=(xw-x)*(xw-x) + (yw-y) * (yw-y)  ;
              if (d<=zasieg) obszar[i][j]=true;
            }
          }
       }
       break;
  case OBSZAR_STOZEK ://p0,p1 - kierunek p2-zasieg
       {
        bool na_ukos=true;
        int dx=sgn(x-p[0]),
            dy=sgn(y-p[1]);
        if (dx*dy==0) na_ukos=false;
        if (na_ukos)
        {
         int zasieg=p[2]+1;
         int xw,yw,xp,yp;
         for (int i=1;i<=zasieg;i++)
         {
          for (int j=0;j<=i;j++)
          {
           xw=dx*(i-j);
           yw=dy*j;
           xp=p[0]+xw;
           yp=p[1]+yw;
           if ( nalezy(1,1,pietro[np_gr].dl,pietro[np_gr].sz,xp,yp) )
            if ( przezroczysty(pietro[np_gr].mapa[xp][yp]) )
              if ( czy_widzi(xp,yp,p[0],p[1]) )
                 obszar[xw+Mobszar/2][yw+Mobszar/2]=true;
          }
         }
        }
        else
        {
         int zasieg=p[2];
         int xw,yw,xp,yp;
         for (int i=1;i<=zasieg;i++)
          for (int j=-i;j<=i;j++)
          {
           if (dx==0)
           {
            xw=j;
            yw=dy*i;
           }
           else
           {
            xw=dx*i;
            yw=j;
           }
           xp=p[0]+xw;
           yp=p[1]+yw;
           if ( nalezy(1,1,pietro[np_gr].dl,pietro[np_gr].sz,xp,yp) )
            if ( przezroczysty(pietro[np_gr].mapa[xp][yp]) )
              if ( czy_widzi(xp,yp,p[0],p[1]) )
                 obszar[xw+Mobszar/2][yw+Mobszar/2]=true;
          }
        }
       }
       break;
 }
 return 0;
}
int generuj_obszar( int **obszar, int x, int y, int rodzaj, int p[10])
{
 const int dl_max=500;
 int pom[dl_max];
 int dl=0;
 switch (rodzaj)
 {
  case OBSZAR_ODCINEK :
       {
        float xc=p[0]-x,yc=p[1]-y;
        float odl= sqrt (xc*xc+yc*yc) ; if (odl==0) return 0;
        float dx= xc/odl, dy= yc/odl;
        float dt=sqrt(dx*dx+dy*dy),d=0;
        float xw=float(x)+0.5,yw=float(y)+0.5;
        bool koniec=false;
        int i=0;
        while (!koniec)
        {
         koniec=true;
         if ( nalezy (1,1,pietro[np_gr].dl,pietro[np_gr].sz, int(xw), int(yw) ) )
          if ( przezroczysty (pietro[np_gr].mapa[int(xw)][int(yw)]) )
          {
           if ( int(xw)!=p[0] || int(yw)!=p[1] ) koniec=false;
           pom[2*i]=int(xw);
           pom[2*i+1]=int(yw);
           i++;
          }
         d+=dt;
         xw+=dx;
         yw+=dy;
        }
        dl=i;
//        if (obszar[2*dl-2]!=;obszar[2*dl-1]=0
       }
       break;
  case OBSZAR_PROSTA :
       {
        float xc=p[0]-x,yc=p[1]-y;
        float odl= sqrt (xc*xc+yc*yc) ; if (odl==0) return 0;
        float dx= xc/odl, dy= yc/odl;
        float dt=sqrt(dx*dx+dy*dy),d=0;
        float xw=float(x)+0.5,yw=float(y)+0.5;
        bool koniec=false;
        int i=0;
        while (!koniec)
        {
         koniec=true;
         if ( nalezy (1,1,pietro[np_gr].dl,pietro[np_gr].sz, int(xw), int(yw) ) )
          if ( przezroczysty (pietro[np_gr].mapa[int(xw)][int(yw)]) )
           if (i<=p[2])
           {
            pom[2*i]=int(xw);
            pom[2*i+1]=int(yw);
            i++;
            koniec=false;
           }
         d+=dt;
         xw+=dx;
         yw+=dy;
        }
        dl=i;
       }
       break;
  case OBSZAR_STRUMIEN :
       {
        float xc=p[0]-x,yc=p[1]-y;
        float odl= sqrt (xc*xc+yc*yc) ; if (odl==0) return 0;
        float dx= xc/odl, dy= yc/odl;
        float dt=sqrt(dx*dx+dy*dy),d=dt;
        float xw=float(x)+0.5+dx,yw=float(y)+0.5+dy;
        pom[0]=x;pom[1]=y;
        bool koniec=false;
        int i=1;
        while (!koniec)
        {
         koniec=true;
         if ( nalezy (1,1,pietro[np_gr].dl,pietro[np_gr].sz, int(xw), int(yw) ) )
          if ( przezroczysty (pietro[np_gr].mapa[int(xw)][int(yw)]) )
          {
           if (i<=p[2])
           {
            pom[2*i]=int(xw);
            pom[2*i+1]=int(yw);
            i++;
            koniec=false;
           }
          }
          else
          {
           float kx=1,ky=1;
           float dxp=dx,dyp=dy;
           if (dx>0) kx=-1;
           if ( !przezroczysty (pietro[np_gr].mapa[int(xw-1)][int(yw)]) &&
                !przezroczysty (pietro[np_gr].mapa[int(xw+1)][int(yw)]) )
              dx=-dx;
           if (dy>0) ky=-1;
           if ( !przezroczysty (pietro[np_gr].mapa[int(xw)][int(yw-1)]) &&
                !przezroczysty (pietro[np_gr].mapa[int(xw)][int(yw+1)]))
              dy=-dy;
           if (dxp==dx && dyp==dy)
            if (dx*dy!=0)
            {
             dx=-dy;
             dy=-dxp;
            }
            else
            { dx=-dx;dy=-dy; }
            //dx=(los(0,1)*2-1)*dx;dy=(los(0,1)*2-1)*dy;
           koniec=false;

          }
         d+=dt;
         xw+=dx;
         yw+=dy;
        }
        dl=i;
       }
       break;

 }
 (*obszar)=new int[2*dl];
 for(int i=0; i<(2*dl) ;i++) (*obszar)[i]=pom[i];
 return dl;

}
char efekt_wstrzasy(char znak)
{
 switch (znak)
  {
      case LOCHY :return PIASEK;
      case MUR    :return LOCHY;
      case TRAWA  :return PIASEK;
      case WODA   :return PIASEK;
      case PLOT   :return POPIOL;
      case PIASEK :return SKALA; //176
      case CZASZKA:return POPIOL;
      case SKALA  :return POPIOL;
      case BAGNO  :return WODA;
      case DRZEWO :return TRAWA;
      case POPIOL :return PIASEK;
      case DRZWI_O  :return POPIOL;
      case DRZWI_Z  :return DRZWI_O;
  }
}
int obszar_efekt(int x, int y, int rodzaj)
{
  switch (rodzaj)
  {
   case EFEKT_WYPAL_TRAWE:
        if ( pietro[np_gr].mapa[x][y]==TRAWA)
        /* ||
             pietro[np_gr].mapa[x][y]==PLOT ||
             pietro[np_gr].mapa[x][y]==DRZEWO ||
             pietro[np_gr].mapa[x][y]==DRZWI_O ||
             pietro[np_gr].mapa[x][y]==DRZWI_Z)*/
          pietro[np_gr].mapa[x][y]=POPIOL;
        break;
   case EFEKT_WSTRZASY:
    if (los(1,4)==1)
      pietro[np_gr].mapa[x][y]=efekt_wstrzasy(pietro[np_gr].mapa[x][y]);
   break;
  }
}
int obszar_efekt(bool obszar[Mobszar][Mobszar], int x, int y, int rodzaj )
{
 for (int i=0;i<Mobszar;i++)
  for (int j=0;j<Mobszar;j++)
  {
   int xn=x+i-Mobszar/2;
   int yn=y+j-Mobszar/2;
   if (obszar[i][j])
    if ( nalezy(1,1,pietro[np_gr].dl,pietro[np_gr].sz,xn,yn) )
     obszar_efekt(xn,yn,rodzaj);
  }
}
void obszar_efekt(int *obszar, int dl, int x, int y,int rodzaj)
{
 int xn,yn;
 for (int i=0;i<dl;i++)
 {
   xn=obszar[2*i];
   yn=obszar[2*i+1];
   if ( nalezy(1,1,pietro[np_gr].dl,pietro[np_gr].sz,xn,yn) )
      obszar_efekt(xn,yn,rodzaj);
 }
}
int atak_obszarowy (int zrodlo, int x, int y,int moc,void (*obr)(int,lpostac*,int,int*),int *p )
{
 lpostac *cel=NULL;
 if ( nalezy(1,1,pietro[np_gr].dl,pietro[np_gr].sz,x,y) )
      if ( jest_Hero(x,y) ) (*obr)(zrodlo,lHero,moc,p);
       else
       {
        cel=znajdz_npc(x,y);
        if (cel) (*obr)(zrodlo,cel,moc,p);
       }
}
int atak_obszarowy( int zrodlo, bool obszar[Mobszar][Mobszar],
                    int x, int y, int moc,void (*obr)(int,lpostac*,int,int*),int *p )
{
 for (int i=0;i<Mobszar;i++)
  for (int j=0;j<Mobszar;j++)
   if (obszar[i][j]) atak_obszarowy(zrodlo,x+i-Mobszar/2,y+j-Mobszar/2,moc,obr,p);
 return 0;
}
int atak_obszarowy (int zrodlo, int *obszar, int dl, int moc,void (*obr)(int,lpostac*,int,int*),int* p )
{
 for (int i=0;i<dl;i++) atak_obszarowy(zrodlo,obszar[2*i],obszar[2*i+1],moc,obr,p);
 return 0;
}

