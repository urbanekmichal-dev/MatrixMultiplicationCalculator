#include "pch.h"

using namespace std;

int MNOZENIE(int *A,int *B,int *C,int m,int n, int p,int x)
{
    for (int i = x; i < m; i++)
        for (int j = 0; j < p; j++)
        {
            int s = 0;
            for (int k = 0; k < n; k++)  s += A[i * n + k] * B[k * p + j];
            C[i * p + j] = s;
            

           
        }
    return 1;
}



