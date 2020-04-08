#ifndef PEPERA_H_INCLUDED
#define PEPERA_INCLUDED

using namespace std;

void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B);
SDL_Color getPixel(int x, int y);
void zapiszDedykowana();

/**********************************************************************/
/*  Marzena Pepera - generowanie tablicy barw dedykowanej kolorowej oraz
    czarno bialej, zapis i odczyt pliku .hama */
/**********************************************************************/

/*Funkcja zeruj¹ca tablice z danymi w celu unikniêcia kolizji*/
void f_zerowanie_tablic();

/* Funkcja zapisuj¹ca dane obrazu do pliku z rozszerzeniem .hama */
void f_zapisDoHAMA(string & Com);
void f_genKolorowaPaleta();
void f_genMonoPaleta();
void f_genWarjacji(string kod, int dlugosc_kodu, string stany);

/* Funkcja odczytuj¹ca dane z pliku z rozszerzeniem .hama do tablic w programie */
int f_odczytajHAMA(string X, string & Com);

/* Funkcja wypisuj¹ca dane z pliku z rozszerzeniem .hama do konsoli */
int f_wypiszHAMA(string X, string & Com);

void f_genDedykowanaPaletaKolor();

#endif // PEPERA_H_INCLUDED
