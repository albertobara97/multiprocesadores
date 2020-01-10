#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h> 
#include <stdbool.h>

#define TAG 1

int operacionesDisponibles(){
    
    bool flag = true;
    int operacion;
    while(flag){
            printf("operacion a realizar?\n\t1. Pasar una palabra a mayuscula\n\t2. Calcular suma de un vector y la raiz cuadrada de la suma \n\t3. Calcular los enteros correspondientes de una frase\n\t4. Realizar todas las operaciones anteriores\n");
            scanf ("%d", &operacion);
            //printf("Alto de la matriz? \n");
            //scanf("%d", &alto);
            if(operacion < 1 || operacion < 1){
                printf("Debes de escoger entre 0 y 4\n");
            }else{
                flag = false;
            }
        }flag = true;
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
        case 0:
            operacion = operacionesDisponibles();

            /*for (i=1; i<size; i++){
				MPI_Send (&operacion, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
			}*/

            switch(operacion){
                case 0: printf("\nSaliendo del programa...\n\n");
                MPI_Finalize();
                exit(0);
				break;

                case 1: printf("Voy a decirle al procesador 1 que te pregunte una letra que luego pasaré a mayuscula\n\n");
                break;

                case 2: printf("voy a enviarle procesador 2 un vector con 10 numeros reales y el ya hara algunos calculos\n\n");
                break;

                case 3: printf("Voy a enviarle una señal al procesador 3 para que el ya haga sus cosas\n\n");
                break;

                case 4: printf("Ea, Todos los procesadores a trabajar. Yo me quedo aqui mirando como trabajais\n\n");
                break;
            }

    }
    
    MPI_Finalize();
}