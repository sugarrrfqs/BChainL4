#include <iostream>
#include "mpi.h"

using namespace std;
/*
int m[4][4] = {     { 1,  2,  3,  4},
                    { 5,  6,  7,  8},
                    { 9, 10, 11, 12},
                    {13, 14, 15, 16}};
*/

int mSize = 4;

int main()
{
    MPI_Init(NULL, NULL);

    int processID;
    MPI_Comm_rank(MPI_COMM_WORLD, &processID);
    int numProc;
    MPI_Comm_size(MPI_COMM_WORLD, &numProc);

    if (numProc > 1)
    {
        int blockSize = mSize / (numProc - 1);
        if (processID == 0)
        {
            //int** m = new int* [mSize];
            //for (int i = 0; i < mSize; i++)
            //{
            //    m[i] = new int[mSize];
            //}
            int m[4][4] = { { 1,  2,  3,  4},
                        { 5,  6,  7,  8},
                        { 9, 10, 11, 12},
                        {13, 14, 15, 16} };
            int* m1List = new int[mSize * mSize],
                * m2List = new int[mSize * mSize],
                * resultList = new int[mSize * (mSize + blockSize)],
                ** result = new int* [mSize];
            for (int i = 0; i < mSize; i++)
            {
                result[i] = new int[mSize];
            }

            for (int i = 0; i < mSize; i++)
            {
                for (int j = 0; j < mSize; j++)
                {
                    m2List[mSize * i + j] = m[i][j];
                }
            }

            for (int i = 0; i < (numProc - 1); i++)
            {
                for (int j = blockSize * i; j < blockSize * (i + 1); j++)
                {
                    for (int k = 0; k < mSize; k++)
                    {
                        m1List[j * mSize + k] = m[j][k];
                    }
                }

                int* bfStart = m1List + i * mSize * blockSize;
                MPI_Send(bfStart,
                    mSize * blockSize,
                    MPI_INT,
                    i + 1,
                    0,
                    MPI_COMM_WORLD);

                bfStart = m2List;
                MPI_Send(bfStart,
                    mSize * mSize,
                    MPI_INT,
                    i + 1,
                    0,
                    MPI_COMM_WORLD);
            }

            int* filler = new int[mSize * blockSize];
            for (int i = 0; i < mSize * blockSize; i++) filler[i] = 0;

            MPI_Gather(filler, mSize * blockSize, MPI_INT,
                resultList, mSize * blockSize, MPI_INT,
                0, MPI_COMM_WORLD);

            for (int j = 0; j < mSize; j++)
            {
                for (int k = 0; k < mSize; k++)
                {
                    result[j][k] = resultList[(blockSize + j) * mSize + k];
                }
            }
            cout << "Otvet:\n";
            for (int i = 0; i < mSize; i++)
            {
                for (int j = 0; j < mSize; j++)
                {
                    cout << result[i][j] << " ";
                }
                cout << "\n";
            }

        }
        else
        {
            int* m1List = new int[blockSize * mSize],
                * m2List = new int[mSize * mSize],
                ** m1 = new int* [blockSize],
                ** m2 = new int* [mSize],
                * resultList = new int[blockSize * mSize],
                ** result = new int* [blockSize];

            for (int i = 0; i < blockSize; i++)
            {
                result[i] = new int[mSize];
                m1[i] = new int[mSize];
            }
            for (int i = 0; i < mSize; i++)
            {
                m2[i] = new int[mSize];
            }

            MPI_Recv(m1List,
                mSize * blockSize,
                MPI_INT,
                0,
                0,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);
            MPI_Recv(m2List,
                mSize * mSize,
                MPI_INT,
                0,
                0,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);

            //printf("Process %d\n", processID);
            //for (int i = 0; i < mSize * blockSize; i++) cout << m1List[i] << " ";
            //cout << "\n";
            //for (int i = 0; i < mSize * mSize; i++) cout << m2List[i] << " ";
            //cout << "\n";

            for (int i = 0; i < blockSize; i++)
            {
                for (int j = 0; j < mSize; j++)
                {
                    m1[i][j] = m1List[i * mSize + j];
                }
            }
            for (int i = 0; i < mSize; i++)
            {
                for (int j = 0; j < mSize; j++)
                {
                    m2[i][j] = m2List[i * mSize + j];
                }
            }
            for (int i = 0; i < blockSize; i++)
            {
                for (int j = 0; j < mSize; j++)
                {
                    result[i][j] = 0;
                }
            }

            for (int i = 0; i < blockSize; i++)
            {
                for (int j = 0; j < mSize; j++)
                {
                    for (int k = 0; k < mSize; k++)
                    {
                        result[i][j] += m1[i][k] * m2[k][j];
                    }
                }
            }

            for (int i = 0; i < blockSize; i++)
            {
                for (int j = 0; j < mSize; j++)
                {
                    resultList[i * mSize + j] = result[i][j];
                }
            }

            int* bfStart = resultList;
            int* filler = new int[mSize * blockSize];
            for (int i = 0; i < mSize * blockSize; i++) filler[i] = 0;

            MPI_Gather(bfStart, 
                mSize * blockSize, 
                MPI_INT,
                filler, 
                mSize * blockSize, MPI_INT,
                0, 
                MPI_COMM_WORLD);
            //for (int i = 0; i < blockSize; i++)
            //{
            //    for (int j = 0; j < mSize; j++)
            //    {
            //        cout << result[i][j] << " ";
            //    }
            //    cout << "\n";
            //}
        }
    }
    MPI_Finalize();
}
