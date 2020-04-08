#ifndef WASIK_H_INCLUDED
#define WASIK_H_INCLUDED

using namespace std;

void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B);
SDL_Color getPixel(int x, int y);
void zapiszDedykowana();

/**********************************************************************/
/*  Hubert W¹sik - generowanie palet czarnobialej/kolorowej
    oraz kompresja obrazu do 64 kolorowego obrazu*/
/**********************************************************************/

/* Funkcja tworzaca przestrzen potrzebna do zapisu pliku BMP */
void f_utworzPlikBMP(SDL_Surface ** surface, char const * nazwa_pliku, string & Com);

/* Przeksztalcona funkcja z zajec ustawiajaca wartosci pikseli w przestrzeni */
void f_setPixelBMP(SDL_Surface * surface, int x, int y, Uint8 R, Uint8 G, Uint8 B);

/* Funkcja tworzaca przestrzeni potrzebna do zapisu pliku BMP */
void f_utworzPrzestrzenBMP(string & Com, int numer);

/* Funkcja odpowiedzialna za generowanie czarno-bialej palety z kolorowej palety dedykowanej */
void f_genDedykowanaPaletaSzary();

/* Funkcja odpowiedzialna za generowanie czarno-bialej palety */
void f_genMonoPaleta() ;

/* Funkcja generuj¹ca warjacje potrzebne do palety kolorowej narzuconej */
void f_genWarjacji(string kod, int dlugosc_kodu, string stany);

/* Funkcja odpowiedzialna za generowanie narzuconej kolorowej palety */
void f_genKolorowaPaleta();

/* 64 kolorowy obraz z ditheringiem, paleta narzucona */
void Funkcja1(string & Com);

/* 64 czarnobialy obraz z ditheringiem */
void Funkcja2(string & Com);

/* 64 kolorowy obraz z ditheringiem, paleta dedykowana */
void Funkcja3(string & Com);

/* 64 czarnobialy obraz z ditheringiem, paleta dedykowana */
void Funkcja4(string & Com);

#endif // WASIK_H_INCLUDED
