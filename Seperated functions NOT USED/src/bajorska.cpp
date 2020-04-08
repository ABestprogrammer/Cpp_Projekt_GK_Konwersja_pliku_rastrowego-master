#include "../headers/bajorska.h"
#include "../headers/pawlikowska.h"
#include "../headers/pepera.h"
#include "../headers/wasik.h"

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
#include <string>

using namespace std;

static SDL_Surface * screen;
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
/* Agata Bajorska - obsługa menu + funkcja main + wczytywanie plikow
   + ładowanie pliku BMP do tab_pixeli+ aktualny podgląd pliku  */
/**********************************************************************/


//----------------------------------------------------------------
void f_bmp_do_tab_pixeli() {
  SDL_Color Pixel; //pojedynczy pixel z obrazka
  SDL_Surface * bmp = SDL_LoadBMP(plik_graficzny_nazwa.c_str());

  //wysokosc i szerokosc obrazka, czyli wymiary tab_pixeli
  width = tab_pixeli_w = bmp -> w;
  height = tab_pixeli_h = bmp -> h;

  tab_pixeli = new SDL_Color * [width];
  for (int i = 0; i < bmp -> w; i++) {
    tab_pixeli[i] = new SDL_Color[height];
  }

  for (int i = 0; i < width; i++) //szerokosc obrazka
  {
    for (int j = 0; j < height; j++) //wysokosc obrazka
    {
      SDL_LockSurface(bmp);
      Uint8 * p = (Uint8 * ) bmp -> pixels + j * bmp -> pitch + i * bmp -> format -> BytesPerPixel;
      SDL_UnlockSurface(bmp);
      //Uint32 a = getpixel(bmp, i,j);
      Pixel.r = * (p + 2);
      Pixel.g = * (p + 1);
      Pixel.b = p[0];

      tab_pixeli[i][j] = Pixel;
      //setPixel(i,j,Pixel.r, Pixel.g,Pixel.b);
    }
  }
  //SDL_Flip(screen);
}

//----------------------------------------------------------------
void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B) {
  if ((x >= 0) && (x < width) && (y >= 0) && (y < height)) {
    /* Zamieniamy poszczególne skladowe koloru na format koloru pixela */
    Uint32 pixel = SDL_MapRGB(screen -> format, R, G, B);

    /* Pobieramy informacji ile bajtów zajmuje jeden pixel */
    int bpp = screen -> format -> BytesPerPixel;

    /* Obliczamy adres pixela */
    Uint8 * p = (Uint8 * ) screen -> pixels + y * screen -> pitch + x * bpp;

    /* Ustawiamy wartosc pixela, w zale¿noœci od formatu powierzchni*/
    switch (bpp) {
    case 1: //8-bit
      *
      p = pixel;
      break;

    case 2: //16-bit
      *
      (Uint16 * ) p = pixel;
      break;

    case 3: //24-bit
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
    /* update the screen (aka double buffering) */
  }
}

//----------------------------------------------------------------
SDL_Color getPixel(int x, int y) {
  SDL_Color color;
  Uint32 col = 0;
  if ((x >= 0) && (x < width) && (y >= 0) && (y < height)) {
    //determine position
    char * pPosition = (char * ) screen -> pixels;
    //offset by y
    pPosition += (screen -> pitch * y);
    //offset by x
    pPosition += (screen -> format -> BytesPerPixel * x);
    //copy pixel data
    memcpy( & col, pPosition, screen -> format -> BytesPerPixel);
    //convert color
    SDL_GetRGB(col, screen -> format, & color.r, & color.g, & color.b);
  }
  return (color);
}

//----------------------------------------------------------------------
/* funkcja przesuwa kursor w lewy gorny rog ekranu consoli */
void f_clear_console() {
  COORD topLeft = {
    0,
    0
  };
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO screen;
  DWORD written;

  GetConsoleScreenBufferInfo(console, & screen);
  FillConsoleOutputCharacterA(
    console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, & written);
  FillConsoleOutputAttribute(
    console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
    screen.dwSize.X * screen.dwSize.Y, topLeft, & written);
  SetConsoleCursorPosition(console, topLeft);
}

//----------------------------------------------------------------------
/* wyswietla obrazek zapisany w plik_graficzny_nazwa w osobnym oknie graficznym */
int f_pokaz_obrazek(string X) {
  ////SDL_Surface *screen = NULL;
  SDL_Surface * obrazek = NULL;
  SDL_Event event; //zdarzenie-nacisniecie klawisza lub zamkniecie okienka
  bool done = false;
  string Com;

  SDL_Color Pixel; //pojedynczy pixel z obrazka
  //SDL_Surface* bmp = SDL_LoadBMP(plik_graficzny_nazwa.c_str());

  SDL_Init(SDL_INIT_EVERYTHING);

  screen = SDL_SetVideoMode(width, height, 32, SDL_SWSURFACE);

  if (X == "z_pliku") {
    obrazek = SDL_LoadBMP(plik_graficzny_nazwa.c_str());
    SDL_BlitSurface(obrazek, NULL, screen, NULL);
  } else if (X == "from_tab_pixeli") {
    for (int i = 0; i < width; i++) //szerokosc obrazka
    {
      for (int j = 0; j < height; j++) //wysokosc obrazka
      {
        Pixel = tab_pixeli[i][j];
        //on global screen
        setPixel(i, j, Pixel.r, Pixel.g, Pixel.b);
      }
    }
  } else if (X == "from_hama") {
    f_odczytajHAMA("inny", Com);
  }

  SDL_Flip(screen);

  //SDL_Delay( 4000 );
  //petla ktora sprawdza nacisniecie Esc na klawiaturze lub zamkniecie okienka
  while (!done) {
    while (SDL_PollEvent( & event)) {
      if (event.key.keysym.sym == SDLK_ESCAPE) {
        done = true;
      }
      if (event.type == SDL_QUIT) {
        done = true;
      }
    }
  }

  SDL_Quit();
  //SDL_FreeSurface( obrazek ); //?

  return 0;
}

//----------------------------------------------------------------------
/* sprawdzenie czy istnieje plik graficzny w biezacym katalogi, zapisanie nazwy do zmiennych globalnych
   _plik = nazwa pliku wprowadzona z klawiatury */
int f_readFile(string X, string _plik) {
  int Ret = 1;

  if (X == "bmp") {
    plik_graficzny_nazwa = "";

    ifstream ifile(_plik.c_str());
    if (ifile.good()) {
      ifile.close();
      plik_graficzny_nazwa = _plik;
      //plik_graficzny = SDL_LoadBMP( plik_graficzny_nazwa.c_str() );
      Ret = 0;
    }
  } else if (X == "hama") {
    plik_graficzny_inny_hama_nazwa = "";

    ifstream ifile(_plik.c_str());
    if (ifile.good()) {
      ifile.close();
      plik_graficzny_inny_hama_nazwa = _plik;
      Ret = 0;
    }
  }

  return Ret;
}

//----------------------------------------------------------------------
void f_menu_header() {
  cout << endl;
  cout << " #################################################" << endl;
  cout << " #" << " PROJEKT GKiM - zmiana formatu graficznego     #" << endl;
  cout << " #################################################" << endl;
}

//----------------------------------------------------------------------
/* menu BMP na HAMA */
void f_menu_konwersja_BMP_HAMA() {
  int M = 1, i = 0;
  string S = "", Com = "";

  while (M != 0) {
    f_clear_console();
    for (i = 0; i < 30; i++) {
      cout << setw(100) << " " << endl;
    }
    f_clear_console();
    f_menu_header();

    cout << " Konwersja z BMP -> HAMA:" << endl;
    cout << " [1] Paleta narzucona z ditheringiem." << endl;
    cout << " [2] Paleta dedykowana z ditheringiem." << endl;
    cout << " [3] Skala szarosci narzucona z ditheringiem." << endl;
    cout << " [4] Skala szarosci dedykowana z ditheringiem." << endl;

    if (plik_graficzny_hama_paleta_narzucona_nazwa != "") {
      cout << " [5] Wyswietl plik HAMA (paleta narzucona z ditheringiem) '" + plik_graficzny_hama_paleta_narzucona_nazwa + "'." << endl;
    } else {
      cout << " [ ] Brak pliku HAMA (paleta narzucona z ditheringiem)" << endl;
    }
    if (plik_graficzny_hama_paleta_dedykowana_nazwa != "") {
      cout << " [6] Wyswietl plik HAMA (paleta dedykowana z ditheringiem) '" + plik_graficzny_hama_paleta_dedykowana_nazwa + "'." << endl;
    } else {
      cout << " [ ] Brak pliku HAMA (paleta dedykowana z ditheringiem)" << endl;
    }
    if (plik_gr_hama_skala_szarosci_narzucona_nazwa != "") {
      cout << " [7] Wyswietl plik HAMA (skala szarosci narzucona z ditheringiem) '" + plik_gr_hama_skala_szarosci_narzucona_nazwa + "'." << endl;
    } else {
      cout << " [ ] Brak pliku HAMA (skala szarosci narzucona z ditheringiem)" << endl;
    }
    if (plik_gr_hama_skala_szarosci_dedykowana_nazwa != "") {
      cout << " [8] Wyswietl plik HAMA (skala szarosci dedykowana z ditheringiem) '" + plik_gr_hama_skala_szarosci_dedykowana_nazwa + "'." << endl;
    } else {
      cout << " [ ] Brak pliku HAMA (skala szarosci dedykowana z ditheringiem)" << endl;
    }

    cout << " [10] Odswiez ekran." << endl;
    cout << " [0] Cofnij." << endl;
    cout << "-------------------------------------------------" << endl;
    if (Com != "") {
      cout << " " << Com << endl;
      Com = "";
    }
    cout << " Wybor: ";
    cin >> S; //c=getchar();

    if (S == "1") {

      Funkcja1(Com);
      f_huffman(Com);
      f_zapisDoHAMA(Com);
      f_zerowanie_tablic();
      f_bmp_do_tab_pixeli();
    } else if (S == "2") {

      Funkcja3(Com);
      f_huffman(Com);
      f_zapisDoHAMA(Com);
      f_zerowanie_tablic();
      f_bmp_do_tab_pixeli();
    } else if (S == "3") {

      Funkcja2(Com);
      f_huffman(Com);
      f_zapisDoHAMA(Com);
      f_zerowanie_tablic();
      f_bmp_do_tab_pixeli();
    } else if (S == "4") {
      Funkcja4(Com);
      f_huffman(Com);
      f_zapisDoHAMA(Com);
      f_zerowanie_tablic();
      f_bmp_do_tab_pixeli();
    } else if (S == "5" && plik_graficzny_hama_paleta_narzucona_nazwa != "") {
      f_wypiszHAMA("paleta_narzucona_z_ditheringiem", Com);
      f_zerowanie_tablic();
    } else if (S == "6" && plik_graficzny_hama_paleta_dedykowana_nazwa != "") {
      f_wypiszHAMA("paleta_dedykowana_z_ditheringiem", Com);
      f_zerowanie_tablic();
    } else if (S == "7" && plik_gr_hama_skala_szarosci_narzucona_nazwa != "") {
      f_wypiszHAMA("skala_szarosci_narzucona_z_ditheringiem", Com);
      f_zerowanie_tablic();
    } else if (S == "8" && plik_gr_hama_skala_szarosci_dedykowana_nazwa != "") {
      f_wypiszHAMA("skala_szarosci_dedykowana_z_ditheringiem", Com);
      f_zerowanie_tablic();
    } else if (S == "10") {
      for (i = 0; i < 60; i++) {
        cout << setw(100) << " " << endl;
      }
    } else if (S == "0") {
      M = 0;
    } else {
      Com = " Zly wybor!";
    }
  }

}

//----------------------------------------------------------------------
/* menu HAMA na BMP */
void f_menu_konwersja_HAMA_BMP() {
  int M = 1, i = 0;
  string S = "", Com = "";

  while (M != 0) {
    f_clear_console();
    for (i = 0; i < 30; i++) {
      cout << setw(100) << " " << endl;
    }
    f_clear_console();
    f_menu_header();

    cout << " Konwersja z HAMA -> BMP:" << endl;
    if (plik_graficzny_hama_paleta_narzucona_nazwa != "") {
      cout << " [1] Rozpocznij konwersje pliku HAMA (paleta narzucona z ditheringiem) '" + plik_graficzny_hama_paleta_narzucona_nazwa + "'." << endl;
    } else {
      cout << " [ ] Konwersja dostepna dopiero po utworzeniu pliku HAMA (paleta narzucona z ditheringiem)" << endl;
    }
    if (plik_graficzny_hama_paleta_dedykowana_nazwa != "") {
      cout << " [2] Rozpocznij konwersje pliku HAMA (paleta dedykowana z ditheringiem) '" + plik_graficzny_hama_paleta_dedykowana_nazwa + "'." << endl;
    } else {
      cout << " [ ] Konwersja dostepna dopiero po utworzeniu pliku HAMA (paleta dedykowana z ditheringiem)" << endl;
    }
    if (plik_gr_hama_skala_szarosci_narzucona_nazwa != "") {
      cout << " [3] Rozpocznij konwersje pliku HAMA (skala szarosci narzucona z ditheringiem) '" + plik_gr_hama_skala_szarosci_narzucona_nazwa + "'." << endl;
    } else {
      cout << " [ ] Konwersja dostepna dopiero po utworzeniu pliku HAMA (skala szarosci narzucona z ditheringiem)" << endl;
    }
    if (plik_gr_hama_skala_szarosci_dedykowana_nazwa != "") {
      cout << " [4] Rozpocznij konwersje pliku HAMA (skala szarosci dedykowana z ditheringiem) '" + plik_gr_hama_skala_szarosci_dedykowana_nazwa + "'." << endl;
    } else {
      cout << " [ ] Konwersja dostepna dopiero po utworzeniu pliku HAMA (skala szarosci dedykowana z ditheringiem)" << endl;
    }

    cout << " [10] Odswiez ekran." << endl;
    cout << " [0] Cofnij." << endl;
    cout << "-------------------------------------------------" << endl;
    if (Com != "") {
      cout << " " << Com << endl;
      Com = "";
    }
    cout << " Wybor: ";
    cin >> S;

    if (S == "1" && plik_graficzny_hama_paleta_narzucona_nazwa != "") {

      /* WPIERW IDZIE FUNKCJA - PODAJ LOKALIZACJE PLIKU HAMA */
      //          f_odczytajHAMA("plik.hama");/* POTEM FUNKCJA ODCZYTUJACA DANE Z PLIKU HAMA */
      f_odczytajHAMA("paleta_narzucona_z_ditheringiem", Com); /* POTEM FUNKCJA ODCZYTUJACA DANE Z PLIKU HAMA */
      f_rozkoduj_huffman();
      /* Utworzenie pliku BMP na podstawie HAMA */
      f_utworzPrzestrzenBMP(Com, 1);
      f_zerowanie_tablic();

    } else if (S == "2" && plik_graficzny_hama_paleta_dedykowana_nazwa != "") {
      // konwersja
      f_odczytajHAMA("paleta_dedykowana_z_ditheringiem", Com);
      f_rozkoduj_huffman();

      /* Utworzenie pliku BMP na podstawie HAMA */
      f_utworzPrzestrzenBMP(Com, 2);
      f_zerowanie_tablic();
    } else if (S == "3" && plik_gr_hama_skala_szarosci_narzucona_nazwa != "") {
      // konwersja
      f_odczytajHAMA("skala_szarosci_narzucona_z_ditheringiem", Com);
      f_rozkoduj_huffman();

      /* Utworzenie pliku BMP na podstawie HAMA */
      f_utworzPrzestrzenBMP(Com, 3);
      f_zerowanie_tablic();
    } else if (S == "4" && plik_gr_hama_skala_szarosci_dedykowana_nazwa != "") {
      // konwersja
      f_odczytajHAMA("skala_szarosci_dedykowana_z_ditheringiem", Com);
      f_rozkoduj_huffman();

      /* Utworzenie pliku BMP na podstawie HAMA */
      f_utworzPrzestrzenBMP(Com, 4);
      f_zerowanie_tablic();
    } else if (S == "10") {
      for (i = 0; i < 60; i++) {
        cout << setw(100) << " " << endl;
      }
    } else if (S == "0") {
      M = 0;
    } else {
      Com += " Zly wybor!";
    }
  }

}
