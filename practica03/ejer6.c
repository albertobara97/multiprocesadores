#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h> 

#define TAG 1

int operacionesDisponibles(){

    printf("\nLlego aqui\n");
    int operacion;
    printf("Elija una operación: \n");
    scanf ("%i", &operacion);

    if(operacion<0 && operacion>4){

        printf("Operación %i no válida.", &operacion);
        exit(-1);
    }else
    {
        printf("Número entre 0 y 4 ");
    }
    return operacion;
}


int main (int argc, char *argv[]){

    int size, rank, operacion, i;
    double suma;
    MPI_Status status;

	MPI_Init (&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &size);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);

    if (size != 4){
    
        if (rank == 0){
            printf ("\nRequiere de 4 procesadores para su ejecución.\n\n");
        }
        MPI_Finalize();
        exit(-1);
    }
    switch (rank){
        case 0: do{
            operacion = operacionesDisponibles();

            for (i=1; i<size; i++)
			{
				MPI_Send (&operacion, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
			}

            switch(operacion){
                case 0: printf("\nSaliendo del programa...\n");
				break;
            }








        }while (operacion != 0);
        break;
    }
    
    MPI_Finalize();
}