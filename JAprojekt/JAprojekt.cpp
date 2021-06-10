#include <iostream>
#include <iomanip>
#include <fstream>
#include <windows.h>
#include <thread>
#include <queue>
#include <string>
#include <time.h>
#include <chrono>
#include <ctime>

typedef int(_fastcall* MNOZENIE)(int*, int*, int*, int, int, int, int);
typedef int(_fastcall* MNOZENIE_ASM)(int*, int*, int*, int, int, int,int);
HINSTANCE dllHandle = NULL;

int podaj_liczbe_watkow();
//bool zapisz_czas_do_pliku(string nazwa_pliku_wyj, int liczba_watkow, double czas);

using namespace std::chrono;
using namespace std;

bool zapisz_czas_do_pliku(string nazwa_pliku_wyj, int liczba_watkow, double czas)
{
    time_t now = time(0);

    // convert now to string form
    char* dt = ctime(&now);

   // cout << "The local date and time is: " << dt << endl;


    string kopia_nazwy = nazwa_pliku_wyj;
    kopia_nazwy.erase(0, nazwa_pliku_wyj.size() - 4);
    if (!(kopia_nazwy == ".txt")) return false;

    fstream plik_wyj;
    plik_wyj.open(nazwa_pliku_wyj, ios::app);
    if (plik_wyj.is_open())
    {
        plik_wyj << liczba_watkow <<';'<< czas <<';'<< dt;

        plik_wyj.close();
        return true;
    }
    else return false;
}

bool czy_liczba(string text) {

    for (int i = 0; i < text.length(); i++) if (text[i] > '9' || text[i] < '0') return false;

    return true;

}

bool wczytaj_dane(string plik,int *&A,int *&B,int *&C,int &n,int &m,int &p)
{
    string a;
    // odczytujemy wymiary macierzy
    fstream plik_wej;
    plik_wej.open(plik, fstream::in);
    if (plik_wej.is_open())
    {
        plik_wej >> a;
        if (czy_liczba(a) == false) return false;
        n = atoi(a.c_str());
        plik_wej >> a;
        if (czy_liczba(a) == false) return false;
        m = atoi(a.c_str());
        plik_wej >> a;
        if (czy_liczba(a) == false) return false;
        p = atoi(a.c_str());
        if (n != p)return false;
    }
    else return false;

    // tworzymy macierze o odpowiednich rozmiarach

    A = new int [n*m];
    B = new int [m*p];
    C = new int [n*p];

    for (int i = 0; i < n*p; i++) C[i] = 0;

    // odczytujemy dane dla macierzy A

    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
        {
            plik_wej >> a;
            if (czy_liczba(a) == false) return false;
            A[i*m+j]= atoi(a.c_str());
           
        }

    // odczytujemy dane dla macierzy B

    for (int i = 0; i < m; i++)
        for (int j = 0; j < p; j++)
        {
            plik_wej >> a;
            if (czy_liczba(a) == false) return false;
            B[i*p+j] = atoi(a.c_str());
        }
    plik_wej.close();
    return true;
}

bool zapisz_dane(string nazwa_pliku,int *&C,int &n,int &p)
{
    string kopia_nazwy = nazwa_pliku;
    kopia_nazwy.erase(0, nazwa_pliku.size() - 4);
    if (!(kopia_nazwy == ".txt")) return false;

    fstream plik_wyj;
    plik_wyj.open(nazwa_pliku, fstream::out);
    if (plik_wyj.is_open())
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < p; j++)
            {
                plik_wyj << setw(6) << C[i * p + j] << " ";
            }
            plik_wyj << endl;
        }
        plik_wyj.close();
        return true;
    }
    else return false;
   
}

void przetwarzanie_asm(int*& A, int*& B, int*& C, int& n, int& m, int& p,int o)
{
    int ilosc_w = o;
       // podaj_liczbe_watkow();
    dllHandle = LoadLibrary(L"Asm_Dll.dll");
    MNOZENIE_ASM procedura_asm = (MNOZENIE_ASM)GetProcAddress(dllHandle, "MNOZENIE_ASM");     
    auto start_time = std::chrono::high_resolution_clock::now();
     //procedura_asm(A, B, C, n, m, p, 0);
  

       queue<thread> watki;
       for (int i = 0; i < ilosc_w; i++)
       {
           int x = (i + 1) * n / ilosc_w;
           int y = i * n / ilosc_w;
           watki.push(move(thread(procedura_asm, A, B, C, x, m, p, y)));
           //cout << i * n / ilosc_w << "  " << (i + 1) * n / ilosc_w << endl;
       }
       while (!watki.empty())
       {
           watki.front().join();
           watki.pop();
       }


        auto end_time = chrono::high_resolution_clock::now();
        auto time = end_time - start_time;
       // cout << time / chrono::milliseconds(1) << "ms to run.\n";
        double czas = time / chrono::milliseconds(1);
        zapisz_czas_do_pliku("czasy_asm.txt", ilosc_w, czas);
}

void przetwarzanie_cpp(int*& A, int*& B, int*& C, int& n, int& m, int& p,int o)
{
    int ilosc_w = o;
        //odaj_liczbe_watkow();
    dllHandle = LoadLibrary(L"Cpp_Dll.dll");
    MNOZENIE procedura = (MNOZENIE)GetProcAddress(dllHandle, "MNOZENIE");

    procedura(A, B, C, 1, 2, 3, 4);

    auto start_time = std::chrono::high_resolution_clock::now();
    queue<thread> watki;
    for (int i = 0; i < ilosc_w; i++)
    {
        watki.push(move(thread(procedura, A, B, C, (i + 1) * n / ilosc_w, m, p, i * n / ilosc_w)));
        //cout << i * n / ilosc_w << "  " << (i + 1) * n / ilosc_w << endl;
    }
    while (!watki.empty())
    {
        watki.front().join();
        watki.pop();
    }
   
    auto end_time = chrono::high_resolution_clock::now();
    auto time = end_time - start_time;
   // cout << time /chrono::milliseconds(1) << "ms to run.\n";
    double czas = time / chrono::milliseconds(1);
    zapisz_czas_do_pliku("czasy_cpp.txt", ilosc_w, czas);
    

   
}

void zwolnij_pamiec(int *&A,int *&B,int *&C)
{
    if (A != nullptr && B != nullptr && C != nullptr)
    {
        delete[] A;
        delete[] B;
        delete[] C;
    }
}

void menu_glowne(int *&A,int *&B,int *&C,int &n,int &m,int &p)
{
    char znak;
    string nazwa_pliku_wej;
    string nazwa_pliku_wyj;
    int ponowny_wybor = 0, ponowny_wybor1 = 0, ponowny_wybor2 = 0;
    
    while (1)
    {
        cout << "*****MNOZENIE MACIERZY*****" << endl;
        Sleep(1000);
        system("cls");
        do {
            if (ponowny_wybor1)
            {
                system("cls");
                cout << "Blad pliku wejsciowego! Sprawdz nazwe i format" << endl;
                Sleep(1000);
                system("cls");
            }
            cout << "Podaj nazwe pliku wejsciowego: ";
            cin >> nazwa_pliku_wej;
            ponowny_wybor1 = 1;
        } while (wczytaj_dane(nazwa_pliku_wej, A, B, C, m, n, p) != true);
            ponowny_wybor1 = 0;
            system("cls");
            
        do
        {
            if (ponowny_wybor)
            {
                system("cls");
                cout << "Nieprawidlowy wybor!" << endl;
                Sleep(1000);
                system("cls");
            }

            cout << "1. Mnozenie macierzy [ASM]" << endl;
            cout << "2. Mnozenie macierzy [CPP]" << endl;
            cout << "3. Wyjscie z programu" << endl;
            cout << "Wybor: ";
            cin >> znak;

            ponowny_wybor = 1;
            if (znak == '1') przetwarzanie_asm(A, B, C, m, n, p,1);
            else if (znak == '2') przetwarzanie_cpp(A,B,C,m,n,p,1);
            else if (znak == '3') return;
        } while (znak != '0' && znak != '1' && znak!='2');
        ponowny_wybor = 0;

        do
        {
            if (ponowny_wybor2)
            {
                system("cls");
                cout << "Blad pliku wyjsciowego! Sprawdz nazwe" << endl;
                Sleep(1000);
                system("cls");
            }
            cout << "Podaj nazwe pliku wyjsciowego: ";
            cin >> nazwa_pliku_wyj;
            ponowny_wybor2 = 1;
        } while (zapisz_dane(nazwa_pliku_wyj, C, m, p) != true);
        ponowny_wybor2 = 0;
        zwolnij_pamiec(A, B, C);
        cout << "Program zakonczono poprawnie! " << endl;
        Sleep(1000);
        system("cls");
    }
}

int podaj_liczbe_watkow()
{
    int liczba_watkow;
    do
    {
        cout << "Twoje urzadzenie posiada " << thread::hardware_concurrency() << " watkow"<<endl;
        cout << "Podaj liczbe watkow do przetwarzania [1-64]: ";
        cin >> liczba_watkow;
    } while (liczba_watkow<1 || liczba_watkow>64);
    return liczba_watkow;
}

bool tworzenie_duzej_macierzy(int a, int b, int c, string nazwa_pliku_wyj)
{
    srand(time(NULL));
    fstream plik_wyj;
    plik_wyj.open(nazwa_pliku_wyj, ios::out);
    if (plik_wyj.is_open())
    {
        plik_wyj << a << " " << b << " " << c << endl;
        for (int i = 0; i < a; i++)
        {
            for (int j = 0; j < b; j++)
            {
                int liczba = rand() % 10;
                plik_wyj << liczba << " ";
            }
            plik_wyj << endl;
        }

        for (int i = 0; i < b; i++)
        {
            for (int j = 0; j < c; j++)
            {
                int liczba = rand() % 5;
                plik_wyj << liczba << " ";
            }
            plik_wyj << endl;
        }


        plik_wyj.close();
        return true;
    }
    else return false;
}

int main()
{

    int* A = nullptr, * B = nullptr, * C = nullptr, m, n, p;
   // menu_glowne(A,B,C,m,n,p);
   // zapisz_czas_do_pliku("czasy.txt", 8, 182.9928);

    //tworzenie_duzej_macierzy(2048, 2048 , 2048, "b_duza.txt");
    //for (int j = 1; j < 9; j++)
    //{
    //    for (int i = 0; i < 3; i++)
    //    {
         wczytaj_dane("mala_m.txt", A, B, C, n, m, p);
         przetwarzanie_asm(A, B, C, n, m, p,8);
    //       // cout << j;
    //        //zapisz_dane("test_wynik_asm.txt", C, n, p);
    //    }
    //}
   /*      int MM[9] = { 1,2,3,4,5,6,7,8,9 };
         int BB[9];
         for (int i = 0; i < 3; i++)
         {
             for (int j = 0; j < 3; j++)
             {
                 cout << MM[i*3+j] << " ";
             }
             cout << endl;            
         }
                 
             
         for (int i = 0; i < 3; i++)
             for (int j = 0; j < 3; j++) BB[i * 3 + j] = MM[j * 3 + i];

         for (int i = 0; i < 3; i++)
         {
             for (int j = 0; j < 3; j++)
             {
                 cout << BB[i * 3 + j] << " ";
             }
             cout << endl;
         }
         Sleep(5000);*/


                 
    return 0;
    
}