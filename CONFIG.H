#ifndef __CONFIG__

#define __CONFIG__

///interfejs

#define mapa_dl  200
#define mapa_wys 100
#define ekran_dl 41
#define ekran_wys 17
#define ekran_X 1
#define ekran_Y 1

// Parametry gry
#define Lpieter 13
#define Lstat 6
#define Lnasobie 8 //hl,zb,lr,pr,ammo, am,lp,pp
#define Lodpornosci 6
#define Lumiejetnosci 32
#define Lstan 25
#define Lstan_przedmiot 10
#define Lczar 150
#define Lefekt 30
#define Lrzut 3
#define Lsil 10
#define Lflag 10
#define Lnpc 1000
#define Lzadan 250
#define Lteren 60
#define Mobszar 20

// Klawisze

#define GORA 72
#define DOL 80
#define LEWO 75
#define PRAWO 77
#define ESC 27
#define ENTER 13
#define SPACJA 32
#define BACK 8
#define PLUS 43
#define MINUS 45

// TEREN

#define MUR 0
#define GORY 1
#define SKALA 2
#define CZASZKA 3
#define PLOT 5
#define LOCHY 20
#define TRAWA 21
#define PIASEK 22
#define BAGNO 23
#define POPIOL 24
#define DRZEWO 30
#define JUNGLA 32
#define WODA 35
#define MAGMA 36
#define DRZWI_O 40
#define DRZWI_Z 41
#define PULAPKA 50
#define LODOWIEC 4
#define STEP 27
#define LOD 26
#define SNIEG 25
#define MARMUR 6




/// KODY MGLY

#define WIDOCZNE 32
#define ZNANE 46
#define NIEZNANE 120

// ATRYBUTY
#define ST_SF 0
#define ST_ZR 1
#define ST_KO 2
#define ST_IN 3
#define ST_MD 4
#define ST_CH 5

// KODY STANOW - postac

#define STAN_OK 1
#define STAN_UNIERUCHOMIENIE 2
#define STAN_ZATRUCIE 3
#define STAN_PRZYSPIESZENIE 4
#define STAN_SPOWOLNIENIE 5
#define STAN_NIEWRAZLIWOSC 6
#define STAN_SZAL 7
#define STAN_NIEPRZYTOMNY 8
#define STAN_TARCZA 9
#define STAN_SLEPOTA 10
#define STAN_NIEWIDZIALNOSC 11
#define STAN_WYKRYCIE 12
#define STAN_OSLABIENIE 13
#define STAN_BLOGOSLAWIENSTWO 14
#define STAN_OCHRONA_SMIERC 15
#define STAN_KRWOTOK 16
#define STAN_PSALM 17
#define STAN_STRACH 18
#define STAN_OCHRONA_STRACH 19
#define STAN_REGENERACJA 20
#define STAN_LOT 21
#define STAN_PLYWANIE 22

// KODY STANOW - przedmiot

#define STAN_PRZEKLETY 1
#define STAN_USWIECONY 2
#define STAN_NIEZNANY 3
#define STAN_NIEZNISZCZALNY 4
#define STAN_ZARDZEWIALY 5
#define STAN_PEKNIETY 6

//KODY CZAROW

#define CZAR_USPIENIE 1
#define CZAR_TARCZA 2
#define CZAR_LEKKIE_LECZENIE 5
#define CZAR_BLOGOSLAWIENSTWO 6
#define CZAR_OPLATANIE 7
#define CZAR_MAGICZNY_POCISK 10
#define CZAR_PLONACE_DLONIE 11
#define CZAR_PORAZENIE 12
#define CZAR_GROZA 13//do opisania
#define CZAR_WYKRYWANIE_PULAPEK 14//do opisania
#define CZAR_IDENTYFIKACJA 15 //do opisania

#define CZAR_ZAUROCZENIE 19
#define CZAR_KWASOWA_STRZALA 20
#define CZAR_PLOMIENIE 21
#define CZAR_LODOWA_PETLA 22
#define CZAR_SILA_HEROSA 23
#define CZAR_PRZELAMANIE_STRACHU 24
#define CZAR_SLEPOTA 25
#define CZAR_NIEWIDZIALNOSC 26
#define CZAR_WYKRYCIE_NIEWIDZIALNYCH 27
#define CZAR_PAJECZYNA 28
#define CZAR_SREDNIE_LECZENIE 29

#define CZAR_OGNISTA_STRZALA 30
#define CZAR_KULA_OGNISTA 31
#define CZAR_OTWARCIE 33//do opisania
#define CZAR_SPOWOLNIENIE 34
#define CZAR_BLYSKAWICA 35
#define CZAR_ANTIDOTUM 36
#define CZAR_UNIERUCHOMIENIE 37
#define CZAR_PSALM 38
#define CZAR_OCHRONA_PRZED_OGNIEM 39
#define CZAR_PRZYW_SZKIELETU 40

#define CZAR_POCISKI_MOCY 41
#define CZAR_SCIANA_OGNIA 42
#define CZAR_BURZA 43
#define CZAR_ZAAWANSOWANE_LECZENIE 45
#define CZAR_KAMIENNA_SKORA 50
#define CZAR_OCHRONA_ZYWIOLY 54
#define CZAR_OSLABIENIE 56
#define CZAR_PRZYSPIESZENIE 58

#define CZAR_STOZEK_ZIMNA 60
#define CZAR_POZOGA 61
#define CZAR_POLE_STATYCZNE 62
#define CZAR_OKO_MAGA 63
#define CZAR_WEZWANIE_ZWIERZAT 64

#define CZAR_PRZYW_ZYWIOLAKA 75
#define CZAR_UZDROWIENIE 80
#define CZAR_OCHRONA_PRZED_MAGIA 82
#define CZAR_TELEPORT 85

#define CZAR_PALEC_SMIERCI 103
#define CZAR_STREFA_MROZU 104
#define CZAR_CALUN_PLOMIENI 105
#define CZAR_TRZESIENIE_ZIEMI 106
#define CZAR_OCHRONA_SMIERC 109
#define CZAR_REGENERACJA 110

#define CZAR_PLUGAWY_UWIAD 120
#define CZAR_SM_OGLUSZENIE 121
#define CZAR_BRAMA_PIEKIEL 130

#define CZAR_SKOWYT_BANSHEE 135
#define CZAR_NIEWRAZLIWOSC 140

//EFEKT czarow

#define EFEKT_NIEPRZYTOMNY 0
#define EFEKT_SEN 1
#define EFEKT_ZATRUCIE 2
#define EFEKT_SLEPOTA 3
#define EFEKT_ZAUROCZENIE 4
#define EFEKT_UNIERUCHOMIENIE 5
#define EFEKT_RZUTY 7
#define EFEKT_TARCZA 8
#define EFEKT_PRZYWOLANIE 9
#define EFEKT_OBRAZENIA 10
#define EFEKT_OCHRONA_1 11
#define EFEKT_OCHRONA_2 12
#define EFEKT_OCHRONA_3 13
#define EFEKT_REGENERACJA 14
#define EFEKT_NIEWIDZIALNOSC 15
#define EFEKT_WYKRYCIE 16
#define EFEKT_OSLABIENIE 17
#define EFEKT_BLOGOSLAWIENSTWO 18
#define EFEKT_PRZYSPIESZENIE 19
#define EFEKT_NIEWRAZLIWOSC 20
#define EFEKT_OKO_MAGA 21
#define EFEKT_OCHRONA_SMIERC 22
#define EFEKT_ATRYBUT 23
#define EFEKT_UMIEJETNOSC 24
#define EFEKT_KRWOTOK 25
#define EFEKT_PSALM 26
#define EFEKT_STRACH 27
#define EFEKT_SPOWOLNIENIE 28
#define EFEKT_OCHRONA_STRACH 29

#define EFEKT_ZMIANA_TERENU 50
#define EFEKT_OBRAZENIA_OBSZAR 51
#define EFEKT_OBRAZENIA_STREFA 52
#define EFEKT_OBRAZENIA_PUNKT 53
#define EFEKT_PAJECZYNA 60
#define EFEKT_OPLATANIE 61

#define EFEKT_WYPAL_TRAWE 20
#define EFEKT_ZAMROZ_WODE 21
#define EFEKT_WSTRZASY 22

//RODZAJE OBRAZEN

#define OBR_NORMAL 0
#define OBR_MAGIA 1
#define OBR_OGIEN 2
#define OBR_MROZ 3
#define OBR_KWAS 4
#define OBR_ELEKT 5

// RODZAJE EFEKTOW OBSZAROW
#define OBSZAR_WYBUCH 1
#define OBSZAR_STRUMIEN 2
#define OBSZAR_ODDECH 3
#define OBSZAR_ODCINEK 4
#define OBSZAR_PROSTA 5
#define OBSZAR_KOLO 6
#define OBSZAR_OKRAG 7
#define OBSZAR_STOZEK 8

//RODZAJE RZUTOW
#define RZUT_WYT 0
#define RZUT_UNIK 1
#define RZUT_WOLA 2
#define RZUT_BRAK 3

//NA_SOBIE
#define NS_GLOWA 0
#define NS_TULOW 1
#define NS_P_REKA 2
#define NS_L_REKA 3
#define NS_SZYJA 4
#define NS_P_DLON 5
#define NS_L_DLON 6
#define NS_AMMO 7

#define ID_HERO 1000

#define UM_W_WRECZ 0
#define UM_B_TNACE 1
#define UM_B_OBUCHOWE 2
#define UM_B_STRZELECKIE 3
#define UM_B_DRZEWCOWE 4
#define UM_B_MIOTANE 5
#define UM_B_EGZOTYCZNE 6
#define UM_M_SKUTECZNOSC 7
#define UM_M_MEDYTACJA 8
#define UM_M_ALCHEMIA 9
#define UM_LC_OCHRONA 10
#define UM_LC_ENERGIA 11
#define UM_LC_UMYSL 12
#define UM_LC_NEKROMANCJA 13
#define UM_LC_OGIEN 14
#define UM_LC_MROZ  15
#define UM_LC_ELEKTRO 16
#define UM_LC_NATURA 17
#define UM_LC_ZMYSLY 18
#define UM_LC_PRZEMIANY 19
#define UM_LC_CIEMNOSC 20
#define UM_LC_MODLITWY 21
#define UM_SKRADANIE 22
#define UM_ZASKAKIWANIE 23
#define UM_ZLODZIEJSTWO 24
#define UM_POSZUKIWANIE 25
#define UM_ELOKWENCJA 26
#define UM_KUPIECTWO 27
#define UM_JEZYKI 28
#define UM_NAPRAWA 29
#define UM_ZAMKI 30
#define UM_PULAPKI 31


#endif
