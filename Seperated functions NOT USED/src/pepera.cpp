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
/*  Marzena Pepera - generowanie tablicy barw dedykowanej kolorowej oraz
    czarno bialej, zapis i odczyt pliku .hama */
/**********************************************************************/
/*Funkcja zeruj¹ca tablice z danymi w celu unikniêcia kolizji*/
void f_zerowanie_tablic() {
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      tablica_zakodowana[i][j] = "";
    }
  }
  for (int i = 0; i < 64; i++)
    slownik[i] = "";
  for (int i = 0; i < 64; i++)
    for (int j = 0; j < 3; j++) {
      tablica_kolorow[i][j] = 0;
    }
}

//----------------------------------------------------------------------
/* Funkcja zapisuj¹ca dane obrazu do pliku z rozszerzeniem .hama */
void f_zapisDoHAMA(string & Com) {

  ofstream zapis;
  stringstream ss; //ss - zmienna do tworzenia stringow, komentarzy
  ss.str("");
  ss.clear();
  int rozmiar = 20 + 12 + 64 + (width * height + sizeof(string));
  int off_Naglowek = 20;
  int off_PaletaKolorow = -1;
  int off_TablicaAlfabetu = 20 + 12;
  int off_TablicaPikseli = 20 + 12 + 64;

  if (nr_palety == 0) {
    zapis.open(plik_graficzny_hama_paleta_narzucona_nazwa.c_str(), ios::binary);
  } else if (nr_palety == 1) {
    rozmiar += 64 * 3;
    off_PaletaKolorow = 20 + 12;
    off_TablicaAlfabetu += 64 * 3;
    off_TablicaPikseli += 64 * 3;
    zapis.open(plik_graficzny_hama_paleta_dedykowana_nazwa.c_str(), ios::binary);
  } else if (nr_palety == 2) {
    //zapis.open(plik_graficzny_hama_skala_szarosci_nazwa.c_str(), ios::binary);
    zapis.open(plik_gr_hama_skala_szarosci_narzucona_nazwa.c_str(), ios::binary);
  } else if (nr_palety == 3) {
    rozmiar += 64;
    off_PaletaKolorow = 20 + 12;
    off_TablicaAlfabetu += 64;
    off_TablicaPikseli += 64;
    //zapis.open(plik_graficzny_hama_skala_szarosci_nazwa.c_str(), ios::binary);
    zapis.open(plik_gr_hama_skala_szarosci_dedykowana_nazwa.c_str(), ios::binary);
  } else {
    ss << "Blad - brak palety";
    Com += ss.str();
  }
  zapis.write((char * ) & (rozmiar), sizeof(rozmiar));
  zapis.write((char * ) & (off_Naglowek), sizeof(off_Naglowek));
  zapis.write((char * ) & (off_PaletaKolorow), sizeof(off_PaletaKolorow));
  zapis.write((char * ) & (off_TablicaAlfabetu), sizeof(off_TablicaAlfabetu));
  zapis.write((char * ) & (off_TablicaPikseli), sizeof(off_TablicaPikseli));
  zapis.write((char * ) & (width), sizeof(width));
  zapis.write((char * ) & (height), sizeof(height));
  zapis.write((char * ) & (nr_palety), sizeof(nr_palety));

  if (nr_palety == 1) {
    for (int i = 0; i < 64; i++) //zapis palety dedykowanej kolorowej
    {
      zapis.write((char * ) & (tablica_kolorow[i][0]), sizeof(tablica_kolorow[i][0]));
      zapis.write((char * ) & (tablica_kolorow[i][1]), sizeof(tablica_kolorow[i][1]));
      zapis.write((char * ) & (tablica_kolorow[i][2]), sizeof(tablica_kolorow[i][2]));
    }
  } else if (nr_palety == 3) {
    for (int i = 0; i < 64; i++) //zapis palety dedykowanej szarej
    {
      zapis.write((char * ) & (tablica_kolorow[i][0]), sizeof(tablica_kolorow[i][0]));
    }
  }

  string temp = "";
  for (int i = 0; i < 64; i++) //zapis s³ownika
  {
    temp = slownik[i];
    zapis.write(temp.c_str(), temp.size());
    zapis.write("\0", sizeof(char));
  }

  for (int i = 0; i < height; i++) //zapis tablicy pikseli obrazu
  {
    for (int j = 0; j < width; j++) {
      temp = tablica_zakodowana[i][j];
      zapis.write(temp.c_str(), temp.size());
      zapis.write("\0", sizeof(char));
    }
  }
  zapis.close();

  ss.str("");
  ss.clear();
  ss << "zapiszDedykowana() Skonczone. " << endl;

  if (nr_palety == 0) {
    ss << " Wygenerowany plik: " << plik_graficzny_hama_paleta_narzucona_nazwa << endl;
  } else if (nr_palety == 1) {
    ss << " Wygenerowany plik: " << plik_graficzny_hama_paleta_dedykowana_nazwa << endl;
  } else if (nr_palety == 2) {
    ss << " Wygenerowany plik: " << plik_gr_hama_skala_szarosci_narzucona_nazwa << endl;
  } else if (nr_palety == 3) {
    ss << " Wygenerowany plik: " << plik_gr_hama_skala_szarosci_dedykowana_nazwa << endl;
  }

  ss << endl;
  ss << " rozmiar: " << rozmiar << endl;
  ss << " off_Naglowek: " << off_Naglowek << endl;
  ss << " off_PaletaKolorow: " << off_PaletaKolorow << endl;
  ss << " off_TablicaAlfabetu: " << off_TablicaAlfabetu << endl;
  ss << " off_TablicaPikseli: " << off_TablicaPikseli << endl;
  ss << " width: " << width << endl;
  ss << " height: " << height << endl;
  if (nr_palety == 0) {
    ss << " nr_palety: " << nr_palety << " - paleta narzucona" << endl;
  } else if (nr_palety == 1) {
    ss << " nr_palety: " << nr_palety << " - paleta dedykowana" << endl;
  } else if (nr_palety == 2) {
    ss << " nr_palety: " << nr_palety << " - paleta narzucona szara" << endl;
  } else if (nr_palety == 3) {
    ss << " nr_palety: " << nr_palety << " - paleta dedykowana szara" << endl;
  }
  Com += ss.str();
}

//----------------------------------------------------------------------

void f_genKolorowaPaleta();
void f_genMonoPaleta();
void f_genWarjacji(string kod, int dlugosc_kodu, string stany);
//----------------------------------------------------------------------
/* Funkcja odczytuj¹ca dane z pliku z rozszerzeniem .hama do tablic w programie */
int f_odczytajHAMA(string X, string & Com) {
  //nazwaPliku w zmiennej globalnej plik_graficzny_hama_nazwa
  string plik_hama_nazwa = "";
  stringstream ss;
  ss.str("");
  ss.clear();

  if (X == "paleta_narzucona_z_ditheringiem") {
    plik_hama_nazwa = plik_graficzny_hama_paleta_narzucona_nazwa;
  } else if (X == "paleta_dedykowana_z_ditheringiem") {
    plik_hama_nazwa = plik_graficzny_hama_paleta_dedykowana_nazwa;
  } else if (X == "skala_szarosci_narzucona_z_ditheringiem") {
    plik_hama_nazwa = plik_gr_hama_skala_szarosci_narzucona_nazwa;
  } else if (X == "skala_szarosci_dedykowana_z_ditheringiem") {
    plik_hama_nazwa = plik_gr_hama_skala_szarosci_dedykowana_nazwa;
  } else if (X == "inny") {
    plik_hama_nazwa = plik_graficzny_inny_hama_nazwa;
  } else {
    Com += "error: bledny argument odczytajHAMA() !";
    return 1;
  }

  ss << "plik_hama = " << plik_hama_nazwa << endl;

  ifstream odczyt;
  int rozmiar;
  int off_Naglowek;
  int off_PaletaKolorow;
  int off_TablicaAlfabetu;
  int off_TablicaPikseli;

  odczyt.open(plik_hama_nazwa.c_str(), ios::binary);
  odczyt.read((char * ) & (rozmiar), sizeof(rozmiar));
  odczyt.read((char * ) & (off_Naglowek), sizeof(off_Naglowek));
  odczyt.read((char * ) & (off_PaletaKolorow), sizeof(off_PaletaKolorow));
  odczyt.read((char * ) & (off_TablicaAlfabetu), sizeof(off_TablicaAlfabetu));
  odczyt.read((char * ) & (off_TablicaPikseli), sizeof(off_TablicaPikseli));
  odczyt.read((char * ) & (width), sizeof(width));
  odczyt.read((char * ) & (height), sizeof(height));
  odczyt.read((char * ) & (nr_palety), sizeof(nr_palety));

  ss << " Rozmiar:" << rozmiar << endl;
  ss << " off_Naglowek:" << off_Naglowek << endl;
  ss << " off_PaletaKolorow:" << off_PaletaKolorow << endl;
  ss << " off_TablicaAlfabetu:" << off_TablicaAlfabetu << endl;
  ss << " off_TablicaPikseli:" << off_TablicaPikseli << endl;
  ss << " width:" << width << endl;
  ss << " height:" << height << endl;
  //odczyt palet
  if (nr_palety == 0) {
    ss << " nr_palety: " << nr_palety << " - paleta narzucona" << endl;
    f_genKolorowaPaleta();
  } else if (nr_palety == 1) {
    ss << " nr_palety: " << nr_palety << " - paleta dedykowana" << endl;
    for (int i = 0; i < 64; i++) {
      odczyt.read((char * ) & (tablica_kolorow[i][0]), sizeof(tablica_kolorow[i][0]));
      odczyt.read((char * ) & (tablica_kolorow[i][1]), sizeof(tablica_kolorow[i][1]));
      odczyt.read((char * ) & (tablica_kolorow[i][2]), sizeof(tablica_kolorow[i][2]));
    }
  } else if (nr_palety == 2) {
    ss << " nr_palety: " << nr_palety << " - paleta narzucona szara" << endl;
    f_genMonoPaleta();
  } else if (nr_palety == 3) {
    ss << " nr_palety: " << nr_palety << " - paleta dedykowana szara" << endl;
    for (int i = 0; i < 64; i++) {
      odczyt.read((char * ) & (tablica_kolorow[i][0]), sizeof(tablica_kolorow[i][0]));
      tablica_kolorow[i][1] = tablica_kolorow[i][0];
      tablica_kolorow[i][2] = tablica_kolorow[i][0];
    }
  }

  for (int i = 0; i < 64; i++) //odczyt alfabetu
  {
    getline(odczyt, slownik[i], '\0');
  }

  if (X == "inny") {
    tablica_zakodowana = new string * [height];
    for (int i = 0; i < height; i++) {
      tablica_zakodowana[i] = new string[width];
      for (int j = 0; j < width; j++) {
        tablica_zakodowana[i][j] = "";
      }
    }
  }

  for (int i = 0; i < height; i++) //odczyt tablicy zakodowanej
  {
    for (int j = 0; j < width; j++) {
      getline(odczyt, tablica_zakodowana[i][j], '\0');
    }
  }

  odczyt.close();
  Com += ss.str();
  return 0;
}

/* Funkcja wypisuj¹ca dane z pliku z rozszerzeniem .hama do konsoli */
int f_wypiszHAMA(string X, string & Com) {
  string plik_hama_nazwa = "";
  stringstream ss;
  ss.str("");
  ss.clear();

  if (X == "paleta_narzucona_z_ditheringiem") {
    plik_hama_nazwa = plik_graficzny_hama_paleta_narzucona_nazwa;
  } else if (X == "paleta_dedykowana_z_ditheringiem") {
    plik_hama_nazwa = plik_graficzny_hama_paleta_dedykowana_nazwa;
  } else if (X == "skala_szarosci_narzucona_z_ditheringiem") {
    plik_hama_nazwa = plik_gr_hama_skala_szarosci_narzucona_nazwa;
  } else if (X == "skala_szarosci_dedykowana_z_ditheringiem") {
    plik_hama_nazwa = plik_gr_hama_skala_szarosci_dedykowana_nazwa;
  } else if (X == "inny") {
    plik_hama_nazwa = plik_graficzny_inny_hama_nazwa;
  } else {
    Com += "error: bledny argument odczytajHAMA() !";
    return 1;
  }

  ss << "plik_hama = " << plik_hama_nazwa << endl;

  ifstream odczyt;
  int rozmiar;
  int off_Naglowek;
  int off_PaletaKolorow;
  int off_TablicaAlfabetu;
  int off_TablicaPikseli;

  odczyt.open(plik_hama_nazwa.c_str(), ios::binary);
  odczyt.read((char * ) & (rozmiar), sizeof(rozmiar));
  odczyt.read((char * ) & (off_Naglowek), sizeof(off_Naglowek));
  odczyt.read((char * ) & (off_PaletaKolorow), sizeof(off_PaletaKolorow));
  odczyt.read((char * ) & (off_TablicaAlfabetu), sizeof(off_TablicaAlfabetu));
  odczyt.read((char * ) & (off_TablicaPikseli), sizeof(off_TablicaPikseli));
  odczyt.read((char * ) & (width), sizeof(width));
  odczyt.read((char * ) & (height), sizeof(height));
  odczyt.read((char * ) & (nr_palety), sizeof(nr_palety));

  ss << " Rozmiar:" << rozmiar << endl;
  ss << " off_Naglowek:" << off_Naglowek << endl;
  ss << " off_PaletaKolorow:" << off_PaletaKolorow << endl;
  ss << " off_TablicaAlfabetu:" << off_TablicaAlfabetu << endl;
  ss << " off_TablicaPikseli:" << off_TablicaPikseli << endl;
  ss << " width:" << width << endl;
  ss << " height:" << height << endl;
  //odczyt palet
  if (nr_palety == 0) {
    ss << " nr_palety: " << nr_palety << " - paleta narzucona" << endl;
    f_genKolorowaPaleta();
  } else if (nr_palety == 1) {
    ss << " nr_palety: " << nr_palety << " - paleta dedykowana" << endl;
    for (int i = 0; i < 64; i++) {
      odczyt.read((char * ) & (tablica_kolorow[i][0]), sizeof(tablica_kolorow[i][0]));
      odczyt.read((char * ) & (tablica_kolorow[i][1]), sizeof(tablica_kolorow[i][1]));
      odczyt.read((char * ) & (tablica_kolorow[i][2]), sizeof(tablica_kolorow[i][2]));
    }
  } else if (nr_palety == 2) {
    ss << " nr_palety: " << nr_palety << " - paleta narzucona szara" << endl;
    f_genMonoPaleta();
  } else if (nr_palety == 3) {
    ss << " nr_palety: " << nr_palety << " - paleta dedykowana szara" << endl;
    for (int i = 0; i < 64; i++) {
      odczyt.read((char * ) & (tablica_kolorow[i][0]), sizeof(tablica_kolorow[i][0]));
      tablica_kolorow[i][1] = tablica_kolorow[i][0];
      tablica_kolorow[i][2] = tablica_kolorow[i][0];
    }
  }

  ss << endl << " paleta: " << endl; //wypisanie palety
  ss << "nr R  G  B" << endl;
  for (int i = 0; i < 64; i++) {
    ss << i << " ";
    ss << tablica_kolorow[i][0] << " ";
    if (nr_palety == 0 || nr_palety == 1) {
      ss << " " << tablica_kolorow[i][1] << " ";
      ss << " " << tablica_kolorow[i][2] << " ";
    }
    ss << endl;
  }

  for (int i = 0; i < 64; i++) //odczyt alfabetu
  {
    getline(odczyt, slownik[i], '\0');
  }
  ss << endl << " Alfabet: " << endl; //wypisanie alfabetu
  for (int i = 0; i < 64; i++) {
    ss << " " << i << " " << slownik[i] << endl;
  }

  for (int i = 0; i < height; i++) //odczyt tablicy zakodowanej
  {
    for (int j = 0; j < width; j++) {
      getline(odczyt, tablica_zakodowana[i][j], '\0');
    }
  }
  if (rozmiar < 3000) {
    ss << endl << " Tablica zakodowanych pikseli: " << endl; //wypisanie tablicy zakodowanej
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        ss << " " << tablica_zakodowana[i][j] << " ";
      }
      ss << endl;
    }
  }
  odczyt.close();
  Com += ss.str();
  return 0;
}

//----------------------------------------------------------------------
struct podprzestrzen {
  public: Uint8 Rd;
  Uint8 Gd;
  Uint8 Bd;
  Uint8 Rg;
  Uint8 Gg;
  Uint8 Bg;
  short poziom;
  podprzestrzen * lewy;
  podprzestrzen * prawy;

  void podzielDo64(int ** * kolory) {
    if (poziom < 6) {
      int Rmin = 255, Rmax = 0;
      int Gmin = 255, Gmax = 0;
      int Bmin = 255, Bmax = 0;

      int Rrange, Grange, Brange;
      Rrange = Rg - Rd;
      Grange = Gg - Gd;
      Brange = Bg - Bd;

      if (max(max(Rrange, Grange), Brange) == Rrange) {

        int * iloscWskladowej = new int[Rg - Rd + 1];
        for (int i = 0; i < Rg - Rd + 1; ++i) {
          iloscWskladowej[i] = 0;
        }

        int iloscKolorow = 0;
        for (int i = Rd; i <= Rg; ++i) {
          for (int j = Gd; j <= Gg; ++j) {
            for (int k = Bd; k <= Bg; ++k) {
              iloscWskladowej[i - Rd] += kolory[i][j][k];
            }
          }
        }

        for (int i = 0; i < Rg - Rd + 1; ++i) {
          iloscKolorow += iloscWskladowej[i];
        }

        unsigned long long int srednia = iloscKolorow / 2;
        iloscKolorow = 0;
        int podzial;
        int i = 0;
        while (iloscKolorow < srednia) {
          iloscKolorow += iloscWskladowej[i];
          ++i;
        }
        podzial = --i;

        //Gmin Gmax
        for (int i = Rd; i <= podzial + Rd; ++i) {
          for (int j = Gd; j <= Gg; ++j) {
            for (int k = Bd; k <= Bg; ++k) {
              if (kolory[i][j][k] > 0 && j < Gmin) {
                Gmin = j;
              }
              if (kolory[i][j][k] > 0 && j > Gmax) {
                Gmax = j;
              }
            }
          }
        }

        //Bmin Bmax
        for (int i = Rd; i <= podzial + Rd; ++i) {
          for (int j = Gd; j <= Gg; ++j) {
            for (int k = Bd; k <= Bg; ++k) {
              if (kolory[i][j][k] > 0 && k < Bmin) {
                Bmin = k;
              }
              if (kolory[i][j][k] > 0 && k > Bmax) {
                Bmax = k;
              }
            }
          }
        }

        lewy = new podprzestrzen;
        lewy -> Rd = Rd;
        lewy -> Gd = Gmin;
        lewy -> Bd = Bmin;
        lewy -> Rg = podzial + Rd;
        lewy -> Gg = Gmax;
        lewy -> Bg = Bmax;
        lewy -> poziom = poziom + 1;
        lewy -> lewy = NULL;
        lewy -> prawy = NULL;

        //Gmin Gmax
        for (int i = podzial + Rd + 1; i <= Rg; ++i) {
          for (int j = Gd; j <= Gg; ++j) {
            for (int k = Bd; k <= Bg; ++k) {
              if (kolory[i][j][k] > 0 && j < Gmin) {
                Gmin = j;
              }
              if (kolory[i][j][k] > 0 && j > Gmax) {
                Gmax = j;
              }
            }
          }
        }

        //Bmin Bmax
        for (int i = podzial + Rd + 1; i <= Rg; ++i) {
          for (int j = Gd; j <= Gg; ++j) {
            for (int k = Bd; k <= Bg; ++k) {
              if (kolory[i][j][k] > 0 && k < Bmin) {
                Bmin = k;
              }
              if (kolory[i][j][k] > 0 && k > Bmax) {
                Bmax = k;
              }
            }
          }
        }

        prawy = new podprzestrzen;
        prawy -> Rd = podzial + Rd + 1;
        prawy -> Gd = Gmin;
        prawy -> Bd = Bmin;
        prawy -> Rg = Rg;
        prawy -> Gg = Gmax;
        prawy -> Bg = Bmax;
        prawy -> poziom = poziom + 1;
        prawy -> lewy = NULL;
        prawy -> prawy = NULL;
      } else if (max(max(Rrange, Grange), Brange) == Grange) {
        int * iloscWskladowej = new int[Gg - Gd + 1];
        for (int i = 0; i < Gg - Gd + 1; ++i) {
          iloscWskladowej[i] = 0;
        }

        int iloscKolorow = 0;
        for (int i = Rd; i <= Rg; ++i) {
          for (int j = Gd; j <= Gg; ++j) {
            for (int k = Bd; k <= Bg; ++k) {
              iloscWskladowej[j - Gd] += kolory[i][j][k];
            }
          }
        }

        for (int i = 0; i < Gg - Gd + 1; ++i) {
          iloscKolorow += iloscWskladowej[i];
        }

        int srednia = iloscKolorow / 2;
        iloscKolorow = 0;
        int podzial;
        int i = 0;
        while (iloscKolorow < srednia) {
          iloscKolorow += iloscWskladowej[i];
          ++i;
        }
        podzial = --i;

        //Rmin Rmax
        for (int i = Rd; i <= Rg; ++i) {
          for (int j = Gd; j <= podzial + Gd; ++j) {
            for (int k = Bd; k <= Bg; ++k) {
              if (kolory[i][j][k] > 0 && i < Rmin) {
                Rmin = i;
              }
              if (kolory[i][j][k] > 0 && i > Rmax) {
                Rmax = i;
              }
            }
          }
        }

        //Bmin Bmax
        for (int i = Rd; i <= Rg; ++i) {
          for (int j = Gd; j <= podzial + Gd; ++j) {
            for (int k = Bd; k <= Bg; ++k) {
              if (kolory[i][j][k] > 0 && k < Bmin) {
                Bmin = k;
              }
              if (kolory[i][j][k] > 0 && k > Bmax) {
                Bmax = k;
              }
            }
          }
        }

        lewy = new podprzestrzen;
        lewy -> Rd = Rmin;
        lewy -> Gd = Gd;
        lewy -> Bd = Bmin;
        lewy -> Rg = Rmax;
        lewy -> Gg = podzial + Gd;
        lewy -> Bg = Bmax;
        lewy -> poziom = poziom + 1;
        lewy -> lewy = NULL;
        lewy -> prawy = NULL;

        //Rmin Rmax
        for (int i = Rd; i <= Rg; ++i) {
          for (int j = podzial + Gd + 1; j <= Gg; ++j) {
            for (int k = Bd; k <= Bg; ++k) {
              if (kolory[i][j][k] > 0 && i < Rmin) {
                Rmin = i;
              }
              if (kolory[i][j][k] > 0 && i > Rmax) {
                Rmax = i;
              }
            }
          }
        }

        //Bmin Bmax
        for (int i = Rd; i <= Rg; ++i) {
          for (int j = podzial + Gd + 1; j <= Gg; ++j) {
            for (int k = Bd; k <= Bg; ++k) {
              if (kolory[i][j][k] > 0 && k < Bmin) {
                Bmin = k;
              }
              if (kolory[i][j][k] > 0 && k > Bmax) {
                Bmax = k;
              }
            }
          }
        }

        prawy = new podprzestrzen;
        prawy -> Rd = Rmin;
        prawy -> Gd = podzial + Gd + 1;
        prawy -> Bd = Bmin;
        prawy -> Rg = Rmax;
        prawy -> Gg = Gg;
        prawy -> Bg = Bmax;
        prawy -> poziom = poziom + 1;
        prawy -> lewy = NULL;
        prawy -> prawy = NULL;
      } else {

        int * iloscWskladowej = new int[Bg - Bd + 1];
        for (int i = 0; i < Bg - Bd + 1; ++i) {
          iloscWskladowej[i] = 0;
        }

        int iloscKolorow = 0;
        for (int i = Rd; i <= Rg; ++i) {
          for (int j = Gd; j <= Gg; ++j) {
            for (int k = Bd; k <= Bg; ++k) {
              iloscWskladowej[k - Bd] += kolory[i][j][k];
              iloscKolorow += kolory[i][j][k];
            }
          }
        }

        int srednia = iloscKolorow / 2;
        iloscKolorow = 0;
        int podzial;
        int i = 0;
        while (iloscKolorow < srednia) {
          iloscKolorow += iloscWskladowej[i];
          ++i;
        }
        podzial = --i;

        //Rmin Rmax
        for (int i = Rd; i <= Rg; ++i) {
          for (int j = Gd; j <= Gg; ++j) {
            for (int k = Bd; k <= podzial + Bd; ++k) {
              if (kolory[i][j][k] > 0 && i < Rmin) {
                Rmin = i;
              }
              if (kolory[i][j][k] > 0 && i > Rmax) {
                Rmax = i;
              }
            }
          }
        }

        //Gmin Gmax
        for (int i = Rd; i <= Rg; ++i) {
          for (int j = Gd; j <= Gg; ++j) {
            for (int k = Bd; k <= podzial + Bd; ++k) {
              if (kolory[i][j][k] > 0 && j < Gmin) {
                Gmin = j;
              }
              if (kolory[i][j][k] > 0 && j > Gmax) {
                Gmax = j;
              }
            }
          }
        }

        lewy = new podprzestrzen;
        lewy -> Rd = Rmin;
        lewy -> Gd = Gmin;
        lewy -> Bd = Bd;
        lewy -> Rg = Rmax;
        lewy -> Gg = Gmax;
        lewy -> Bg = podzial + Bd;
        lewy -> poziom = poziom + 1;
        lewy -> lewy = NULL;
        lewy -> prawy = NULL;

        //Rmin Rmax
        for (int i = Rd; i <= Rg; ++i) {
          for (int j = Gd; j <= Gg; ++j) {
            for (int k = podzial + Bd + 1; k <= Bg; ++k) {
              if (kolory[i][j][k] > 0 && i < Rmin) {
                Rmin = i;
              }
              if (kolory[i][j][k] > 0 && i > Rmax) {
                Rmax = i;
              }
            }
          }
        }

        //Gmin Gmax
        for (int i = Rd; i <= Rg; ++i) {
          for (int j = Gd; j <= Gg; ++j) {
            for (int k = podzial + Bd + 1; k <= Bg; ++k) {
              if (kolory[i][j][k] > 0 && j < Gmin) {
                Gmin = j;
              }
              if (kolory[i][j][k] > 0 && j > Gmax) {
                Gmax = j;
              }
            }
          }
        }

        prawy = new podprzestrzen;
        prawy -> Rd = Rmin;
        prawy -> Gd = Gmin;
        prawy -> Bd = podzial + Bd + 1;
        prawy -> Rg = Rmax;
        prawy -> Gg = Gmax;
        prawy -> Bg = Bg;
        prawy -> poziom = poziom + 1;
        prawy -> lewy = NULL;
        prawy -> prawy = NULL;
      }

      lewy -> podzielDo64(kolory);
      prawy -> podzielDo64(kolory);

    }
  }

  void stworzPalete(int & ind, int ** * kolory) {
    unsigned long long int RR = 0;
    unsigned long long int GG = 0;
    unsigned long long int BB = 0;
    int temp;
    unsigned long long int wszystkie = 0;
    if (poziom == 6) {
      for (int i = Rd; i <= Rg; ++i) {
        for (int j = Gd; j <= Gg; ++j) {
          for (int k = Bd; k <= Bg; ++k) {
            temp = kolory[i][j][k];
            RR += temp * i;
            GG += temp * j;
            BB += temp * k;
            wszystkie += temp;
          }
        }
      }
      if (wszystkie == 0) {
        tablica_kolorow[ind][0] = 0;
        tablica_kolorow[ind][1] = 0;
        tablica_kolorow[ind][2] = 0;
      } else {
        tablica_kolorow[ind][0] = RR / wszystkie;
        tablica_kolorow[ind][1] = GG / wszystkie;
        tablica_kolorow[ind][2] = BB / wszystkie;
      }
      ind++;
    } else {
      lewy -> stworzPalete(ind, kolory);
      prawy -> stworzPalete(ind, kolory);
    }
  }

};

//----------------------------------------------------------------------
void f_genDedykowanaPaletaKolor() {
  int ** * kolory = new int ** [256];

  for (int i = 0; i < 256; ++i) {
    kolory[i] = new int * [256];
    for (int j = 0; j < 256; ++j) {
      kolory[i][j] = new int[256];
      for (int k = 0; k < 256; ++k) {
        kolory[i][j][k] = 0;
      }
    }
  }

  Uint8 R, G, B;
  Uint8 Rmin = 255, Rmax = 0, Gmin = 255, Gmax = 0, Bmin = 255, Bmax = 0;
  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      R = tab_pixeli[i][j].r;
      G = tab_pixeli[i][j].g;
      B = tab_pixeli[i][j].b;
      kolory[R][G][B] += 1;
      if (R > Rmax)
        Rmax = R;
      if (R < Rmin)
        Rmin = R;
      if (G > Gmax)
        Gmax = G;
      if (G < Gmin)
        Gmin = G;
      if (B > Bmax)
        Bmax = B;
      if (B < Bmin)
        Bmin = B;
    }
  }

  podprzestrzen * korzen = new podprzestrzen;
  korzen -> Rd = Rmin;
  korzen -> Gd = Gmin;
  korzen -> Bd = Bmin;
  korzen -> Rg = Rmax;
  korzen -> Gg = Gmax;
  korzen -> Bg = Bmax;
  korzen -> poziom = 0;
  korzen -> lewy = NULL;
  korzen -> prawy = NULL;
  korzen -> podzielDo64(kolory);;
  int ind = 0; //indeks tablicy_kolorów palety dedykowanej
  korzen -> stworzPalete(ind, kolory);
}
