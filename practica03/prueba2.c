#include "mpi.h"
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>

#define TAG 123

void generarvalores (int *vec, int total){
	
    //Semilla números aleatorios
    srand(time(NULL));
	total =10;
    printf("Tamaño vector: %d\n", total);
	
    //Rellena el vector
    for (int i=0; i<total; i++) {

		vec[i] = (int)((rand()) % 10000) / 100;
		
        //Imprime por pantalla conforme se va rellenando
        if((i%100 == 0 || i == total-1) && i != 0 ){
            printf("%d\n\n", vec[i]);
        }else{
            printf("%d  |   ", vec[i]);
        }

	}
}

int main(int argc, char *argv[])
{
    int rank, size, i, total, mitadVec, suma, tam = 10;
	
	MPI_Init (&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &size);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);

	int * vec=0;
	if(rank==0) {
		printf("______Llego aquí______\n");
		printf("Procesador %d\n", rank);
		vec[tam];
        vec = (int *) malloc (sizeof(int) * tam);
		//Genera los valores de vec
		printf("Vector: ");
		generarvalores(vec, total);

		/*for (int i=0; i<tam; i++) {
			for(int i = 0; i<tam; i++) vec[i] = i;
			}*/

		for(int i = 1; i<size; i++){
            printf("He llegado aquí.\n");
            MPI_Send(&tam,1,MPI_INT,i,TAG,MPI_COMM_WORLD);
        }
		printf("Envio la mitad del vector\n");
		mitadVec = tam/2;
		printf("Valor de mitadVec = %d\n", mitadVec);
		MPI_Send(&vec[0], mitadVec, MPI_INT, 1, TAG, MPI_COMM_WORLD);
		MPI_Send(&vec[5], mitadVec, MPI_INT, 2, TAG, MPI_COMM_WORLD);
		
		/*reparto = tam / (size-1);

        //resto = total % (size-1);

        inicioVec = 0;
        
        for(int i = 1; i<size; i++){

            repartoLocal = reparto;

            MPI_Send (&repartoLocal, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
            MPI_Send (&vec[inicioVec], repartoLocal, MPI_INT, i, TAG, MPI_COMM_WORLD);
            
            inicioVec += repartoLocal;
        }*/

 	}	
	else{
		MPI_Status status;
		if(rank==1){
			printf("Seleccionado procesador 1\n\n");
			MPI_Recv(&tam, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
        	printf("Recibo tamaño = %d\n", tam);
			mitadVec = tam/2;
			vec = (int *) malloc (sizeof(int) * mitadVec);
			MPI_Recv (&vec[0], mitadVec, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
			printf("Procesador %d recibe %d\n", rank, mitadVec);

			//Suma
			suma = 0;
			for(int i = 0; i<mitadVec; i++){
				
				suma = suma + vec[i];
			}
			printf("Suma = %d\n", suma);
			MPI_Send(&suma, 1, MPI_INT, 3, TAG, MPI_COMM_WORLD);

		}else{if(rank==2){
				printf("Seleccionado procesador 2\n\n");
				MPI_Recv(&tam, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
        		printf("Recibo tamaño = %d\n", tam);
				mitadVec = tam/2;
				vec = (int *) malloc (sizeof(int) * mitadVec);
				MPI_Recv (&vec[5], mitadVec, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
				printf("Procesador %d recibe %d\n", rank, mitadVec);
			}else{
				printf("Seleccionado procesador 3\n\n");
				MPI_Recv(&suma, 1, MPI_INT, 1, TAG, MPI_COMM_WORLD, &status);
				printf("Recibo suma = %d\n", suma);

			}

		}
	}
	MPI_Finalize();
	return 0;
}

