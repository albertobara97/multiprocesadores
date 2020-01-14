#include "mpi.h"
#include <stdio.h>
#include <stdlib.h> 
int main(int argc, char *argv[])
{

	int rank, size, suma, tam;
    MPI_Init (&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	int * vec=0;
    
    MPI_Status status;
    
    switch(rank){
        case 0: printf("[Proceso %d]\n", rank);
        //Inicializo tam en rank 0 para que tenga sentido enviarla a los demas procesos
        tam = 10;
        vec[tam];
        int *vectorLocal;
		vec = malloc(sizeof(int)*tam);
        printf("[Proceso %d] relleno vector vec: \n", rank);
        for(int i = 0; i<tam; i++){
            if((i != 4 && i !=tam-1) || i == 0){
                printf("vec[%d] = %d | ", i, i);
            }else{
                printf("vec[%d] = %d\n", i, i);
            }
            
            vec[i] = i;
        } 

        printf("[Proceso %d] Hago broadcast de tam...\n", rank);
        for(int i = 1; i<size; i++){
            printf("[Proceso %d] enviando tam a proceso %d\n", rank, i);
            MPI_Send(&tam,1,MPI_INT,i,123,MPI_COMM_WORLD);
        }
        printf("[Proceso %d] Envio los primeros 5 numeros del vector al proceso 1\n", rank);
        MPI_Send(&vec[0],5,MPI_INT,1,123,MPI_COMM_WORLD);

        printf("[Proceso %d] Envio los ultimos 5 numeros del vector al proceso 2\n", rank);
        MPI_Send(&vec[5],5,MPI_INT,2,123,MPI_COMM_WORLD);

        break;

        case 1: printf("[Proceso %d]\n", rank);
        MPI_Recv(&tam,1,MPI_INT,0,123,MPI_COMM_WORLD, &status);
        printf("[Proceso %d] He recibido tam (%d)\n", rank, tam);
        
        vectorLocal = (int *) malloc (sizeof(int) * (tam/2));
        MPI_Recv(&vectorLocal[0],5,MPI_INT,0,123,MPI_COMM_WORLD, &status);
        printf("[Proceso %d] He recibido un vector de 5 numeros\n", rank);

        suma = 0;
        printf("[Proceso %d] Proceso de suma: ", rank);
        for(int i = 0; i<5; i++){
            suma += vectorLocal[i];
            if(i < 4){
                printf("%d | ", suma);
            }else{
                printf("%d  <= resultado final\n", suma);
            }
        }

        printf("[Proceso %d] Le envio el resultado final al procesador 3\n", rank);
        MPI_Send(&suma,1,MPI_INT,3,123,MPI_COMM_WORLD);
        break;

        case 2: printf("[Proceso %d]\n", rank);
        MPI_Recv(&tam,1,MPI_INT,0,123,MPI_COMM_WORLD, &status);
        printf("[Proceso %d] He recibido tam (%d)\n", rank, tam);
        
        vectorLocal = (int *) malloc (sizeof(int) * (tam/2));
        MPI_Recv(&vectorLocal[0],5,MPI_INT,0,123,MPI_COMM_WORLD, &status);
        printf("[Proceso %d] He recibido un vector de 5 numeros\n", rank);

        //printf("[Proceso %d] Estoy parado...\n", rank);
        break;

        case 3: printf("[Proceso %d]\n", rank);
        MPI_Recv(&tam,1,MPI_INT,0,123,MPI_COMM_WORLD, &status);
        
        printf("[Proceso %d] He recibido tam (%d)\n", rank, tam);

        MPI_Recv(&suma,1,MPI_INT,1,123,MPI_COMM_WORLD, &status);
        printf("[Proceso %d] El resutlado de la suma es: %d\n", rank, suma);
        //printf("[Proceso %d] Estoy parado...\n", rank);
        break;

        default: printf("Estoy parado...\n");
    }

	/*if(rank==0) {
        printf("size: %d", size);
		vec = malloc(sizeof(int)*5);
        
		for(int i = 0; i<5; i++) vec[i] = i;
        
        for(int i = 0; i<size; i++){
            printf("entro");
            MPI_Send(&tam,1,MPI_INT,i,123,MPI_COMM_WORLD);
        }
        
		MPI_Send(vec,5,MPI_INT,1,123,MPI_COMM_WORLD);
        
 	}else{
		MPI_Status status;
		vec = malloc(sizeof(int)*5);
        MPI_Recv(&tam,1,MPI_INT,0,123,MPI_COMM_WORLD, &status);
        printf("tam : %d", tam);
		MPI_Recv(vec,5,MPI_INT,0,123,MPI_COMM_WORLD, &status);
		for(int i=0; i<5; i++)
			printf("%i \n", vec[i]);

	}*/
	MPI_Finalize();
	return 0;
}