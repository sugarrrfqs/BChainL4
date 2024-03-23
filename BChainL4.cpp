#include <iostream>
#include "mpi.h"

using namespace std;
/*
int m[4][4] = {     { 1,  2,  3,  4},
                    { 5,  6,  7,  8},
                    { 9, 10, 11, 12},
                    {13, 14, 15, 16}};
*/

int main()
{
    MPI_Init(NULL, NULL);

    int processID;
    MPI_Comm_rank(MPI_COMM_WORLD, &processID);
    int numProc;
    MPI_Comm_size(MPI_COMM_WORLD, &numProc);
    int blockLenght = numProc / 2;

    if (processID == 0)
    {
        int m[4][4] = {     { 1,  2,  3,  4},
                            { 5,  6,  7,  8},
                            { 9, 10, 11, 12},
                            {13, 14, 15, 16}};
        int m1List[16], m2List[16];
        int k = 0;      
        for (int b = 0; b < 4; b++)
        {
            for (int i = (b / blockLenght) * blockLenght; i < (b / blockLenght) * blockLenght + blockLenght; i++)
            {
                for (int j = (b % blockLenght) * blockLenght; j < (b % blockLenght) * blockLenght + blockLenght; j++)
                {
                    m1List[k] = m[i][j];
                    k++;
                }
            }
        }
        k = 0;
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                m2List[k] = m[i][j];
                k++;
            }
        }

        for (int i = 1; i <= 4; i++)
        {
            int* bfStart = m1List + blockLenght * blockLenght * (i - 1);
            MPI_Send(bfStart,
                blockLenght * blockLenght,
                MPI_INT,
                i,
                0,
                MPI_COMM_WORLD);

            bfStart = m2List;
            MPI_Send(bfStart,
                16,
                MPI_INT,
                i,
                0,
                MPI_COMM_WORLD);
        }
    }
    else
    {
        int m1List[4], m2List[16], result[2][2], m1[2][2], m2[4][4];
        MPI_Recv(m1List,
            4,
            MPI_INT,
            0,
            0,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE);
        MPI_Recv(m2List,
            16,
            MPI_INT,
            0,
            0,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE);

        printf("Hello World! Kind regards, Process %d\n", processID);
        for (int i = 0; i < 4; i++) cout << m1List[i] << " ";
        cout << "\n";
        for (int i = 0; i < 16; i++) cout << m2List[i] << " ";
        cout << "\n";
        int k = 0;
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                m1[i][j] = m1List[k];
                k++;
            }
        }
        k = 0;
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                m2[i][j] = m2List[k];
                k++;
            }
        }
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                result[i][j] = 0;
            }
        }

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					result[i][j] += m1[i][k] * m2[k][j];
				}
			}
		}
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                cout << result[i][j] << " ";
            }
            cout << "\n";
        }
    }
    

    MPI_Finalize();
}
