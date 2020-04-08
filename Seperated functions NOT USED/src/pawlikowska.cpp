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
/*  Anna Pawlikowska - Funkcje Huffmana - koduje zawartoœæ
    tablica_wskaznikow_palety, wynik kodowania trafia do tablica_zakodowana*/
/**********************************************************************/

//----------------------------------------------------------------------
struct wezel {
  char data;
  unsigned freq;
  wezel * left, * right;

  wezel(char data, unsigned freq) {
    left = right = NULL;
    this -> data = data;
    this -> freq = freq;
  }
};

//----------------------------------------------------------------------
struct compare {
  bool operator()(wezel * l, wezel * r) {
    return (l -> freq > r -> freq);
  }
};

//----------------------------------------------------------------------
/*wpisywanie znalezionych kodów do s³ownika*/
void f_slownik(struct wezel * root, string str, string * odpowiedniki) {
  if (!root)
    return;

  if (root -> data != '}') {
    odpowiedniki[root -> data] = str;
  }
  f_slownik(root -> left, str + "0", odpowiedniki);
  f_slownik(root -> right, str + "1", odpowiedniki);
}

//----------------------------------------------------------------------
/*tworzenie kodów Huffmana w oparciu o drzewo i kolejkê priorytetow¹*/
void f_kody(char data[], int freq[], int size, string * odpowiedniki) {
  struct wezel * left, * right, * top;

  priority_queue < wezel * , vector < wezel * > , compare > najmn;
  for (int i = 0; i < size; ++i)
    najmn.push(new wezel(data[i], freq[i]));

  while (najmn.size() != 1) {
    left = najmn.top();
    najmn.pop();

    right = najmn.top();
    najmn.pop();

    top = new wezel('}', left -> freq + right -> freq);
    top -> left = left;
    top -> right = right;
    najmn.push(top);
  }

  f_slownik(najmn.top(), "", odpowiedniki);
}

//----------------------------------------------------------------------
/* u¿ywa s³ownika do rozkodowania tablicy tablica_zakodowana, wynik zapisuje do tablicy tablica_rozkodowana
TO S£U¯Y DO ODCZYTU ZAKODOWANEJ TABLICY*/
void f_rozkoduj_huffman() {

  tablica_rozkodowana = new int * [height];
  for (int i = 0; i < height; i++) {
    tablica_rozkodowana[i] = new int[width];
  }

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      for (int k = 0; k < 64; k++) {
        if (tablica_zakodowana[i][j] == slownik[k]) {
          tablica_rozkodowana[i][j] = k;
          break;
        }
      }
    }

  }
}

//----------------------------------------------------------------------
/*g³ówne cia³o funkcji, wynik zapisywany do tablicy tablica_zakodowana
TA FUNKCJA INICJUJE I ZAPISUJE TABLICÊ DO WPISANIA DO PLIKU*/
void f_huffman(string & Com) {
  int rozmiar = 0;
  int c = 0, count[64] = {
    0
  };

  tablica_zakodowana = new string * [height];
  for (int i = 0; i < height; i++) {
    tablica_zakodowana[i] = new string[width];
  }

  //zlicza wystêpowanie danych kolorów reprezentowanych wartoœciami int od 0 do 63
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      count[tablica_wskaznikow_palety[i][j]]++;
    }
  }

  for (c = 0; c < 64; c++) {
    if (count[c] != 0)
      rozmiar++;
  }

  //przygotowanie danych do wywo³ania funkcji buduj¹cej kody
  char * arr = new char[rozmiar];
  int * freq = new int[rozmiar];

  int licznik = 0;
  for (c = 0; c < 64; c++) {
    if (count[c] != 0) {
      arr[licznik] = c;
      freq[licznik++] = count[c];
    }
  }

  f_kody(arr, freq, rozmiar, slownik);
  int wyp;
  //wype³nia tablicê wynikow¹
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      wyp = tablica_wskaznikow_palety[i][j];
      tablica_zakodowana[i][j] = slownik[wyp];
    }
  }
  Com += "f_huffman(). Skonczone. ";
}
