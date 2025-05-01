#include <iostream>
#include <cstdlib>
#include <omp.h>
using namespace std;

void print_mat(int **mat, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            cout << mat[i][j] << "\t";
        }
        cout << endl;
    }
    cout << endl;
}

void matmul(int **A, int **B, int **C, int m, int n, int p)
{
#pragma omp parallel for
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < p; j++)
        {
            C[i][j] = 0;
            for (int k = 0; k < n; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main()
{
    int m, n, b_rows, p;

    cout << "Enter dimensions for Matrix A (rows columns): ";
    cin >> m >> n;
    cout << "Enter dimensions for Matrix B (rows columns): ";
    cin >> b_rows >> p;

    // Validate matrix multiplication compatibility
    if (n != b_rows)
    {
        cerr << "Error: Matrix A columns (" << n << ") must match Matrix B rows (" << b_rows << ")!" << endl;
        return 1;
    }

    // Allocate matrices
    int **A = new int *[m];
    int **B = new int *[b_rows]; // Using b_rows instead of n for clarity
    int **C = new int *[m];

    // Initialize matrix A
    for (int i = 0; i < m; i++)
    {
        A[i] = new int[n];
        for (int j = 0; j < n; j++)
        {
            A[i][j] = rand() % 10;
        }
    }

    // Initialize matrix B
    for (int i = 0; i < b_rows; i++)
    {
        B[i] = new int[p];
        for (int j = 0; j < p; j++)
        {
            B[i][j] = rand() % 10;
        }
    }

    // Initialize result matrix C
    for (int i = 0; i < m; i++)
    {
        C[i] = new int[p];
    }

    cout << "\nMatrix A (" << m << "X" << n << "):\n";
    print_mat(A, m, n);
    cout << "Matrix B (" << b_rows << "X" << p << "):\n";
    print_mat(B, b_rows, p);

    matmul(A, B, C, m, n, p);

    cout << "Result Matrix C (" << m << "X" << p << "):\n";
    print_mat(C, m, p);

    // Clean up memory
    for (int i = 0; i < m; i++)
    {
        delete[] A[i];
        delete[] C[i];
    }
    for (int i = 0; i < b_rows; i++)
    {
        delete[] B[i];
    }
    delete[] A;
    delete[] B;
    delete[] C;

    return 0;
}
