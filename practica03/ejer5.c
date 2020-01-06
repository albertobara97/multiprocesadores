#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>

//#define TAG 
#define MAX 10

int main (int argc, char *argv[])
{
    int TAG;
	int vec[MAX];
	int size, rank, n, i;
	MPI_Status status;
    srand (time(NULL));
	MPI_Init (&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &size);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);

	if (size > 2){
		
        if (rank == 0){
			printf ("\nSe necesitan solo 2 procesadores aunque se hayan habilitado %d\n\n", size);
		}
	}else if(size == 1){
        printf("Seleccionado solo 1 procesador para la ejecución. Selecciona 2 o más\n");
        MPI_Finalize();
        exit(-1);
    }

	if (rank == 0)
	{
		for (n=1; n<=MAX; n++)
		{
            TAG = rand();
            printf("TAG a enviar %d\n", TAG);
			MPI_Send (&n, 1, MPI_INT, 1, TAG, MPI_COMM_WORLD);
		}
		MPI_Recv (&vec[0], MAX, MPI_INT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        TAG = status.MPI_TAG;
        printf("TAG de array: %d\n", TAG);
		printf ("\nCuadrados de los %d primeros numeros naturales\n", MAX);
		for (i=0; i<MAX; i++)
		{
			printf ("Valor: %d\n", vec[i]);
		}
	}
	else if(rank == 1){
		for (i=0; i<MAX; i++){
			MPI_Recv (&n, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            TAG = status.MPI_TAG;
            printf("\t\t\t\t\tTAG recibido: %d\n", TAG);
			vec[i] = n*n;
		}
        TAG = rand();
		MPI_Send (&vec[0], MAX, MPI_INT, 0, TAG, MPI_COMM_WORLD);
	}
	MPI_Finalize();
}