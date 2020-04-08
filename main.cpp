#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif
#ifdef __APPLE__
    #include <SDL/SDL.h>
#else
    #include <SDL.h>
#endif
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

SDL_Surface *screen;
SDL_Surface *plik_graficzny = NULL;
SDL_Surface *plik_graficzny_po_konwersji = NULL;

// globalne wymiary obrazka wyliczone przy wczytywaniu
int width;
int height;

char const* tytul = "projekt grafika";

// nazwy plikow
string plik_graficzny_nazwa="";
string plik_graficzny_inny_hama_nazwa="";
string plik_graficzny_hama_paleta_narzucona_nazwa="";
string plik_graficzny_hama_paleta_dedykowana_nazwa="";
//string plik_graficzny_hama_skala_szarosci_nazwa="";
string plik_gr_hama_skala_szarosci_narzucona_nazwa="";
string plik_gr_hama_skala_szarosci_dedykowana_nazwa="";


void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B);
SDL_Color getPixel (int x, int y);

/* Tablica przechowywujaca pixele wygenerowane przez odpowiednia funkcje ograniczajaca kolory
Jest to tablica dynamiczna, inicjowana w momencie wywolania ktoregos z algorytmow generowania
kolorow. Zapisywane sa w niej wyniki ograniczania koloru z ditheringiem. Rozmiar tablicy to
[tab_pixeli_w][tab_pixeli_h]!! */
int tab_pixeli_w=0;
int tab_pixeli_h=0;
SDL_Color **tab_pixeli;
SDL_Color **tab_pixeli_wyjsciowych;


int tablica_kolorow[64][3];
stack <string> stos;
int **tablica_wskaznikow_palety;

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
string **tablica_zakodowana; //TO WYCHODZI Z HUFFA I WCHODZI DO PLIKU
string slownik[64]; //TO TRZEBA WPISAĆ DO PLIKU RAZEM Z TABLICA_ZAKODOWANA BO JEST POTRZEBNE DO ODCZYTU
int **tablica_rozkodowana;//TU TRAFIAJA RZECZY ODKODOWYWANE Z HUFFA

void zapiszDedykowana();

/**********************************************************************/
/*  Marzena Pepera - generowanie tablicy barw dedykowanej kolorowej oraz
    czarno bialej, zapis i odczyt pliku .hama */
/**********************************************************************/
/*Funkcja zerująca tablice z danymi w celu uniknięcia kolizji*/
void f_zerowanie_tablic()
{
    for (int i = 0; i < height; i++){
        for(int j=0;j<width; j++){
            tablica_zakodowana[i][j] = "";
        }
    }
    for(int i=0;i<64;i++)
        slownik[i]="";
    for (int i = 0; i < 64; i++)
        for(int j=0;j<3; j++){
            tablica_kolorow[i][j] = 0;
        }
}

//----------------------------------------------------------------------
/* Funkcja zapisująca dane obrazu do pliku z rozszerzeniem .hama */
void f_zapisDoHAMA(string &Com)
{

    ofstream zapis;
    stringstream ss;         //ss - zmienna do tworzenia stringow, komentarzy
    ss.str(""); ss.clear();
    int rozmiar=20+12+64+(width*height+sizeof(string));
    int off_Naglowek = 20;
    int off_PaletaKolorow = -1;
    int off_TablicaAlfabetu = 20+12;
    int off_TablicaPikseli = 20+12+64;

    if(nr_palety==0)
    {
        zapis.open(plik_graficzny_hama_paleta_narzucona_nazwa.c_str(), ios::binary);
    }
    else if(nr_palety==1)
    {
        rozmiar+=64*3;
        off_PaletaKolorow=20+12;
        off_TablicaAlfabetu+=64*3;
        off_TablicaPikseli+=64*3;
        zapis.open(plik_graficzny_hama_paleta_dedykowana_nazwa.c_str(), ios::binary);
    }
    else if(nr_palety==2)
    {
        //zapis.open(plik_graficzny_hama_skala_szarosci_nazwa.c_str(), ios::binary);
        zapis.open(plik_gr_hama_skala_szarosci_narzucona_nazwa.c_str(), ios::binary);
    }
    else if(nr_palety==3)
    {
        rozmiar+=64;
        off_PaletaKolorow=20+12;
        off_TablicaAlfabetu+=64;
        off_TablicaPikseli+=64;
        //zapis.open(plik_graficzny_hama_skala_szarosci_nazwa.c_str(), ios::binary);
        zapis.open(plik_gr_hama_skala_szarosci_dedykowana_nazwa.c_str(), ios::binary);
    }
    else {ss << "Blad - brak palety"; Com+=ss.str();}
    zapis.write((char*)&(rozmiar), sizeof(rozmiar));
    zapis.write((char*)&(off_Naglowek), sizeof(off_Naglowek));
    zapis.write((char*)&(off_PaletaKolorow), sizeof(off_PaletaKolorow));
    zapis.write((char*)&(off_TablicaAlfabetu), sizeof(off_TablicaAlfabetu));
    zapis.write((char*)&(off_TablicaPikseli), sizeof(off_TablicaPikseli));
    zapis.write((char*)&(width), sizeof(width));
    zapis.write((char*)&(height), sizeof(height));
    zapis.write((char*)&(nr_palety), sizeof(nr_palety));

    if(nr_palety==1)
    {
        for(int i=0;i<64;i++) //zapis palety dedykowanej kolorowej
        {
            zapis.write((char*)&(tablica_kolorow[i][0]), sizeof(tablica_kolorow[i][0]));
            zapis.write((char*)&(tablica_kolorow[i][1]), sizeof(tablica_kolorow[i][1]));
            zapis.write((char*)&(tablica_kolorow[i][2]), sizeof(tablica_kolorow[i][2]));
        }
    }
    else if(nr_palety==3)
    {
        for(int i=0;i<64;i++) //zapis palety dedykowanej szarej
        {
            zapis.write((char*)&(tablica_kolorow[i][0]), sizeof(tablica_kolorow[i][0]));
        }
    }


    string temp="";
    for(int i=0;i<64;i++) //zapis słownika
    {
        temp = slownik[i];
        zapis.write(temp.c_str(),temp.size());
        zapis.write("\0",sizeof(char));
    }

    for(int i=0;i<height;i++) //zapis tablicy pikseli obrazu
    {
        for(int j=0;j<width;j++)
            {
                temp = tablica_zakodowana[i][j];
                zapis.write(temp.c_str(),temp.size());
                zapis.write("\0",sizeof(char));
            }
    }
    zapis.close();

    ss.str(""); ss.clear();
    ss << "zapiszDedykowana() Skonczone. "<<endl;

    if(nr_palety==0)
    {
        ss << " Wygenerowany plik: " << plik_graficzny_hama_paleta_narzucona_nazwa << endl;
    }
    else if(nr_palety==1)
    {
        ss << " Wygenerowany plik: " << plik_graficzny_hama_paleta_dedykowana_nazwa << endl;
    }
    else if(nr_palety==2)
    {
        ss << " Wygenerowany plik: " << plik_gr_hama_skala_szarosci_narzucona_nazwa << endl;
    }
    else if(nr_palety==3)
    {
        ss << " Wygenerowany plik: " << plik_gr_hama_skala_szarosci_dedykowana_nazwa << endl;
    }

    ss << endl;
    ss << " rozmiar: "<<rozmiar<<endl;
    ss << " off_Naglowek: " << off_Naglowek << endl;
    ss << " off_PaletaKolorow: " << off_PaletaKolorow << endl;
    ss << " off_TablicaAlfabetu: " << off_TablicaAlfabetu << endl;
    ss << " off_TablicaPikseli: " << off_TablicaPikseli << endl;
    ss << " width: " << width << endl;
    ss << " height: " << height << endl;
    if(nr_palety==0)
    {
        ss << " nr_palety: " << nr_palety << " - paleta narzucona" << endl;
    }
    else if(nr_palety==1)
    {
        ss << " nr_palety: " << nr_palety << " - paleta dedykowana" << endl;
    }
    else if(nr_palety==2)
    {
        ss << " nr_palety: " << nr_palety << " - paleta narzucona szara" << endl;
    }
    else if(nr_palety==3)
    {
        ss << " nr_palety: " << nr_palety << " - paleta dedykowana szara" << endl;
    }
    Com+=ss.str();
}

//----------------------------------------------------------------------

void f_genKolorowaPaleta();
void f_genMonoPaleta();
void f_genWarjacji(string kod, int dlugosc_kodu, string stany);
//----------------------------------------------------------------------
/* Funkcja odczytująca dane z pliku z rozszerzeniem .hama do tablic w programie */
int f_odczytajHAMA(string X, string &Com){
     //nazwaPliku w zmiennej globalnej plik_graficzny_hama_nazwa
    string plik_hama_nazwa="";
    stringstream ss;
    ss.str(""); ss.clear();

         if(X == "paleta_narzucona_z_ditheringiem")         { plik_hama_nazwa = plik_graficzny_hama_paleta_narzucona_nazwa; }
    else if(X == "paleta_dedykowana_z_ditheringiem")        { plik_hama_nazwa = plik_graficzny_hama_paleta_dedykowana_nazwa; }
    else if(X == "skala_szarosci_narzucona_z_ditheringiem") { plik_hama_nazwa = plik_gr_hama_skala_szarosci_narzucona_nazwa; }
    else if(X == "skala_szarosci_dedykowana_z_ditheringiem"){ plik_hama_nazwa = plik_gr_hama_skala_szarosci_dedykowana_nazwa; }
    else if(X == "inny")                                    { plik_hama_nazwa = plik_graficzny_inny_hama_nazwa; }
    else { Com+="error: bledny argument odczytajHAMA() !"; return 1; }

    ss<<"plik_hama = " << plik_hama_nazwa << endl;

    ifstream odczyt;
    int rozmiar;
    int off_Naglowek;
    int off_PaletaKolorow;
    int off_TablicaAlfabetu;
    int off_TablicaPikseli;

    odczyt.open(plik_hama_nazwa.c_str(), ios::binary);
	odczyt.read((char*)&(rozmiar), sizeof(rozmiar));
    odczyt.read((char*)&(off_Naglowek), sizeof(off_Naglowek));
    odczyt.read((char*)&(off_PaletaKolorow), sizeof(off_PaletaKolorow));
    odczyt.read((char*)&(off_TablicaAlfabetu), sizeof(off_TablicaAlfabetu));
    odczyt.read((char*)&(off_TablicaPikseli), sizeof(off_TablicaPikseli));
    odczyt.read((char*)&(width), sizeof(width));
    odczyt.read((char*)&(height), sizeof(height));
    odczyt.read((char*)&(nr_palety), sizeof(nr_palety));

	ss << " Rozmiar:" << rozmiar << endl;
    ss << " off_Naglowek:" << off_Naglowek << endl;
    ss << " off_PaletaKolorow:" << off_PaletaKolorow << endl;
    ss << " off_TablicaAlfabetu:" << off_TablicaAlfabetu << endl;
    ss << " off_TablicaPikseli:" << off_TablicaPikseli << endl;
    ss << " width:" << width << endl;
    ss << " height:" << height << endl;
    //odczyt palet
    if(nr_palety==0)
    {
        ss << " nr_palety: " << nr_palety << " - paleta narzucona" << endl;
        f_genKolorowaPaleta();
    }
    else if(nr_palety==1)
    {
        ss << " nr_palety: " << nr_palety << " - paleta dedykowana" << endl;
        for(int i=0;i<64;i++)
        {
            odczyt.read((char*)&(tablica_kolorow[i][0]), sizeof(tablica_kolorow[i][0]));
            odczyt.read((char*)&(tablica_kolorow[i][1]), sizeof(tablica_kolorow[i][1]));
            odczyt.read((char*)&(tablica_kolorow[i][2]), sizeof(tablica_kolorow[i][2]));
        }
    }
    else if(nr_palety==2)
    {
        ss << " nr_palety: " << nr_palety << " - paleta narzucona szara" << endl;
        f_genMonoPaleta();
    }
    else if(nr_palety==3)
    {
        ss << " nr_palety: " << nr_palety << " - paleta dedykowana szara" << endl;
        for(int i=0;i<64;i++)
        {
            odczyt.read((char*)&(tablica_kolorow[i][0]), sizeof(tablica_kolorow[i][0]));
            tablica_kolorow[i][1]=tablica_kolorow[i][0];
            tablica_kolorow[i][2]=tablica_kolorow[i][0];
        }
    }

    for(int i=0;i<64;i++) //odczyt alfabetu
    {
        getline(odczyt,slownik[i],'\0');
    }

    if(X == "inny") {
        tablica_zakodowana = new string *[height];
        for (int i = 0; i < height; i++){
            tablica_zakodowana[i] = new string[width];
            for(int j=0;j<width; j++){
                tablica_zakodowana[i][j] = "";
            }
        }
    }

    for(int i=0;i<height;i++) //odczyt tablicy zakodowanej
    {
        for(int j=0;j<width;j++)
            {
                getline(odczyt,tablica_zakodowana[i][j],'\0');
            }
    }

    odczyt.close();
    Com+=ss.str();
    return 0;
}

/* Funkcja wypisująca dane z pliku z rozszerzeniem .hama do konsoli */
int f_wypiszHAMA(string X, string &Com){
    string plik_hama_nazwa="";
    stringstream ss;
    ss.str(""); ss.clear();

         if(X == "paleta_narzucona_z_ditheringiem")         { plik_hama_nazwa = plik_graficzny_hama_paleta_narzucona_nazwa; }
    else if(X == "paleta_dedykowana_z_ditheringiem")        { plik_hama_nazwa = plik_graficzny_hama_paleta_dedykowana_nazwa; }
    else if(X == "skala_szarosci_narzucona_z_ditheringiem") { plik_hama_nazwa = plik_gr_hama_skala_szarosci_narzucona_nazwa; }
    else if(X == "skala_szarosci_dedykowana_z_ditheringiem"){ plik_hama_nazwa = plik_gr_hama_skala_szarosci_dedykowana_nazwa; }
    else if(X == "inny")                                    { plik_hama_nazwa = plik_graficzny_inny_hama_nazwa; }
    else { Com+="error: bledny argument odczytajHAMA() !"; return 1; }

    ss<<"plik_hama = " << plik_hama_nazwa << endl;

    ifstream odczyt;
    int rozmiar;
    int off_Naglowek;
    int off_PaletaKolorow;
    int off_TablicaAlfabetu;
    int off_TablicaPikseli;

    odczyt.open(plik_hama_nazwa.c_str(), ios::binary);
	odczyt.read((char*)&(rozmiar), sizeof(rozmiar));
    odczyt.read((char*)&(off_Naglowek), sizeof(off_Naglowek));
    odczyt.read((char*)&(off_PaletaKolorow), sizeof(off_PaletaKolorow));
    odczyt.read((char*)&(off_TablicaAlfabetu), sizeof(off_TablicaAlfabetu));
    odczyt.read((char*)&(off_TablicaPikseli), sizeof(off_TablicaPikseli));
    odczyt.read((char*)&(width), sizeof(width));
    odczyt.read((char*)&(height), sizeof(height));
    odczyt.read((char*)&(nr_palety), sizeof(nr_palety));

	ss << " Rozmiar:" << rozmiar << endl;
    ss << " off_Naglowek:" << off_Naglowek << endl;
    ss << " off_PaletaKolorow:" << off_PaletaKolorow << endl;
    ss << " off_TablicaAlfabetu:" << off_TablicaAlfabetu << endl;
    ss << " off_TablicaPikseli:" << off_TablicaPikseli << endl;
    ss << " width:" << width << endl;
    ss << " height:" << height << endl;
    //odczyt palet
    if(nr_palety==0)
    {
        ss << " nr_palety: " << nr_palety << " - paleta narzucona" << endl;
        f_genKolorowaPaleta();
    }
    else if(nr_palety==1)
    {
        ss << " nr_palety: " << nr_palety << " - paleta dedykowana" << endl;
        for(int i=0;i<64;i++)
        {
            odczyt.read((char*)&(tablica_kolorow[i][0]), sizeof(tablica_kolorow[i][0]));
            odczyt.read((char*)&(tablica_kolorow[i][1]), sizeof(tablica_kolorow[i][1]));
            odczyt.read((char*)&(tablica_kolorow[i][2]), sizeof(tablica_kolorow[i][2]));
        }
    }
    else if(nr_palety==2)
    {
        ss << " nr_palety: " << nr_palety << " - paleta narzucona szara" << endl;
        f_genMonoPaleta();
    }
    else if(nr_palety==3)
    {
        ss << " nr_palety: " << nr_palety << " - paleta dedykowana szara" << endl;
        for(int i=0;i<64;i++)
        {
            odczyt.read((char*)&(tablica_kolorow[i][0]), sizeof(tablica_kolorow[i][0]));
            tablica_kolorow[i][1]=tablica_kolorow[i][0];
            tablica_kolorow[i][2]=tablica_kolorow[i][0];
        }
    }

    ss<<endl<<" paleta: "<<endl; //wypisanie palety
    ss<<"nr R  G  B"<<endl;
    for(int i=0;i<64;i++)
    {
        ss<<i<<" ";
        ss<<tablica_kolorow[i][0]<<" ";
        if(nr_palety == 0 || nr_palety == 1)
        {
            ss<<" " << tablica_kolorow[i][1]<<" ";
            ss<<" " <<tablica_kolorow[i][2]<<" ";
        }
        ss<<endl;
    }

    for(int i=0;i<64;i++) //odczyt alfabetu
    {
        getline(odczyt,slownik[i],'\0');
    }
    ss<<endl<<" Alfabet: "<<endl; //wypisanie alfabetu
    for(int i=0;i<64;i++)
    {
        ss<<" " <<i<<" "<<slownik[i]<<endl;
    }

    for(int i=0;i<height;i++) //odczyt tablicy zakodowanej
    {
        for(int j=0;j<width;j++)
            {
                getline(odczyt,tablica_zakodowana[i][j],'\0');
            }
    }
    if(rozmiar<3000)
    {
        ss<<endl<<" Tablica zakodowanych pikseli: "<<endl; //wypisanie tablicy zakodowanej
        for(int i=0;i<height;i++)
        {
            for(int j=0;j<width;j++)
                {
                    ss<<" " <<tablica_zakodowana[i][j]<<" ";
                }
            ss<<endl;
        }
    }
    odczyt.close();
    Com+=ss.str();
    return 0;
}

//----------------------------------------------------------------------
struct podprzestrzen{
public:
    Uint8 Rd;
    Uint8 Gd;
    Uint8 Bd;
    Uint8 Rg;
    Uint8 Gg;
    Uint8 Bg;
    short poziom;
    podprzestrzen* lewy;
    podprzestrzen* prawy;

    void podzielDo64(int*** kolory){
        if(poziom < 6){
            int Rmin = 255, Rmax = 0;
            int Gmin = 255, Gmax = 0;
            int Bmin = 255, Bmax = 0;

            int Rrange, Grange, Brange;
            Rrange = Rg - Rd;
            Grange = Gg - Gd;
            Brange = Bg - Bd;

            if(max(max(Rrange,Grange),Brange) == Rrange){

                int* iloscWskladowej = new int[Rg - Rd +1];
                for(int i=0; i<Rg - Rd +1; ++i){
                    iloscWskladowej[i] = 0;
                }

                int iloscKolorow = 0;
                for(int i=Rd; i<=Rg; ++i){
                    for(int j=Gd; j<=Gg; ++j){
                        for(int k=Bd; k<=Bg; ++k){
                                iloscWskladowej[i-Rd] += kolory[i][j][k];
                        }
                    }
                }

                for(int i=0; i<Rg - Rd +1; ++i){
                    iloscKolorow += iloscWskladowej[i];
                }

                unsigned long long int srednia = iloscKolorow/2;
                iloscKolorow = 0;
                int podzial;
                int i=0;
                while(iloscKolorow<srednia){
                    iloscKolorow += iloscWskladowej[i];
                    ++i;
                }
                podzial = --i;

                //Gmin Gmax
                for(int i=Rd; i<=podzial + Rd; ++i){
                    for(int j=Gd; j<=Gg; ++j){
                        for(int k=Bd; k<=Bg; ++k){
                            if(kolory[i][j][k]> 0 && j < Gmin){
                                Gmin = j;
                            }
                            if(kolory[i][j][k]> 0 && j > Gmax){
                                Gmax = j;
                            }
                        }
                    }
                }

                //Bmin Bmax
                for(int i=Rd; i<=podzial + Rd; ++i){
                    for(int j=Gd; j<=Gg; ++j){
                        for(int k=Bd; k<=Bg; ++k){
                            if(kolory[i][j][k]  > 0 && k < Bmin){
                                Bmin = k;
                            }
                            if(kolory[i][j][k] > 0 && k > Bmax){
                                Bmax = k;
                            }
                        }
                    }
                }

                lewy = new podprzestrzen;
                lewy->Rd = Rd;
                lewy->Gd = Gmin;
                lewy->Bd = Bmin;
                lewy->Rg = podzial+Rd;
                lewy->Gg = Gmax;
                lewy->Bg = Bmax;
                lewy->poziom = poziom+1;
                lewy->lewy = NULL;
                lewy->prawy = NULL;


                //Gmin Gmax
                for(int i=podzial+Rd+1; i<=Rg; ++i){
                    for(int j=Gd; j<=Gg; ++j){
                        for(int k=Bd; k<=Bg; ++k){
                            if(kolory[i][j][k] > 0 && j < Gmin){
                                Gmin = j;
                            }
                            if(kolory[i][j][k]> 0 && j > Gmax){
                                Gmax = j;
                            }
                        }
                    }
                }

                //Bmin Bmax
                for(int i=podzial+Rd+1; i<=Rg; ++i){
                    for(int j=Gd; j<=Gg; ++j){
                        for(int k=Bd; k<=Bg; ++k){
                            if(kolory[i][j][k]  > 0 && k < Bmin){
                                Bmin = k;
                            }
                            if(kolory[i][j][k] > 0 && k > Bmax){
                                Bmax = k;
                            }
                        }
                    }
                }

                prawy = new podprzestrzen;
                prawy->Rd = podzial+Rd+1;
                prawy->Gd = Gmin;
                prawy->Bd = Bmin;
                prawy->Rg = Rg;
                prawy->Gg = Gmax;
                prawy->Bg = Bmax;
                prawy->poziom = poziom+1;
                prawy->lewy = NULL;
                prawy->prawy = NULL;
            }
            else if(max(max(Rrange,Grange),Brange) == Grange){
                int* iloscWskladowej = new int[Gg - Gd +1];
                for(int i=0; i<Gg - Gd +1; ++i){
                    iloscWskladowej[i] = 0;
                }

                int iloscKolorow = 0;
                for(int i=Rd; i<=Rg; ++i){
                    for(int j=Gd; j<=Gg; ++j){
                        for(int k=Bd; k<=Bg; ++k){
                                iloscWskladowej[j-Gd] += kolory[i][j][k];
                        }
                    }
                }

                for(int i=0; i<Gg - Gd +1; ++i){
                    iloscKolorow += iloscWskladowej[i];
                }

                int srednia = iloscKolorow/2;
                iloscKolorow = 0;
                int podzial;
                int i=0;
                while(iloscKolorow<srednia){
                    iloscKolorow += iloscWskladowej[i];
                    ++i;
                }
                podzial = --i;

                    //Rmin Rmax
                for(int i=Rd; i<=Rg; ++i){
                    for(int j=Gd; j<=podzial+Gd; ++j){
                        for(int k=Bd; k<=Bg; ++k){
                            if(kolory[i][j][k] > 0 && i < Rmin){
                                Rmin = i;
                            }
                            if(kolory[i][j][k] > 0 && i > Rmax){
                                Rmax = i;
                            }
                        }
                    }
                }

               //Bmin Bmax
                for(int i=Rd; i<=Rg; ++i){
                    for(int j=Gd; j<=podzial+Gd; ++j){
                        for(int k=Bd; k<=Bg; ++k){
                            if(kolory[i][j][k] > 0 && k < Bmin){
                                Bmin = k;
                            }
                            if(kolory[i][j][k] > 0 && k > Bmax){
                                Bmax = k;
                            }
                        }
                    }
                }

                lewy = new podprzestrzen;
                lewy->Rd = Rmin;
                lewy->Gd = Gd;
                lewy->Bd = Bmin;
                lewy->Rg = Rmax;
                lewy->Gg = podzial+Gd;
                lewy->Bg = Bmax;
                lewy->poziom = poziom+1;
                lewy->lewy = NULL;
                lewy->prawy = NULL;



                    //Rmin Rmax
                for(int i=Rd; i<=Rg; ++i){
                    for(int j=podzial+Gd+1; j<=Gg; ++j){
                        for(int k=Bd; k<=Bg; ++k){
                            if(kolory[i][j][k] > 0 && i < Rmin){
                                Rmin = i;
                            }
                            if(kolory[i][j][k] > 0 && i > Rmax){
                                Rmax = i;
                            }
                        }
                    }
                }


               //Bmin Bmax
                for(int i=Rd; i<=Rg; ++i){
                    for(int j=podzial+Gd+1; j<=Gg; ++j){
                        for(int k=Bd; k<=Bg; ++k){
                            if(kolory[i][j][k] > 0 && k < Bmin){
                                Bmin = k;
                            }
                            if(kolory[i][j][k] > 0 && k > Bmax){
                                Bmax = k;
                            }
                        }
                    }
                }

                prawy = new podprzestrzen;
                prawy->Rd = Rmin;
                prawy->Gd = podzial+Gd+1;
                prawy->Bd = Bmin;
                prawy->Rg = Rmax;
                prawy->Gg = Gg;
                prawy->Bg = Bmax;
                prawy->poziom = poziom+1;
                prawy->lewy = NULL;
                prawy->prawy = NULL;
            }
            else{

                int* iloscWskladowej = new int[Bg - Bd +1];
                for(int i=0; i<Bg - Bd +1; ++i){
                    iloscWskladowej[i] = 0;
                }

                int iloscKolorow = 0;
                for(int i=Rd; i<=Rg; ++i){
                    for(int j=Gd; j<=Gg; ++j){
                        for(int k=Bd; k<=Bg; ++k){
                                iloscWskladowej[k-Bd] += kolory[i][j][k];
                                iloscKolorow += kolory[i][j][k];
                        }
                    }
                }

                int srednia = iloscKolorow/2;
                iloscKolorow = 0;
                int podzial;
                int i=0;
                while(iloscKolorow<srednia){
                    iloscKolorow += iloscWskladowej[i];
                    ++i;
                }
                podzial = --i;



                //Rmin Rmax
                for(int i=Rd; i<=Rg; ++i){
                    for(int j=Gd; j<=Gg; ++j){
                        for(int k=Bd; k<=podzial + Bd; ++k){
                            if(kolory[i][j][k] > 0 && i < Rmin){
                                Rmin = i;
                            }
                            if(kolory[i][j][k] > 0 && i > Rmax){
                                Rmax = i;
                            }
                        }
                    }
                }

                //Gmin Gmax
                for(int i=Rd; i<=Rg; ++i){
                    for(int j=Gd; j<=Gg; ++j){
                        for(int k=Bd; k<=podzial + Bd; ++k){
                            if(kolory[i][j][k] > 0 && j < Gmin){
                                Gmin = j;
                            }
                            if(kolory[i][j][k] > 0 && j > Gmax){
                                Gmax = j;
                            }
                        }
                    }
                }

                lewy = new podprzestrzen;
                lewy->Rd = Rmin;
                lewy->Gd = Gmin;
                lewy->Bd = Bd;
                lewy->Rg = Rmax;
                lewy->Gg = Gmax;
                lewy->Bg = podzial+Bd;
                lewy->poziom = poziom+1;
                lewy->lewy = NULL;
                lewy->prawy = NULL;


                //Rmin Rmax
                for(int i=Rd; i<=Rg; ++i){
                    for(int j=Gd; j<=Gg; ++j){
                        for(int k=podzial + Bd + 1; k<=Bg; ++k){
                            if(kolory[i][j][k] > 0 && i < Rmin){
                                Rmin = i;
                            }
                            if(kolory[i][j][k] > 0 && i > Rmax){
                                Rmax = i;
                            }
                        }
                    }
                }

                //Gmin Gmax
                for(int i=Rd; i<=Rg; ++i){
                    for(int j=Gd; j<=Gg; ++j){
                        for(int k=podzial+Bd+1; k<=Bg; ++k){
                            if(kolory[i][j][k]> 0 && j < Gmin){
                                Gmin = j;
                            }
                            if(kolory[i][j][k]> 0 && j > Gmax){
                                Gmax = j;
                            }
                        }
                    }
                }

                prawy = new podprzestrzen;
                prawy->Rd = Rmin;
                prawy->Gd = Gmin;
                prawy->Bd = podzial+Bd+1;
                prawy->Rg = Rmax;
                prawy->Gg = Gmax;
                prawy->Bg = Bg;
                prawy->poziom = poziom+1;
                prawy->lewy = NULL;
                prawy->prawy = NULL;
            }

            lewy->podzielDo64(kolory);
            prawy->podzielDo64(kolory);

        }
    }

    void stworzPalete(int &ind, int*** kolory){
        unsigned long long int RR = 0;
        unsigned long long int GG = 0;
        unsigned long long int BB = 0;
        int temp;
        unsigned long long int wszystkie =  0;
            if(poziom == 6){
                for(int i=Rd; i<=Rg; ++i){
                    for(int j=Gd; j<=Gg; ++j){
                        for(int k=Bd; k<=Bg; ++k){
                            temp = kolory[i][j][k];
                            RR += temp*i;
                            GG += temp*j;
                            BB += temp*k;
                            wszystkie += temp;
                        }
                    }
                }
                if(wszystkie==0){
                    tablica_kolorow[ind][0]=0;
                    tablica_kolorow[ind][1]=0;
                    tablica_kolorow[ind][2]=0;
                }
                else
                {
                    tablica_kolorow[ind][0]=RR/wszystkie;
                    tablica_kolorow[ind][1]=GG/wszystkie;
                    tablica_kolorow[ind][2]=BB/wszystkie;
                }
                ind++;
            }
            else{
                lewy->stworzPalete(ind, kolory);
                prawy->stworzPalete(ind, kolory);
            }
    }

};

//----------------------------------------------------------------------
void f_genDedykowanaPaletaKolor(){
    int*** kolory = new int**[256];

    for(int i=0; i<256; ++i){
        kolory[i] = new int*[256];
        for(int j=0; j<256; ++j){
            kolory[i][j] = new int[256];
            for(int k=0; k<256; ++k){
                kolory[i][j][k] = 0;
            }
        }
    }

    Uint8 R, G, B;
    Uint8 Rmin=255, Rmax=0, Gmin=255, Gmax=0, Bmin=255, Bmax=0;
    for(int i=0; i<width; ++i){
        for(int j=0; j<height; ++j){
            R = tab_pixeli[i][j].r;
            G = tab_pixeli[i][j].g;
            B = tab_pixeli[i][j].b;
            kolory[R][G][B] += 1;
            if(R > Rmax)
                Rmax = R;
            if(R < Rmin)
                Rmin = R;
            if(G > Gmax)
                Gmax = G;
            if(G < Gmin)
                Gmin = G;
            if(B > Bmax)
                Bmax = B;
            if(B < Bmin)
                Bmin = B;
        }
    }

    podprzestrzen* korzen = new podprzestrzen;
    korzen->Rd= Rmin;
    korzen->Gd= Gmin;
    korzen->Bd= Bmin;
    korzen->Rg= Rmax;
    korzen->Gg= Gmax;
    korzen->Bg= Bmax;
    korzen->poziom = 0;
    korzen->lewy = NULL;
    korzen->prawy = NULL;
    korzen->podzielDo64(kolory);
;
    int ind=0; //indeks tablicy_kolorów palety dedykowanej
    korzen -> stworzPalete(ind, kolory);
}

/**********************************************************************/
/*  Hubert Wąsik - generowanie palet czarnobialej/kolorowej
    oraz kompresja obrazu do 64 kolorowego obrazu*/
/**********************************************************************/

//----------------------------------------------------------------------
/* Funkcja tworzaca przestrzen potrzebna do zapisu pliku BMP */
void f_utworzPlikBMP(SDL_Surface** surface, char const* nazwa_pliku, string &Com) {

    if ( !surface ) {
        SDL_FreeSurface(*surface);
        *surface = NULL;
    }

    SDL_SaveBMP(*surface, nazwa_pliku);
    Com+=" Wygenerowano plik BMP. ";
}

//----------------------------------------------------------------------
/* Przeksztalcona funkcja z zajec ustawiajaca wartosci pikseli w przestrzeni */
void f_setPixelBMP(SDL_Surface *surface, int x, int y, Uint8 R, Uint8 G, Uint8 B) {

  if ((x>=0) && ( x < width) && ( y >= 0) && ( y <height )) {

    Uint32 pixel = SDL_MapRGB(surface->format, R, G, B);
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {

        case 1:
            *p = pixel;
            break;

        case 2:
            *(Uint16 *)p = pixel;
            break;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
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
            *(Uint32 *)p = pixel;
            break;

    }
  }
}

//----------------------------------------------------------------------
/* Funkcja tworzaca przestrzeni potrzebna do zapisu pliku BMP */
void f_utworzPrzestrzenBMP(string &Com, int numer) {

    SDL_Surface *surface;
    surface = SDL_CreateRGBSurface(0, width, height, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);

    if ( surface == NULL ) {
            Com+="Blad przy tworzeniu przestrzeni pliku BMP. Uruchom ponownie program. ";
    }

    SDL_LockSurface(surface);

    for (int i = 0; i < surface->w; i++) {
        for (int j = 0; j < surface->h; j++) {

            f_setPixelBMP(surface, i, j, tablica_kolorow[tablica_rozkodowana[j][i]][0], tablica_kolorow[tablica_rozkodowana[j][i]][1], tablica_kolorow[tablica_rozkodowana[j][i]][2]);
        }
    }

    SDL_UnlockSurface(surface);

    if (numer == 1) f_utworzPlikBMP(&surface, "HAMA - paleta narzucona.bmp", Com);
    else if (numer == 2) f_utworzPlikBMP(&surface, "HAMA - paleta dedykowana.bmp", Com);
    else if (numer == 3) f_utworzPlikBMP(&surface, "HAMA - skala szarosci narzucona.bmp", Com);
    else if (numer == 4) f_utworzPlikBMP(&surface, "HAMA - skala szarosci dedykowana.bmp", Com);
    else if (numer == 5) f_utworzPlikBMP(&surface, "Odczytany plik HAMA.bmp", Com);
    else return;
}

//----------------------------------------------------------------------
/* Funkcja odpowiedzialna za generowanie czarno-bialej palety z kolorowej palety dedykowanej */
void f_genDedykowanaPaletaSzary() {
    int Y;
    for (int i = 0; i < 64; i++){
        Y=0.299*(tablica_kolorow[i][0])+0.587*(tablica_kolorow[i][1])+0.114*(tablica_kolorow[i][2]);
        tablica_kolorow[i][0]=Y;
        tablica_kolorow[i][1]=Y;
        tablica_kolorow[i][2]=Y;
    }
}

//----------------------------------------------------------------------
/* Funkcja odpowiedzialna za generowanie czarno-bialej palety */
void f_genMonoPaleta(){

    int color; // generowany czarnobialy kolor
    for (int i = 0; i < 64; i++){

        color = i * 255 / 63;

        for (int j = 0; j < 3; j++){
            tablica_kolorow[i][j] = color;
        }
    }
}

//----------------------------------------------------------------------
/* Funkcja generująca warjacje potrzebne do palety kolorowej narzuconej */
void f_genWarjacji(string kod, int dlugosc_kodu, string stany){

    if (kod.length() >= dlugosc_kodu) {
            stos.push(kod);
    } else {
        for(int i = 0; i < stany.length(); i++)
            f_genWarjacji(kod + stany[i], dlugosc_kodu, stany);
    }
}

//----------------------------------------------------------------------
/* Funkcja odpowiedzialna za generowanie narzuconej kolorowej palety */
void f_genKolorowaPaleta(){

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

        for (int j = 0; j < 3; j++){

            stan = odczytany_stan[j];
            if ( stan == '0') tablica_kolorow[i][j] = 0;
            if ( stan == '1') tablica_kolorow[i][j] = 85;
            if ( stan == '2') tablica_kolorow[i][j] = 171;
            if ( stan == '3') tablica_kolorow[i][j] = 255;
        }
    }
}

//----------------------------------------------------------------------
/* 64 kolorowy obraz z ditheringiem, paleta narzucona */
void Funkcja1(string &Com) {

    f_genKolorowaPaleta();
    nr_palety=0;

    /* Funkcja trzecia generuje obrazek jak funkcja 3, ale nanosi korektę */
    S_bledyKolorow blad;
    SDL_Color kolor;

    /* Tworzymy tablicę dynamiczną dwuwymiarową na przechowywanie błędów o rozmiarze A na B, gdzie
    A oznacza szerokość, natomiast B oznacza długość (+2 do obu wartości)
    S_bledyKolorow jest strukturą przechowywującą do 4 błędów - RGB oraz wartosc błąd */
    S_bledyKolorow ** bledy = new S_bledyKolorow *[width + 2];
    for (int i = 0; i < tab_pixeli_w + 2; i++){
        bledy[i] = new S_bledyKolorow[height + 2];
    }

    /* Utworzenie tablicy przechowywujaca indeksy palety kolorow odpowiednich odczytanych pikseli */
    tablica_wskaznikow_palety = new int *[height];
    for (int i = 0; i < height + 2; i++){
        tablica_wskaznikow_palety[i] = new int[width];
    }


    /* Zerowanie tablicy dwuwymiarowej dynamicznej błędów */
    for(int i = 0; i < width + 2; i++){
        for(int j = 0; j < height + 2; j++){
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
    int tab [64];

    for(int i = 0; i < width; ++i)
        for(int j = 0; j < height; ++j) {

            /* Kolor odczytany z tablicy pixeli */
            kolor = tab_pixeli[i][j];

            /* Obliczanie różnicy i wpisywanie wyniku do tablicy */
            for (int k = 0; k < 64; k++){

                tab[k] =
                (kolor.r+bledy[i + przesuniecie][j].r-tablica_kolorow[k][0])*(kolor.r+bledy[i + przesuniecie][j].r-tablica_kolorow[k][0]) +
                (kolor.g+bledy[i + przesuniecie][j].g-tablica_kolorow[k][1])*(kolor.g+bledy[i + przesuniecie][j].g-tablica_kolorow[k][1]) +
                (kolor.b+bledy[i + przesuniecie][j].b-tablica_kolorow[k][2])*(kolor.b+bledy[i + przesuniecie][j].b-tablica_kolorow[k][2]);
            }

            /* Wyznaczanie najmniejszego indeksu */
            najmniejszy = tab[0];
            index = 0;
            for(int l = 1; l < 64; l++){
                if( tab[l] < najmniejszy){
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

            bledy[i + przesuniecie+1][j].r += (blad.r * 7.0/16);
            bledy[i + przesuniecie-1][j+1].r += (blad.r * 3.0/16);
            bledy[i + przesuniecie][j+1].r += (blad.r * 5.0/16);
            bledy[i + przesuniecie+1][j+1].r += (blad.r * 1.0/16);

            bledy[i + przesuniecie+1][j].g += (blad.g * 7.0/16);
            bledy[i + przesuniecie-1][j+1].g += (blad.g * 3.0/16);
            bledy[i + przesuniecie][j+1].g += (blad.g * 5.0/16);
            bledy[i + przesuniecie+1][j+1].g += (blad.g * 1.0/16);

            bledy[i + przesuniecie+1][j].b += (blad.b * 7.0/16);
            bledy[i + przesuniecie-1][j+1].b += (blad.b * 3.0/16);
            bledy[i + przesuniecie][j+1].b += (blad.b * 5.0/16);
            bledy[i + przesuniecie+1][j+1].b += (blad.b * 1.0/16);
    }

//jak jest wygenerowany plik HAMA , to ustawiamy plik_graficzny_hama_nazwa i nie korzystamy z flagi
//    flag_zrobiona_kompresja=1;
    // plik_graficzny_hama_paleta_narzucona_nazwa="";
    plik_graficzny_hama_paleta_narzucona_nazwa = plik_graficzny_nazwa + "_paleta_narzucona.hama";
    Com+="Funkcja1() Skonczone. ";
}

//----------------------------------------------------------------------
/* 64 czarnobialy obraz z ditheringiem */
void Funkcja2(string &Com) {

    f_genMonoPaleta();
    nr_palety=2;

    /* Funkcja trzecia generuje obrazek jak funkcja 3, ale nanosi korektę */
    S_bledyKolorow blad;
    SDL_Color kolor;

    /* Zmienne potrzebne do konwersji w czern-biel */
    int war_Y;
    SDL_Color pixel;

    /* Tworzymy tablicę dynamiczną dwuwymiarową na przechowywanie błędów o rozmiarze A na B, gdzie
    A oznacza szerokość, natomiast B oznacza długość (+2 do obu wartości)
    S_bledyKolorow jest strukturą przechowywującą do 4 błędów - RGB oraz wartosc błąd */
    S_bledyKolorow ** bledy = new S_bledyKolorow *[width + 2];
    for (int i = 0; i < width + 2; i++){
        bledy[i] = new S_bledyKolorow[height + 2];
    }

    /* Utworzenie tablicy przechowywujaca indeksy palety kolorow odpowiednich odczytanych pikseli */
    tablica_wskaznikow_palety = new int *[height];
    for (int i = 0; i < height + 2; i++){
        tablica_wskaznikow_palety[i] = new int[width];
    }

    /* Zerowanie tablicy dwuwymiarowej dynamicznej błędów */
    for(int i = 0; i < width + 2; i++){
        for(int j = 0; j < height + 2; j++){
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
    int tab [64];

    for(int i = 0; i < width; ++i)
        for(int j = 0; j < height; ++j) {

            /* KOLOR TO ODCZYANY DANY PIXEL Z TABLICY */
            pixel = tab_pixeli[i][j];
            war_Y = pixel.r * 0.299 + pixel.g * 0.587 + pixel.b * 0.114;

            kolor.b = kolor.g = kolor.r = war_Y;

            /* Obliczanie różnicy i wpisywanie wyniku do tablicy */
            for (int k = 0; k < 64; k++){

                tab[k] =
                (kolor.r+bledy[i + przesuniecie][j].r-tablica_kolorow[k][0])*(kolor.r+bledy[i + przesuniecie][j].r-tablica_kolorow[k][0]) +
                (kolor.g+bledy[i + przesuniecie][j].g-tablica_kolorow[k][1])*(kolor.g+bledy[i + przesuniecie][j].g-tablica_kolorow[k][1]) +
                (kolor.b+bledy[i + przesuniecie][j].b-tablica_kolorow[k][2])*(kolor.b+bledy[i + przesuniecie][j].b-tablica_kolorow[k][2]);
            }

            /* Wyznaczanie najmniejszego indeksu */
            najmniejszy = tab[0];
            index = 0;
            for(int l = 1; l < 64; l++){
                if( tab[l] < najmniejszy){
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

            bledy[i + przesuniecie+1][j].r += (blad.r * 7.0/16);
            bledy[i + przesuniecie-1][j+1].r += (blad.r * 3.0/16);
            bledy[i + przesuniecie][j+1].r += (blad.r * 5.0/16);
            bledy[i + przesuniecie+1][j+1].r += (blad.r * 1.0/16);

            bledy[i + przesuniecie+1][j].g += (blad.g * 7.0/16);
            bledy[i + przesuniecie-1][j+1].g += (blad.g * 3.0/16);
            bledy[i + przesuniecie][j+1].g += (blad.g * 5.0/16);
            bledy[i + przesuniecie+1][j+1].g += (blad.g * 1.0/16);

            bledy[i + przesuniecie+1][j].b += (blad.b * 7.0/16);
            bledy[i + przesuniecie-1][j+1].b += (blad.b * 3.0/16);
            bledy[i + przesuniecie][j+1].b += (blad.b * 5.0/16);
            bledy[i + przesuniecie+1][j+1].b += (blad.b * 1.0/16);
    }

//jak jest wygenerowany plik HAMA , to ustawiamy plik_graficzny_hama_nazwa i nie korzystamy z flagi
//    flag_zrobiona_kompresja=1;
    // plik_graficzny_hama_skala_szarosci_nazwa="";
    plik_gr_hama_skala_szarosci_narzucona_nazwa = plik_graficzny_nazwa + "_skala_szarosci_narzucona.hama";
    Com+="Funkcja2() Skonczone. ";
}

//----------------------------------------------------------------------
/* 64 kolorowy obraz z ditheringiem, paleta dedykowana */
void Funkcja3(string &Com) {

    /* Generowanie palety dedykowanej */
    f_genDedykowanaPaletaKolor();
    nr_palety=1;

    /* Funkcja trzecia generuje obrazek jak funkcja 3, ale nanosi korektę */
    S_bledyKolorow blad;
    SDL_Color kolor;

    /* Tworzymy tablicę dynamiczną dwuwymiarową na przechowywanie błędów o rozmiarze A na B, gdzie
    A oznacza szerokość, natomiast B oznacza długość (+2 do obu wartości)
    S_bledyKolorow jest strukturą przechowywującą do 4 błędów - RGB oraz wartosc błąd */
    S_bledyKolorow ** bledy = new S_bledyKolorow *[width + 2];
    for (int i = 0; i < width + 2; i++){
        bledy[i] = new S_bledyKolorow[height + 2];
    }

    /* Utworzenie tablicy przechowywujaca indeksy palety kolorow odpowiednich odczytanych pikseli */
    tablica_wskaznikow_palety = new int *[height];
    for (int i = 0; i < height + 2; i++){
        tablica_wskaznikow_palety[i] = new int[width];
    }

    /* Zerowanie tablicy dwuwymiarowej dynamicznej błędów */
    for(int i = 0; i < width + 2; i++){
        for(int j = 0; j < height + 2; j++){
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
    int tab [64];

    for(int i = 0; i < width; ++i)
        for(int j = 0; j < height; ++j) {

            /* Kolor odczytany z tablicy pixeli */
            kolor = tab_pixeli[i][j];

            /* Obliczanie różnicy i wpisywanie wyniku do tablicy */
            for (int k = 0; k < 64; k++){

                tab[k] =
                (kolor.r+bledy[i + przesuniecie][j].r-tablica_kolorow[k][0])*(kolor.r+bledy[i + przesuniecie][j].r-tablica_kolorow[k][0]) +
                (kolor.g+bledy[i + przesuniecie][j].g-tablica_kolorow[k][1])*(kolor.g+bledy[i + przesuniecie][j].g-tablica_kolorow[k][1]) +
                (kolor.b+bledy[i + przesuniecie][j].b-tablica_kolorow[k][2])*(kolor.b+bledy[i + przesuniecie][j].b-tablica_kolorow[k][2]);
            }

            /* Wyznaczanie najmniejszego indeksu */
            najmniejszy = tab[0];
            index = 0;
            for(int l = 1; l < 64; l++){
                if( tab[l] < najmniejszy){
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

            bledy[i + przesuniecie+1][j].r += (blad.r * 7.0/16);
            bledy[i + przesuniecie-1][j+1].r += (blad.r * 3.0/16);
            bledy[i + przesuniecie][j+1].r += (blad.r * 5.0/16);
            bledy[i + przesuniecie+1][j+1].r += (blad.r * 1.0/16);

            bledy[i + przesuniecie+1][j].g += (blad.g * 7.0/16);
            bledy[i + przesuniecie-1][j+1].g += (blad.g * 3.0/16);
            bledy[i + przesuniecie][j+1].g += (blad.g * 5.0/16);
            bledy[i + przesuniecie+1][j+1].g += (blad.g * 1.0/16);

            bledy[i + przesuniecie+1][j].b += (blad.b * 7.0/16);
            bledy[i + przesuniecie-1][j+1].b += (blad.b * 3.0/16);
            bledy[i + przesuniecie][j+1].b += (blad.b * 5.0/16);
            bledy[i + przesuniecie+1][j+1].b += (blad.b * 1.0/16);
    }

    //flag_zrobiona_kompresja=1;
    plik_graficzny_hama_paleta_dedykowana_nazwa = plik_graficzny_nazwa + "_paleta_dedykowana.hama";

    Com+="Funkcja3() Skonczone. ";
}

//----------------------------------------------------------------------
/* 64 czarnobialy obraz z ditheringiem, paleta dedykowana */
void Funkcja4(string &Com) {
    f_genDedykowanaPaletaKolor();
    f_genDedykowanaPaletaSzary();
    nr_palety=3;

    /* Funkcja trzecia generuje obrazek jak funkcja 3, ale nanosi korektę */
    S_bledyKolorow blad;
    SDL_Color kolor;

    /* Tworzymy tablicę dynamiczną dwuwymiarową na przechowywanie błędów o rozmiarze A na B, gdzie
    A oznacza szerokość, natomiast B oznacza długość (+2 do obu wartości)
    S_bledyKolorow jest strukturą przechowywującą do 4 błędów - RGB oraz wartosc błąd */
    S_bledyKolorow ** bledy = new S_bledyKolorow *[width + 2];
    for (int i = 0; i < width + 2; i++){
        bledy[i] = new S_bledyKolorow[height + 2];
    }

    /* Utworzenie tablicy przechowywujaca indeksy palety kolorow odpowiednich odczytanych pikseli */
    tablica_wskaznikow_palety = new int *[height];
    for (int i = 0; i < height + 2; i++){
        tablica_wskaznikow_palety[i] = new int[width];
    }

    /* Zerowanie tablicy dwuwymiarowej dynamicznej błędów */
    for(int i = 0; i < width + 2; i++){
        for(int j = 0; j < height + 2; j++){
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
    int tab [64];

    for(int i = 0; i < width; ++i)
        for(int j = 0; j < height; ++j) {

            /* Kolor odczytany z tablicy pixeli */
            kolor = tab_pixeli[i][j];

            /* Obliczanie różnicy i wpisywanie wyniku do tablicy */
            for (int k = 0; k < 64; k++){

                tab[k] =
                (kolor.r+bledy[i + przesuniecie][j].r-tablica_kolorow[k][0])*(kolor.r+bledy[i + przesuniecie][j].r-tablica_kolorow[k][0]) +
                (kolor.g+bledy[i + przesuniecie][j].g-tablica_kolorow[k][1])*(kolor.g+bledy[i + przesuniecie][j].g-tablica_kolorow[k][1]) +
                (kolor.b+bledy[i + przesuniecie][j].b-tablica_kolorow[k][2])*(kolor.b+bledy[i + przesuniecie][j].b-tablica_kolorow[k][2]);
            }

            /* Wyznaczanie najmniejszego indeksu */
            najmniejszy = tab[0];
            index = 0;
            for(int l = 1; l < 64; l++){
                if( tab[l] < najmniejszy){
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

            bledy[i + przesuniecie+1][j].r += (blad.r * 7.0/16);
            bledy[i + przesuniecie-1][j+1].r += (blad.r * 3.0/16);
            bledy[i + przesuniecie][j+1].r += (blad.r * 5.0/16);
            bledy[i + przesuniecie+1][j+1].r += (blad.r * 1.0/16);

            bledy[i + przesuniecie+1][j].g += (blad.g * 7.0/16);
            bledy[i + przesuniecie-1][j+1].g += (blad.g * 3.0/16);
            bledy[i + przesuniecie][j+1].g += (blad.g * 5.0/16);
            bledy[i + przesuniecie+1][j+1].g += (blad.g * 1.0/16);

            bledy[i + przesuniecie+1][j].b += (blad.b * 7.0/16);
            bledy[i + przesuniecie-1][j+1].b += (blad.b * 3.0/16);
            bledy[i + przesuniecie][j+1].b += (blad.b * 5.0/16);
            bledy[i + przesuniecie+1][j+1].b += (blad.b * 1.0/16);
    }

    //plik_graficzny_hama_skala_szarosci_nazwa = plik_graficzny_nazwa;
    plik_gr_hama_skala_szarosci_dedykowana_nazwa = plik_graficzny_nazwa + "_skala_szarosci_dedykowana.hama";
    Com+="Funkcja4() Skonczone. ";
}

/**********************************************************************/
/*  Anna Pawlikowska - Funkcje Huffmana - koduje zawartość
    tablica_wskaznikow_palety, wynik kodowania trafia do tablica_zakodowana*/
/**********************************************************************/

//----------------------------------------------------------------------
struct wezel
{
    char data;
    unsigned freq;
    wezel *left, *right;

    wezel(char data, unsigned freq)
    {
        left = right = NULL;
        this->data = data;
        this->freq = freq;
    }
};

//----------------------------------------------------------------------
struct compare
{
    bool operator()(wezel* l, wezel* r)
    {
        return (l->freq > r->freq);
    }
};

//----------------------------------------------------------------------
/*wpisywanie znalezionych kodów do słownika*/
void f_slownik(struct wezel* root, string str, string* odpowiedniki)
{
    if (!root)
        return;

    if (root->data != '}') {
        odpowiedniki[root->data] = str;
   }
    f_slownik(root->left, str + "0", odpowiedniki);
    f_slownik(root->right, str + "1", odpowiedniki);
}

//----------------------------------------------------------------------
/*tworzenie kodów Huffmana w oparciu o drzewo i kolejkę priorytetową*/
void f_kody(char data[], int freq[], int size, string* odpowiedniki)
{
    struct wezel *left, *right, *top;

    priority_queue<wezel*, vector<wezel*>, compare> najmn;
    for (int i = 0; i < size; ++i)
        najmn.push(new wezel(data[i], freq[i]));

    while (najmn.size() != 1)
    {
        left = najmn.top();
        najmn.pop();

        right = najmn.top();
        najmn.pop();

        top = new wezel('}', left->freq + right->freq);
        top->left = left;
        top->right = right;
        najmn.push(top);
    }

    f_slownik(najmn.top(), "", odpowiedniki);
}

//----------------------------------------------------------------------
/* używa słownika do rozkodowania tablicy tablica_zakodowana, wynik zapisuje do tablicy tablica_rozkodowana
TO SŁUŻY DO ODCZYTU ZAKODOWANEJ TABLICY*/
void f_rozkoduj_huffman(){

tablica_rozkodowana = new int *[height];
    for (int i = 0; i < height; i++){
        tablica_rozkodowana[i] = new int[width];
    }

for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            for(int k = 0; k < 64; k++){
                if(tablica_zakodowana[i][j] == slownik[k]){
                    tablica_rozkodowana[i][j] = k;
                    break;
                }
            }
        }

    }
}

//----------------------------------------------------------------------
/*główne ciało funkcji, wynik zapisywany do tablicy tablica_zakodowana
TA FUNKCJA INICJUJE I ZAPISUJE TABLICĘ DO WPISANIA DO PLIKU*/
void f_huffman(string &Com){
    int rozmiar = 0;
    int c = 0, count[64] = {0};

    tablica_zakodowana = new string *[height];
    for (int i = 0; i < height; i++){
        tablica_zakodowana[i] = new string[width];
    }

    //zlicza występowanie danych kolorów reprezentowanych wartościami int od 0 do 63
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            count[tablica_wskaznikow_palety[i][j]]++;
        }
    }

    for ( c = 0 ; c < 64 ; c++ )
    {
        if( count[c] != 0 )
            rozmiar++;
    }

    //przygotowanie danych do wywołania funkcji budującej kody
    char* arr = new char[rozmiar];
    int* freq = new int[rozmiar];

    int licznik = 0;
    for ( c = 0 ; c < 64 ; c++ )
    {
        if( count[c] != 0 )
        {
            arr[licznik] = c;
            freq[licznik++] = count[c];
        }
    }

    f_kody(arr, freq, rozmiar, slownik);
    int wyp;
    //wypełnia tablicę wynikową
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            wyp=tablica_wskaznikow_palety[i][j];
            tablica_zakodowana[i][j] = slownik[wyp];
        }
    }
    Com+="f_huffman(). Skonczone. ";
}

/**********************************************************************/
/* Agata Bajorska - obsługa menu + funkcja main + wczytywanie plikow
   + ładowanie pliku BMP do tab_pixeli+ aktualny podgląd pliku  */
/**********************************************************************/

//----------------------------------------------------------------
void f_bmp_do_tab_pixeli()
{
  SDL_Color Pixel;  //pojedynczy pixel z obrazka
  SDL_Surface* bmp = SDL_LoadBMP(plik_graficzny_nazwa.c_str());

  //wysokosc i szerokosc obrazka, czyli wymiary tab_pixeli
  width = tab_pixeli_w = bmp->w;
  height = tab_pixeli_h = bmp->h;

  tab_pixeli = new SDL_Color *[width];
  for(int i=0; i<bmp->w; i++){ tab_pixeli[i] = new SDL_Color[height]; }


  for (int i=0; i<width; i++)      //szerokosc obrazka
  {
      for(int j=0; j<height; j++)   //wysokosc obrazka
      {
          SDL_LockSurface(bmp);
          Uint8 *p= (Uint8*) bmp->pixels + j*bmp->pitch + i * bmp->format->BytesPerPixel;
          SDL_UnlockSurface(bmp);
            //Uint32 a = getpixel(bmp, i,j);
            Pixel.r =*(p+2);
            Pixel.g =*(p+1);
            Pixel.b = p[0];

            tab_pixeli[i][j]=Pixel;
            //setPixel(i,j,Pixel.r, Pixel.g,Pixel.b);
      }
  }
    //SDL_Flip(screen);
}

//----------------------------------------------------------------
void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B)
{
  if ((x>=0) && (x<width) && (y>=0) && (y<height))
  {
    /* Zamieniamy poszczególne skladowe koloru na format koloru pixela */
    Uint32 pixel = SDL_MapRGB(screen->format, R, G, B);

    /* Pobieramy informacji ile bajtów zajmuje jeden pixel */
    int bpp = screen->format->BytesPerPixel;

    /* Obliczamy adres pixela */
    Uint8 *p = (Uint8 *)screen->pixels + y * screen->pitch + x * bpp;

    /* Ustawiamy wartosc pixela, w zale¿noœci od formatu powierzchni*/
    switch(bpp)
    {
        case 1: //8-bit
            *p = pixel;
            break;

        case 2: //16-bit
            *(Uint16 *)p = pixel;
            break;

        case 3: //24-bit
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
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
            *(Uint32 *)p = pixel;
            break;

    }
         /* update the screen (aka double buffering) */
  }
}

//----------------------------------------------------------------
SDL_Color getPixel (int x, int y)
{
    SDL_Color color ;
    Uint32 col = 0 ;
    if ((x>=0) && (x<width) && (y>=0) && (y<height)) {
        //determine position
        char* pPosition=(char*)screen->pixels ;
        //offset by y
        pPosition+=(screen->pitch*y) ;
        //offset by x
        pPosition+=(screen->format->BytesPerPixel*x);
        //copy pixel data
        memcpy(&col, pPosition, screen->format->BytesPerPixel);
        //convert color
        SDL_GetRGB(col, screen->format, &color.r, &color.g, &color.b);
    }
    return ( color ) ;
}

//----------------------------------------------------------------------
/* funkcja przesuwa kursor w lewy gorny rog ekranu consoli */
void f_clear_console()
{
    COORD topLeft  = { 0, 0 };
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD written;

    GetConsoleScreenBufferInfo(console, &screen);
    FillConsoleOutputCharacterA(
        console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written);
    FillConsoleOutputAttribute(
        console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
        screen.dwSize.X * screen.dwSize.Y, topLeft, &written);
    SetConsoleCursorPosition(console, topLeft);
}

//----------------------------------------------------------------------
/* wyswietla obrazek zapisany w plik_graficzny_nazwa w osobnym oknie graficznym */
int f_pokaz_obrazek(string X)
{
    ////SDL_Surface *screen = NULL;
    SDL_Surface *obrazek = NULL;
    SDL_Event event;    //zdarzenie-nacisniecie klawisza lub zamkniecie okienka
    bool done = false;
    string Com;

  SDL_Color Pixel;  //pojedynczy pixel z obrazka
  //SDL_Surface* bmp = SDL_LoadBMP(plik_graficzny_nazwa.c_str());


    SDL_Init( SDL_INIT_EVERYTHING );

    screen = SDL_SetVideoMode( width, height, 32, SDL_SWSURFACE );

    if(X == "z_pliku")
    {
        obrazek = SDL_LoadBMP( plik_graficzny_nazwa.c_str() );
        SDL_BlitSurface( obrazek, NULL, screen, NULL );
    }
    else if(X == "from_tab_pixeli")
    {
        for (int i=0; i<width; i++)      //szerokosc obrazka
        {
            for(int j=0; j<height; j++)   //wysokosc obrazka
            {
                Pixel = tab_pixeli[i][j];
                //on global screen
                setPixel(i,j,Pixel.r, Pixel.g, Pixel.b);
            }
        }
    }
    else if(X == "from_hama")
    {
        f_odczytajHAMA("inny", Com);
    }

    SDL_Flip( screen );

    //SDL_Delay( 4000 );
    //petla ktora sprawdza nacisniecie Esc na klawiaturze lub zamkniecie okienka
    while( !done )
    {   while( SDL_PollEvent( &event ) )
        {
            if (event.key.keysym.sym == SDLK_ESCAPE){ done = true; }
            if( event.type == SDL_QUIT ){ done = true; }
        }
    }

    SDL_Quit();
    //SDL_FreeSurface( obrazek ); //?

return 0;
}

//----------------------------------------------------------------------
/* sprawdzenie czy istnieje plik graficzny w biezacym katalogi, zapisanie nazwy do zmiennych globalnych
   _plik = nazwa pliku wprowadzona z klawiatury */
int f_readFile(string X, string _plik)
{
    int Ret=1;

    if(X == "bmp")
    {
        plik_graficzny_nazwa="";

        ifstream ifile(_plik.c_str());
        if(ifile.good())
        {   ifile.close();
            plik_graficzny_nazwa = _plik;
            //plik_graficzny = SDL_LoadBMP( plik_graficzny_nazwa.c_str() );
            Ret=0;
        }
    }
    else if(X == "hama")
    {
        plik_graficzny_inny_hama_nazwa="";

        ifstream ifile(_plik.c_str());
        if(ifile.good())
        {   ifile.close();
            plik_graficzny_inny_hama_nazwa = _plik;
            Ret=0;
        }
    }

return Ret;
}

//----------------------------------------------------------------------
void f_menu_header()
{
    cout<<endl;
    cout<<" #################################################"<<endl;
    cout<<" #"<<" PROJEKT GKiM - zmiana formatu graficznego     #"<<endl;
    cout<<" #################################################"<<endl;
}

//----------------------------------------------------------------------
/* menu BMP na HAMA */
void f_menu_konwersja_BMP_HAMA()
{
    int M=1, i=0;
    string S="", Com="";

    while(M != 0)
    {
        f_clear_console();
        for(i=0; i<30; i++){ cout<<setw(100)<<" "<<endl; }
        f_clear_console();
        f_menu_header();

            cout<<" Konwersja z BMP -> HAMA:"<<endl;
            cout<<" [1] Paleta narzucona z ditheringiem."<<endl;
            cout<<" [2] Paleta dedykowana z ditheringiem."<<endl;
            cout<<" [3] Skala szarosci narzucona z ditheringiem."<<endl;
            cout<<" [4] Skala szarosci dedykowana z ditheringiem."<<endl;

        if(plik_graficzny_hama_paleta_narzucona_nazwa != ""){
            cout<<" [5] Wyswietl plik HAMA (paleta narzucona z ditheringiem) '" + plik_graficzny_hama_paleta_narzucona_nazwa + "'."<<endl;
        }
        else {
            cout<<" [ ] Brak pliku HAMA (paleta narzucona z ditheringiem)"<<endl;
        }
        if(plik_graficzny_hama_paleta_dedykowana_nazwa != ""){
            cout<<" [6] Wyswietl plik HAMA (paleta dedykowana z ditheringiem) '" + plik_graficzny_hama_paleta_dedykowana_nazwa + "'."<<endl;
        }
        else {
            cout<<" [ ] Brak pliku HAMA (paleta dedykowana z ditheringiem)"<<endl;
        }
        if(plik_gr_hama_skala_szarosci_narzucona_nazwa != ""){
            cout<<" [7] Wyswietl plik HAMA (skala szarosci narzucona z ditheringiem) '" + plik_gr_hama_skala_szarosci_narzucona_nazwa + "'."<<endl;
        }
        else {
            cout<<" [ ] Brak pliku HAMA (skala szarosci narzucona z ditheringiem)"<<endl;
        }
        if(plik_gr_hama_skala_szarosci_dedykowana_nazwa != ""){
            cout<<" [8] Wyswietl plik HAMA (skala szarosci dedykowana z ditheringiem) '" + plik_gr_hama_skala_szarosci_dedykowana_nazwa + "'."<<endl;
        }
        else {
            cout<<" [ ] Brak pliku HAMA (skala szarosci dedykowana z ditheringiem)"<<endl;
        }

            cout<<" [10] Odswiez ekran."<<endl;
            cout<<" [0] Cofnij."<<endl;
            cout<<"-------------------------------------------------"<<endl;
        if(Com != ""){ cout<<" "<<Com<<endl; Com=""; }
            cout<<" Wybor: "; cin>>S; //c=getchar();

        if(S == "1") {

            Funkcja1(Com);
            f_huffman(Com);
            f_zapisDoHAMA(Com);
            f_zerowanie_tablic();
            f_bmp_do_tab_pixeli();
        }

        else if(S == "2"){

            Funkcja3(Com);
            f_huffman(Com);
            f_zapisDoHAMA(Com);
            f_zerowanie_tablic();
            f_bmp_do_tab_pixeli();
        }

        else if(S == "3"){

            Funkcja2(Com);
            f_huffman(Com);
            f_zapisDoHAMA(Com);
            f_zerowanie_tablic();
            f_bmp_do_tab_pixeli();
        }
        else if(S=="4"){
            Funkcja4(Com);
            f_huffman(Com);
            f_zapisDoHAMA(Com);
            f_zerowanie_tablic();
            f_bmp_do_tab_pixeli();
        }
        else if(S == "5" && plik_graficzny_hama_paleta_narzucona_nazwa != ""){
            f_wypiszHAMA("paleta_narzucona_z_ditheringiem",Com);
            f_zerowanie_tablic();
        }
        else if(S == "6" && plik_graficzny_hama_paleta_dedykowana_nazwa != ""){
            f_wypiszHAMA("paleta_dedykowana_z_ditheringiem",Com);
            f_zerowanie_tablic();
        }
        else if(S == "7" && plik_gr_hama_skala_szarosci_narzucona_nazwa != ""){
            f_wypiszHAMA("skala_szarosci_narzucona_z_ditheringiem",Com);
            f_zerowanie_tablic();
        }
        else if(S == "8" && plik_gr_hama_skala_szarosci_dedykowana_nazwa != ""){
            f_wypiszHAMA("skala_szarosci_dedykowana_z_ditheringiem",Com);
            f_zerowanie_tablic();
        }
        else if(S == "10"){ for(i=0; i<60; i++){ cout<<setw(100)<<" "<<endl; } }
        else if(S == "0"){ M=0; }
        else             { Com=" Zly wybor!"; }
    }

}

//----------------------------------------------------------------------
/* menu HAMA na BMP */
void f_menu_konwersja_HAMA_BMP()
{
    int M=1, i=0;
    string S="", Com="";

    while(M != 0)
    {
        f_clear_console();
        for(i=0; i<30; i++){ cout<<setw(100)<<" "<<endl; }
        f_clear_console();
        f_menu_header();

            cout<<" Konwersja z HAMA -> BMP:"<<endl;
        if(plik_graficzny_hama_paleta_narzucona_nazwa != ""){
            cout<<" [1] Rozpocznij konwersje pliku HAMA (paleta narzucona z ditheringiem) '" + plik_graficzny_hama_paleta_narzucona_nazwa + "'."<<endl;
        } else{
            cout<<" [ ] Konwersja dostepna dopiero po utworzeniu pliku HAMA (paleta narzucona z ditheringiem)"<<endl;
        }
        if(plik_graficzny_hama_paleta_dedykowana_nazwa != ""){
            cout<<" [2] Rozpocznij konwersje pliku HAMA (paleta dedykowana z ditheringiem) '" + plik_graficzny_hama_paleta_dedykowana_nazwa + "'."<<endl;
        } else{
            cout<<" [ ] Konwersja dostepna dopiero po utworzeniu pliku HAMA (paleta dedykowana z ditheringiem)"<<endl;
        }
        if(plik_gr_hama_skala_szarosci_narzucona_nazwa != ""){
            cout<<" [3] Rozpocznij konwersje pliku HAMA (skala szarosci narzucona z ditheringiem) '" + plik_gr_hama_skala_szarosci_narzucona_nazwa + "'."<<endl;
        } else{
            cout<<" [ ] Konwersja dostepna dopiero po utworzeniu pliku HAMA (skala szarosci narzucona z ditheringiem)"<<endl;
        }
        if(plik_gr_hama_skala_szarosci_dedykowana_nazwa != ""){
            cout<<" [4] Rozpocznij konwersje pliku HAMA (skala szarosci dedykowana z ditheringiem) '" + plik_gr_hama_skala_szarosci_dedykowana_nazwa + "'."<<endl;
        } else{
            cout<<" [ ] Konwersja dostepna dopiero po utworzeniu pliku HAMA (skala szarosci dedykowana z ditheringiem)"<<endl;
        }

            cout<<" [10] Odswiez ekran."<<endl;
            cout<<" [0] Cofnij."<<endl;
            cout<<"-------------------------------------------------"<<endl;
        if(Com != ""){ cout<<" "<<Com<<endl; Com=""; }
            cout<<" Wybor: "; cin>>S;


        if(S == "1" && plik_graficzny_hama_paleta_narzucona_nazwa != ""){

            /* WPIERW IDZIE FUNKCJA - PODAJ LOKALIZACJE PLIKU HAMA */
//          f_odczytajHAMA("plik.hama");/* POTEM FUNKCJA ODCZYTUJACA DANE Z PLIKU HAMA */
            f_odczytajHAMA("paleta_narzucona_z_ditheringiem",Com); /* POTEM FUNKCJA ODCZYTUJACA DANE Z PLIKU HAMA */
            f_rozkoduj_huffman();
            /* Utworzenie pliku BMP na podstawie HAMA */
            f_utworzPrzestrzenBMP(Com, 1);
            f_zerowanie_tablic();

        }
        else if(S == "2" && plik_graficzny_hama_paleta_dedykowana_nazwa != ""){
            // konwersja
            f_odczytajHAMA("paleta_dedykowana_z_ditheringiem",Com);
            f_rozkoduj_huffman();

            /* Utworzenie pliku BMP na podstawie HAMA */
            f_utworzPrzestrzenBMP(Com, 2);
            f_zerowanie_tablic();
        }
        else if(S == "3" && plik_gr_hama_skala_szarosci_narzucona_nazwa != ""){
            // konwersja
            f_odczytajHAMA("skala_szarosci_narzucona_z_ditheringiem",Com);
            f_rozkoduj_huffman();

            /* Utworzenie pliku BMP na podstawie HAMA */
            f_utworzPrzestrzenBMP(Com, 3);
            f_zerowanie_tablic();
        }
        else if(S == "4" && plik_gr_hama_skala_szarosci_dedykowana_nazwa != ""){
            // konwersja
            f_odczytajHAMA("skala_szarosci_dedykowana_z_ditheringiem",Com);
            f_rozkoduj_huffman();

            /* Utworzenie pliku BMP na podstawie HAMA */
            f_utworzPrzestrzenBMP(Com, 4);
            f_zerowanie_tablic();
        }
        else if(S == "10"){ for(i=0; i<60; i++){ cout<<setw(100)<<" "<<endl; } }
        else if(S == "0"){ M=0; }
        else             { Com+=" Zly wybor!"; }
    }

}

//----------------------------------------------------------------------
/* glowna petla programu (menu) */
int main ( int argc, char** argv )
{

    int M=1, i=0;
    string S="", Com="";

    // console output
    freopen( "CON", "wt", stdout );
    freopen( "CON", "wt", stderr );

    // make sure SDL cleans up before exit
    //atexit(SDL_Quit);

    while(M != 0)
    {
        f_clear_console();
        for(i=0; i<30; i++){ cout<<setw(100)<<" "<<endl; }
        f_clear_console();
        f_menu_header();

            cout<<" MENU:"<<endl;
        if(plik_graficzny_nazwa == "" ) {
            cout<<" [1] Wczytaj plik graficzny BMP."<<endl;
        }
        else {
            cout<<" [1] Wczytano '" << plik_graficzny_nazwa << "' Wczytaj inny plik graficzny BMP."<<endl;
        }
        if(M == 1) {
            cout<<" [2] Wczytaj plik graficzny HAMA i wykonaj konwersje z HAMA do BMP."<<endl;
        }

        if(M == 2) {
            cout<<" [2] Konwersja z BMP -> HAMA."<<endl;
            cout<<" [3] Konwersja z HAMA -> BMP."<<endl;
            //cout<<" [4] Wyswietl aktualnie przechowywany plik BMP '" + plik_graficzny_nazwa + "'."<<endl;
            cout<<" [4] Wyswietl aktualnie otwarty plik BMP."<<endl;

        }
            cout<<" [0] Exit"<<endl;
            cout<<"-------------------------------------------------"<<endl;
        if(Com != ""){ cout<<" "<<Com<<endl; Com=""; }
            cout<<" Wybor: "; cin>>S;

        if(S == "1")
        {   S="";
            cout<<" Podaj nazwe: "; cin>>S;
            if(f_readFile("bmp",S) == 0)
            {   Com+=" Plik " + plik_graficzny_nazwa + " zostal poprawnie wczytany.";
                f_bmp_do_tab_pixeli();
                M=2;
            }
            else { Com="Plik '" + S + "' nie istnieje!"; }
        }
        else if(S == "2" && M == 1)
        {   S="";
            cout<<" Podaj nazwe pliku HAMA: "; cin>>S;
            if(f_readFile("hama",S) == 0)
            {   Com+=" Plik " + plik_graficzny_inny_hama_nazwa + " zostal poprawnie wczytany.";
            }
            else { Com="Plik '" + S + "' nie istnieje!"; }
            //MP tu dopisuję od razu konwersję
            // konwersja

            f_odczytajHAMA("inny",Com);
            f_rozkoduj_huffman();
            Com+="\n";
            /* Utworzenie pliku BMP na podstawie HAMA */
            f_utworzPrzestrzenBMP(Com, 5);
            M=1;
        }
        else if(S == "2" && M == 2){ f_menu_konwersja_BMP_HAMA(); }
        else if(S == "3" && M == 2){ f_menu_konwersja_HAMA_BMP(); }
        else if(S == "4" && M == 2){ f_pokaz_obrazek("from_tab_pixeli"); }

        //else if(S == "10"){ for(i=0; i<60; i++){ cout<<setw(100)<<" "<<endl; } }
        else if(S == "0")          { M=0; }
        else                       { Com=" Zly wybor!"; }
    }

  //czyszczenie tab_pixeli
  for(int i=0; i<tab_pixeli_w; i++){ delete [] tab_pixeli[i]; }
  delete [] tab_pixeli;

return 0;
}
