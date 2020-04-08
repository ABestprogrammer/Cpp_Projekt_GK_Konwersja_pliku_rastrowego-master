#include "../headers/bajorska.h"
#include "../headers/pawlikowska.h"
#include "../headers/pepera.h"
#include "../headers/wasik.h"

#include <string>
#include <SDL/SDL.h>
#include <math.h>
#define pi 3.14+
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include<conio.h>
#include <windows.h>
#include <stack>
#include <vector>
#include <queue>

using namespace std;

SDL_Surface * screen;
SDL_Surface * plik_graficzny = NULL;
SDL_Surface * plik_graficzny_po_konwersji = NULL;

// globalne wymiary obrazka wyliczone przy wczytywaniu
int width;
int height;

char
const * tytul = "projekt grafika";

// nazwy plikow
string plik_graficzny_nazwa = "";
string plik_graficzny_inny_hama_nazwa = "";
string plik_graficzny_hama_paleta_narzucona_nazwa = "";
string plik_graficzny_hama_paleta_dedykowana_nazwa = "";
string plik_gr_hama_skala_szarosci_narzucona_nazwa = "";
string plik_gr_hama_skala_szarosci_dedykowana_nazwa = "";

void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B);
SDL_Color getPixel(int x, int y);

/* Tablica przechowywujaca pixele wygenerowane przez odpowiednia funkcje ograniczajaca kolory
Jest to tablica dynamiczna, inicjowana w momencie wywolania ktoregos z algorytmow generowania
kolorow. Zapisywane sa w niej wyniki ograniczania koloru z ditheringiem. Rozmiar tablicy to
[tab_pixeli_w][tab_pixeli_h]!! */
int tab_pixeli_w = 0;
int tab_pixeli_h = 0;
SDL_Color ** tab_pixeli;
SDL_Color ** tab_pixeli_wyjsciowych;

int tablica_kolorow[64][3];
stack < string > stos;
int ** tablica_wskaznikow_palety;

/* 0 - narzucona kolorowa,
   1 - dedykowana kolorowa,
   2 - narzucona szara,
   3 - dedykowana szara */
int nr_palety = -1;

struct S_bledyKolorow {
  int r;
  int g;
  int b;
  int blad;
};

//Ania
string ** tablica_zakodowana; //TO WYCHODZI Z HUFFA I WCHODZI DO PLIKU
string slownik[64]; //TO TRZEBA WPISAÆ DO PLIKU RAZEM Z TABLICA_ZAKODOWANA BO JEST POTRZEBNE DO ODCZYTU
int ** tablica_rozkodowana; //TU TRAFIAJA RZECZY ODKODOWYWANE Z HUFFA

void zapiszDedykowana();

/**********************************************************************/
/*  Hubert Wąsik - generowanie palet czarnobialej/kolorowej
    oraz kompresja obrazu do 64 kolorowego obrazu*/
/**********************************************************************/

//----------------------------------------------------------------------
/* Funkcja tworzaca przestrzen potrzebna do zapisu pliku BMP */
void f_utworzPlikBMP(SDL_Surface ** surface, char
  const * nazwa_pliku, string & Com) {

  if (!surface) {
    SDL_FreeSurface(*surface);
    * surface = NULL;
  }

  SDL_SaveBMP( * surface, nazwa_pliku);
  Com += " Wygenerowano plik BMP. ";
}

//----------------------------------------------------------------------
/* Przeksztalcona funkcja z zajec ustawiajaca wartosci pikseli w przestrzeni */
void f_setPixelBMP(SDL_Surface * surface, int x, int y, Uint8 R, Uint8 G, Uint8 B) {

  if ((x >= 0) && (x < width) && (y >= 0) && (y < height)) {

    Uint32 pixel = SDL_MapRGB(surface -> format, R, G, B);
    int bpp = surface -> format -> BytesPerPixel;
    Uint8 * p = (Uint8 * ) surface -> pixels + y * surface -> pitch + x * bpp;

    switch (bpp) {

    case 1:
      *
      p = pixel;
      break;

    case 2:
      *
      (Uint16 * ) p = pixel;
      break;

    case 3:
      if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
        p[0] = (pixel >> 16) & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = pixel & 0xff;
      } else {
        p[0] = pixel & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = (pixel >> 16) & 0xff;
      }
      break;

    case 4: //32-bit
      *
      (Uint32 * ) p = pixel;
      break;

    }
  }
}

//----------------------------------------------------------------------
/* Funkcja tworzaca przestrzeni potrzebna do zapisu pliku BMP */
void f_utworzPrzestrzenBMP(string & Com, int numer) {

  SDL_Surface * surface;
  surface = SDL_CreateRGBSurface(0, width, height, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);

  if (surface == NULL) {
    Com += "Blad przy tworzeniu przestrzeni pliku BMP. Uruchom ponownie program. ";
  }

  SDL_LockSurface(surface);

  for (int i = 0; i < surface -> w; i++) {
    for (int j = 0; j < surface -> h; j++) {

      f_setPixelBMP(surface, i, j, tablica_kolorow[tablica_rozkodowana[j][i]][0], tablica_kolorow[tablica_rozkodowana[j][i]][1], tablica_kolorow[tablica_rozkodowana[j][i]][2]);
    }
  }

  SDL_UnlockSurface(surface);

  if (numer == 1) f_utworzPlikBMP( & surface, "HAMA - paleta narzucona.bmp", Com);
  else if (numer == 2) f_utworzPlikBMP( & surface, "HAMA - paleta dedykowana.bmp", Com);
  else if (numer == 3) f_utworzPlikBMP( & surface, "HAMA - skala szarosci narzucona.bmp", Com);
  else if (numer == 4) f_utworzPlikBMP( & surface, "HAMA - skala szarosci dedykowana.bmp", Com);
  else if (numer == 5) f_utworzPlikBMP( & surface, "Odczytany plik HAMA.bmp", Com);
  else return;
}

//----------------------------------------------------------------------
/* Funkcja odpowiedzialna za generowanie czarno-bialej palety z kolorowej palety dedykowanej */
void f_genDedykowanaPaletaSzary() {
  int Y;
  for (int i = 0; i < 64; i++) {
    Y = 0.299 * (tablica_kolorow[i][0]) + 0.587 * (tablica_kolorow[i][1]) + 0.114 * (tablica_kolorow[i][2]);
    tablica_kolorow[i][0] = Y;
    tablica_kolorow[i][1] = Y;
    tablica_kolorow[i][2] = Y;
  }
}

//----------------------------------------------------------------------
/* Funkcja odpowiedzialna za generowanie czarno-bialej palety */
void f_genMonoPaleta() {

  int color; // generowany czarnobialy kolor
  for (int i = 0; i < 64; i++) {

    color = i * 255 / 63;

    for (int j = 0; j < 3; j++) {
      tablica_kolorow[i][j] = color;
    }
  }
}

//----------------------------------------------------------------------
/* Funkcja generująca warjacje potrzebne do palety kolorowej narzuconej */
void f_genWarjacji(string kod, int dlugosc_kodu, string stany) {

  if (kod.length() >= dlugosc_kodu) {
    stos.push(kod);
  } else {
    for (int i = 0; i < stany.length(); i++)
      f_genWarjacji(kod + stany[i], dlugosc_kodu, stany);
  }
}

//----------------------------------------------------------------------
/* Funkcja odpowiedzialna za generowanie narzuconej kolorowej palety */
void f_genKolorowaPaleta() {

  /* Stany okreslajace dany kolor RGB, 0 = 0,
  1 = 85, 2 = 171 oraz 3 = 255 */
  string stany = "0123";
  string odczytany_stan;
  char stan;

  /* Uruchamianie funkcji rekurencyjnej w celu wygenerowania ci¹gu
  64 wariacji, gdzie s¹ to odpowiednie zakodowane wartoœci
  RGB ka¿dego koloru, czyli: RGB(0) RGB(1) RGB(2)... RGB(63).
  Wariacje zapisywane s¹ na stosie */
  f_genWarjacji("", 3, stany);

  /* Gdy ju¿ mamy otrzyman¹ tablicê wariacji, uzupe³niamy tablicê palety
  kolorów odpowiednimi wartoœciami: 0 dla 0, 85 dla 1, 171 dla 2 lub 255 dla 3*/
  for (int i = 0; i < 64; i++) {

    odczytany_stan = stos.top();
    stos.pop();

    for (int j = 0; j < 3; j++) {

      stan = odczytany_stan[j];
      if (stan == '0') tablica_kolorow[i][j] = 0;
      if (stan == '1') tablica_kolorow[i][j] = 85;
      if (stan == '2') tablica_kolorow[i][j] = 171;
      if (stan == '3') tablica_kolorow[i][j] = 255;
    }
  }
}

//----------------------------------------------------------------------
/* 64 kolorowy obraz z ditheringiem, paleta narzucona */
void Funkcja1(string & Com) {

  f_genKolorowaPaleta();
  nr_palety = 0;

  /* Funkcja trzecia generuje obrazek jak funkcja 3, ale nanosi korektę */
  S_bledyKolorow blad;
  SDL_Color kolor;

  /* Tworzymy tablicę dynamiczną dwuwymiarową na przechowywanie błędów o rozmiarze A na B, gdzie
  A oznacza szerokość, natomiast B oznacza długość (+2 do obu wartości)
  S_bledyKolorow jest strukturą przechowywującą do 4 błędów - RGB oraz wartosc błąd */
  S_bledyKolorow ** bledy = new S_bledyKolorow * [width + 2];
  for (int i = 0; i < tab_pixeli_w + 2; i++) {
    bledy[i] = new S_bledyKolorow[height + 2];
  }

  /* Utworzenie tablicy przechowywujaca indeksy palety kolorow odpowiednich odczytanych pikseli */
  tablica_wskaznikow_palety = new int * [height];
  for (int i = 0; i < height + 2; i++) {
    tablica_wskaznikow_palety[i] = new int[width];
  }

  /* Zerowanie tablicy dwuwymiarowej dynamicznej błędów */
  for (int i = 0; i < width + 2; i++) {
    for (int j = 0; j < height + 2; j++) {
      bledy[i][j].r = 0;
      bledy[i][j].g = 0;
      bledy[i][j].b = 0;
    }
  }

  /* Przesunięcie wynosi jeden, najmniejszy/index/tab służy do znalezienia najmniejszej odegłości z pośród 64 kolorow palety, są to
  odległości liczone w przestrzeni 3D dlatego taki jest ich wzór!!! */
  int przesuniecie = 1;
  int najmniejszy;
  int index;
  int tab[64];

  for (int i = 0; i < width; ++i)
    for (int j = 0; j < height; ++j) {

      /* Kolor odczytany z tablicy pixeli */
      kolor = tab_pixeli[i][j];

      /* Obliczanie różnicy i wpisywanie wyniku do tablicy */
      for (int k = 0; k < 64; k++) {

        tab[k] =
          (kolor.r + bledy[i + przesuniecie][j].r - tablica_kolorow[k][0]) * (kolor.r + bledy[i + przesuniecie][j].r - tablica_kolorow[k][0]) +
          (kolor.g + bledy[i + przesuniecie][j].g - tablica_kolorow[k][1]) * (kolor.g + bledy[i + przesuniecie][j].g - tablica_kolorow[k][1]) +
          (kolor.b + bledy[i + przesuniecie][j].b - tablica_kolorow[k][2]) * (kolor.b + bledy[i + przesuniecie][j].b - tablica_kolorow[k][2]);
      }

      /* Wyznaczanie najmniejszego indeksu */
      najmniejszy = tab[0];
      index = 0;
      for (int l = 1; l < 64; l++) {
        if (tab[l] < najmniejszy) {
          najmniejszy = tab[l];
          index = l;
        }
      }

      /* Wpisanie odpowiedniego koloru do tablicy pixeli */
      tab_pixeli[i][j].r = tablica_kolorow[index][0];
      tab_pixeli[i][j].g = tablica_kolorow[index][1];
      tab_pixeli[i][j].b = tablica_kolorow[index][2];

      /* Wpisanie odpowiedniego indeksu do tablicy indeksow palety kolorow */
      tablica_wskaznikow_palety[j][i] = index;

      blad.r = kolor.r - tablica_kolorow[index][0];
      blad.g = kolor.g - tablica_kolorow[index][1];
      blad.b = kolor.b - tablica_kolorow[index][2];

      bledy[i + przesuniecie + 1][j].r += (blad.r * 7.0 / 16);
      bledy[i + przesuniecie - 1][j + 1].r += (blad.r * 3.0 / 16);
      bledy[i + przesuniecie][j + 1].r += (blad.r * 5.0 / 16);
      bledy[i + przesuniecie + 1][j + 1].r += (blad.r * 1.0 / 16);

      bledy[i + przesuniecie + 1][j].g += (blad.g * 7.0 / 16);
      bledy[i + przesuniecie - 1][j + 1].g += (blad.g * 3.0 / 16);
      bledy[i + przesuniecie][j + 1].g += (blad.g * 5.0 / 16);
      bledy[i + przesuniecie + 1][j + 1].g += (blad.g * 1.0 / 16);

      bledy[i + przesuniecie + 1][j].b += (blad.b * 7.0 / 16);
      bledy[i + przesuniecie - 1][j + 1].b += (blad.b * 3.0 / 16);
      bledy[i + przesuniecie][j + 1].b += (blad.b * 5.0 / 16);
      bledy[i + przesuniecie + 1][j + 1].b += (blad.b * 1.0 / 16);
    }

  //jak jest wygenerowany plik HAMA , to ustawiamy plik_graficzny_hama_nazwa i nie korzystamy z flagi
  //    flag_zrobiona_kompresja=1;
  // plik_graficzny_hama_paleta_narzucona_nazwa="";
  plik_graficzny_hama_paleta_narzucona_nazwa = plik_graficzny_nazwa + "_paleta_narzucona.hama";
  Com += "Funkcja1() Skonczone. ";
}

//----------------------------------------------------------------------
/* 64 czarnobialy obraz z ditheringiem */
void Funkcja2(string & Com) {

  f_genMonoPaleta();
  nr_palety = 2;

  /* Funkcja trzecia generuje obrazek jak funkcja 3, ale nanosi korektę */
  S_bledyKolorow blad;
  SDL_Color kolor;

  /* Zmienne potrzebne do konwersji w czern-biel */
  int war_Y;
  SDL_Color pixel;

  /* Tworzymy tablicę dynamiczną dwuwymiarową na przechowywanie błędów o rozmiarze A na B, gdzie
  A oznacza szerokość, natomiast B oznacza długość (+2 do obu wartości)
  S_bledyKolorow jest strukturą przechowywującą do 4 błędów - RGB oraz wartosc błąd */
  S_bledyKolorow ** bledy = new S_bledyKolorow * [width + 2];
  for (int i = 0; i < width + 2; i++) {
    bledy[i] = new S_bledyKolorow[height + 2];
  }

  /* Utworzenie tablicy przechowywujaca indeksy palety kolorow odpowiednich odczytanych pikseli */
  tablica_wskaznikow_palety = new int * [height];
  for (int i = 0; i < height + 2; i++) {
    tablica_wskaznikow_palety[i] = new int[width];
  }

  /* Zerowanie tablicy dwuwymiarowej dynamicznej błędów */
  for (int i = 0; i < width + 2; i++) {
    for (int j = 0; j < height + 2; j++) {
      bledy[i][j].r = 0;
      bledy[i][j].g = 0;
      bledy[i][j].b = 0;
    }
  }

  /* Przesunięcie wynosi jeden, najmniejszy/index/tab służy do znalezienia najmniejszej odegłości z pośród 64 kolorow palety, są to
  odległości liczone w przestrzeni 3D dlatego taki jest ich wzór!!! */
  int przesuniecie = 1;
  int najmniejszy;
  int index;
  int tab[64];

  for (int i = 0; i < width; ++i)
    for (int j = 0; j < height; ++j) {

      /* KOLOR TO ODCZYANY DANY PIXEL Z TABLICY */
      pixel = tab_pixeli[i][j];
      war_Y = pixel.r * 0.299 + pixel.g * 0.587 + pixel.b * 0.114;

      kolor.b = kolor.g = kolor.r = war_Y;

      /* Obliczanie różnicy i wpisywanie wyniku do tablicy */
      for (int k = 0; k < 64; k++) {

        tab[k] =
          (kolor.r + bledy[i + przesuniecie][j].r - tablica_kolorow[k][0]) * (kolor.r + bledy[i + przesuniecie][j].r - tablica_kolorow[k][0]) +
          (kolor.g + bledy[i + przesuniecie][j].g - tablica_kolorow[k][1]) * (kolor.g + bledy[i + przesuniecie][j].g - tablica_kolorow[k][1]) +
          (kolor.b + bledy[i + przesuniecie][j].b - tablica_kolorow[k][2]) * (kolor.b + bledy[i + przesuniecie][j].b - tablica_kolorow[k][2]);
      }

      /* Wyznaczanie najmniejszego indeksu */
      najmniejszy = tab[0];
      index = 0;
      for (int l = 1; l < 64; l++) {
        if (tab[l] < najmniejszy) {
          najmniejszy = tab[l];
          index = l;
        }
      }

      /* Wpisanie odpowiedniego indeksu do tablicy indeksow palety kolorow */
      tablica_wskaznikow_palety[j][i] = index;

      /* Wpisanie odpowiedniego koloru do tablicy pixeli */
      tab_pixeli[i][j].r = tablica_kolorow[index][0];
      tab_pixeli[i][j].g = tablica_kolorow[index][1];
      tab_pixeli[i][j].b = tablica_kolorow[index][2];

      blad.r = kolor.r - tablica_kolorow[index][0];
      blad.g = kolor.g - tablica_kolorow[index][1];
      blad.b = kolor.b - tablica_kolorow[index][2];

      bledy[i + przesuniecie + 1][j].r += (blad.r * 7.0 / 16);
      bledy[i + przesuniecie - 1][j + 1].r += (blad.r * 3.0 / 16);
      bledy[i + przesuniecie][j + 1].r += (blad.r * 5.0 / 16);
      bledy[i + przesuniecie + 1][j + 1].r += (blad.r * 1.0 / 16);

      bledy[i + przesuniecie + 1][j].g += (blad.g * 7.0 / 16);
      bledy[i + przesuniecie - 1][j + 1].g += (blad.g * 3.0 / 16);
      bledy[i + przesuniecie][j + 1].g += (blad.g * 5.0 / 16);
      bledy[i + przesuniecie + 1][j + 1].g += (blad.g * 1.0 / 16);

      bledy[i + przesuniecie + 1][j].b += (blad.b * 7.0 / 16);
      bledy[i + przesuniecie - 1][j + 1].b += (blad.b * 3.0 / 16);
      bledy[i + przesuniecie][j + 1].b += (blad.b * 5.0 / 16);
      bledy[i + przesuniecie + 1][j + 1].b += (blad.b * 1.0 / 16);
    }

  //jak jest wygenerowany plik HAMA , to ustawiamy plik_graficzny_hama_nazwa i nie korzystamy z flagi
  //    flag_zrobiona_kompresja=1;
  // plik_graficzny_hama_skala_szarosci_nazwa="";
  plik_gr_hama_skala_szarosci_narzucona_nazwa = plik_graficzny_nazwa + "_skala_szarosci_narzucona.hama";
  Com += "Funkcja2() Skonczone. ";
}

//----------------------------------------------------------------------
/* 64 kolorowy obraz z ditheringiem, paleta dedykowana */
void Funkcja3(string & Com) {

  /* Generowanie palety dedykowanej */
  f_genDedykowanaPaletaKolor();
  nr_palety = 1;

  /* Funkcja trzecia generuje obrazek jak funkcja 3, ale nanosi korektę */
  S_bledyKolorow blad;
  SDL_Color kolor;

  /* Tworzymy tablicę dynamiczną dwuwymiarową na przechowywanie błędów o rozmiarze A na B, gdzie
  A oznacza szerokość, natomiast B oznacza długość (+2 do obu wartości)
  S_bledyKolorow jest strukturą przechowywującą do 4 błędów - RGB oraz wartosc błąd */
  S_bledyKolorow ** bledy = new S_bledyKolorow * [width + 2];
  for (int i = 0; i < width + 2; i++) {
    bledy[i] = new S_bledyKolorow[height + 2];
  }

  /* Utworzenie tablicy przechowywujaca indeksy palety kolorow odpowiednich odczytanych pikseli */
  tablica_wskaznikow_palety = new int * [height];
  for (int i = 0; i < height + 2; i++) {
    tablica_wskaznikow_palety[i] = new int[width];
  }

  /* Zerowanie tablicy dwuwymiarowej dynamicznej błędów */
  for (int i = 0; i < width + 2; i++) {
    for (int j = 0; j < height + 2; j++) {
      bledy[i][j].r = 0;
      bledy[i][j].g = 0;
      bledy[i][j].b = 0;
    }
  }

  /* Przesunięcie wynosi jeden, najmniejszy/index/tab służy do znalezienia najmniejszej odegłości z pośród 64 kolorow palety, są to
  odległości liczone w przestrzeni 3D dlatego taki jest ich wzór!!! */
  int przesuniecie = 1;
  int najmniejszy;
  int index;
  int tab[64];

  for (int i = 0; i < width; ++i)
    for (int j = 0; j < height; ++j) {

      /* Kolor odczytany z tablicy pixeli */
      kolor = tab_pixeli[i][j];

      /* Obliczanie różnicy i wpisywanie wyniku do tablicy */
      for (int k = 0; k < 64; k++) {

        tab[k] =
          (kolor.r + bledy[i + przesuniecie][j].r - tablica_kolorow[k][0]) * (kolor.r + bledy[i + przesuniecie][j].r - tablica_kolorow[k][0]) +
          (kolor.g + bledy[i + przesuniecie][j].g - tablica_kolorow[k][1]) * (kolor.g + bledy[i + przesuniecie][j].g - tablica_kolorow[k][1]) +
          (kolor.b + bledy[i + przesuniecie][j].b - tablica_kolorow[k][2]) * (kolor.b + bledy[i + przesuniecie][j].b - tablica_kolorow[k][2]);
      }

      /* Wyznaczanie najmniejszego indeksu */
      najmniejszy = tab[0];
      index = 0;
      for (int l = 1; l < 64; l++) {
        if (tab[l] < najmniejszy) {
          najmniejszy = tab[l];
          index = l;
        }
      }

      /* Wpisanie odpowiedniego koloru do tablicy pixeli */
      tab_pixeli[i][j].r = tablica_kolorow[index][0];
      tab_pixeli[i][j].g = tablica_kolorow[index][1];
      tab_pixeli[i][j].b = tablica_kolorow[index][2];

      /* Wpisanie odpowiedniego indeksu do tablicy indeksow palety kolorow */
      tablica_wskaznikow_palety[j][i] = index;

      blad.r = kolor.r - tablica_kolorow[index][0];
      blad.g = kolor.g - tablica_kolorow[index][1];
      blad.b = kolor.b - tablica_kolorow[index][2];

      bledy[i + przesuniecie + 1][j].r += (blad.r * 7.0 / 16);
      bledy[i + przesuniecie - 1][j + 1].r += (blad.r * 3.0 / 16);
      bledy[i + przesuniecie][j + 1].r += (blad.r * 5.0 / 16);
      bledy[i + przesuniecie + 1][j + 1].r += (blad.r * 1.0 / 16);

      bledy[i + przesuniecie + 1][j].g += (blad.g * 7.0 / 16);
      bledy[i + przesuniecie - 1][j + 1].g += (blad.g * 3.0 / 16);
      bledy[i + przesuniecie][j + 1].g += (blad.g * 5.0 / 16);
      bledy[i + przesuniecie + 1][j + 1].g += (blad.g * 1.0 / 16);

      bledy[i + przesuniecie + 1][j].b += (blad.b * 7.0 / 16);
      bledy[i + przesuniecie - 1][j + 1].b += (blad.b * 3.0 / 16);
      bledy[i + przesuniecie][j + 1].b += (blad.b * 5.0 / 16);
      bledy[i + przesuniecie + 1][j + 1].b += (blad.b * 1.0 / 16);
    }

  //flag_zrobiona_kompresja=1;
  plik_graficzny_hama_paleta_dedykowana_nazwa = plik_graficzny_nazwa + "_paleta_dedykowana.hama";

  Com += "Funkcja3() Skonczone. ";
}

//----------------------------------------------------------------------
/* 64 czarnobialy obraz z ditheringiem, paleta dedykowana */
void Funkcja4(string & Com) {
  f_genDedykowanaPaletaKolor();
  f_genDedykowanaPaletaSzary();
  nr_palety = 3;

  /* Funkcja trzecia generuje obrazek jak funkcja 3, ale nanosi korektę */
  S_bledyKolorow blad;
  SDL_Color kolor;

  /* Tworzymy tablicę dynamiczną dwuwymiarową na przechowywanie błędów o rozmiarze A na B, gdzie
  A oznacza szerokość, natomiast B oznacza długość (+2 do obu wartości)
  S_bledyKolorow jest strukturą przechowywującą do 4 błędów - RGB oraz wartosc błąd */
  S_bledyKolorow ** bledy = new S_bledyKolorow * [width + 2];
  for (int i = 0; i < width + 2; i++) {
    bledy[i] = new S_bledyKolorow[height + 2];
  }

  /* Utworzenie tablicy przechowywujaca indeksy palety kolorow odpowiednich odczytanych pikseli */
  tablica_wskaznikow_palety = new int * [height];
  for (int i = 0; i < height + 2; i++) {
    tablica_wskaznikow_palety[i] = new int[width];
  }

  /* Zerowanie tablicy dwuwymiarowej dynamicznej błędów */
  for (int i = 0; i < width + 2; i++) {
    for (int j = 0; j < height + 2; j++) {
      bledy[i][j].r = 0;
      bledy[i][j].g = 0;
      bledy[i][j].b = 0;
    }
  }

  /* Przesunięcie wynosi jeden, najmniejszy/index/tab służy do znalezienia najmniejszej odegłości z pośród 64 kolorow palety, są to
  odległości liczone w przestrzeni 3D dlatego taki jest ich wzór!!! */
  int przesuniecie = 1;
  int najmniejszy;
  int index;
  int tab[64];

  for (int i = 0; i < width; ++i)
    for (int j = 0; j < height; ++j) {

      /* Kolor odczytany z tablicy pixeli */
      kolor = tab_pixeli[i][j];

      /* Obliczanie różnicy i wpisywanie wyniku do tablicy */
      for (int k = 0; k < 64; k++) {

        tab[k] =
          (kolor.r + bledy[i + przesuniecie][j].r - tablica_kolorow[k][0]) * (kolor.r + bledy[i + przesuniecie][j].r - tablica_kolorow[k][0]) +
          (kolor.g + bledy[i + przesuniecie][j].g - tablica_kolorow[k][1]) * (kolor.g + bledy[i + przesuniecie][j].g - tablica_kolorow[k][1]) +
          (kolor.b + bledy[i + przesuniecie][j].b - tablica_kolorow[k][2]) * (kolor.b + bledy[i + przesuniecie][j].b - tablica_kolorow[k][2]);
      }

      /* Wyznaczanie najmniejszego indeksu */
      najmniejszy = tab[0];
      index = 0;
      for (int l = 1; l < 64; l++) {
        if (tab[l] < najmniejszy) {
          najmniejszy = tab[l];
          index = l;
        }
      }

      /* Wpisanie odpowiedniego koloru do tablicy pixeli */
      tab_pixeli[i][j].r = tablica_kolorow[index][0];
      tab_pixeli[i][j].g = tablica_kolorow[index][1];
      tab_pixeli[i][j].b = tablica_kolorow[index][2];

      /* Wpisanie odpowiedniego indeksu do tablicy indeksow palety kolorow */
      tablica_wskaznikow_palety[j][i] = index;

      blad.r = kolor.r - tablica_kolorow[index][0];
      blad.g = kolor.g - tablica_kolorow[index][1];
      blad.b = kolor.b - tablica_kolorow[index][2];

      bledy[i + przesuniecie + 1][j].r += (blad.r * 7.0 / 16);
      bledy[i + przesuniecie - 1][j + 1].r += (blad.r * 3.0 / 16);
      bledy[i + przesuniecie][j + 1].r += (blad.r * 5.0 / 16);
      bledy[i + przesuniecie + 1][j + 1].r += (blad.r * 1.0 / 16);

      bledy[i + przesuniecie + 1][j].g += (blad.g * 7.0 / 16);
      bledy[i + przesuniecie - 1][j + 1].g += (blad.g * 3.0 / 16);
      bledy[i + przesuniecie][j + 1].g += (blad.g * 5.0 / 16);
      bledy[i + przesuniecie + 1][j + 1].g += (blad.g * 1.0 / 16);

      bledy[i + przesuniecie + 1][j].b += (blad.b * 7.0 / 16);
      bledy[i + przesuniecie - 1][j + 1].b += (blad.b * 3.0 / 16);
      bledy[i + przesuniecie][j + 1].b += (blad.b * 5.0 / 16);
      bledy[i + przesuniecie + 1][j + 1].b += (blad.b * 1.0 / 16);
    }

  //plik_graficzny_hama_skala_szarosci_nazwa = plik_graficzny_nazwa;
  plik_gr_hama_skala_szarosci_dedykowana_nazwa = plik_graficzny_nazwa + "_skala_szarosci_dedykowana.hama";
  Com += "Funkcja4() Skonczone. ";
}

