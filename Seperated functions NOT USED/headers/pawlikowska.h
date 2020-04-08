#ifndef PAWLIKOWSKA_H_INCLUDED
#define PAWLIKOWSKA_H_INCLUDED

using namespace std;

void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B);
SDL_Color getPixel(int x, int y);
void zapiszDedykowana();

/**********************************************************************/
/*  Anna Pawlikowska - Funkcje Huffmana - koduje zawarto��
    tablica_wskaznikow_palety, wynik kodowania trafia do tablica_zakodowana*/
/**********************************************************************/

/*wpisywanie znalezionych kod�w do s�ownika*/
void f_slownik(struct wezel * root, string str, string * odpowiedniki);

/*tworzenie kod�w Huffmana w oparciu o drzewo i kolejk� priorytetow�*/
void f_kody(char data[], int freq[], int size, string * odpowiedniki);

/* u�ywa s�ownika do rozkodowania tablicy tablica_zakodowana, wynik zapisuje do tablicy tablica_rozkodowana
TO S�U�Y DO ODCZYTU ZAKODOWANEJ TABLICY*/
void f_rozkoduj_huffman();

/*g��wne cia�o funkcji, wynik zapisywany do tablicy tablica_zakodowana
TA FUNKCJA INICJUJE I ZAPISUJE TABLIC� DO WPISANIA DO PLIKU*/
void f_huffman(string & Com);

#endif // PAWLIKOWSKA_H_INCLUDED
