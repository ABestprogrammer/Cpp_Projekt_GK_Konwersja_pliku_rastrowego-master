#ifndef BAJORSKA_H_INCLUDED
#define BAJORSKA_H_INCLUDED
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

void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B);
SDL_Color getPixel(int x, int y);
void zapiszDedykowana();

/**********************************************************************/
/* Agata Bajorska - obs³uga menu + funkcja main + wczytywanie plikow
   + ³adowanie pliku BMP do tab_pixeli+ aktualny podgl¹d pliku  */
/**********************************************************************/

void f_bmp_do_tab_pixeli();
void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B);
SDL_Color getPixel(int x, int y);

/* funkcja przesuwa kursor w lewy gorny rog ekranu consoli */
void f_clear_console();

/* wyswietla obrazek zapisany w plik_graficzny_nazwa w osobnym oknie graficznym */
int f_pokaz_obrazek(string X);

/* sprawdzenie czy istnieje plik graficzny w biezacym katalogi, zapisanie nazwy do zmiennych globalnych
   _plik = nazwa pliku wprowadzona z klawiatury */
int f_readFile(string X, string _plik);

void f_menu_header();

/* menu BMP na HAMA */
void f_menu_konwersja_BMP_HAMA();

/* menu HAMA na BMP */
void f_menu_konwersja_HAMA_BMP();

#endif // BAJORSKA_H_INCLUDED
