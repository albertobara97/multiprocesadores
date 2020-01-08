#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>
#include<stdbool.h>

#define TAG 1

//Genera los valores aleatorios que le hayamos indicado por consola
void generarvalores (double *vector, int total){
	
    //Inicializa la semilla que genera los numeros aleatorios 
    srand(time(NULL));

    printf("Numeros generados. %d\n", total);

    //Rellena el vector
    for (int i=0; i<total; i++) {

		vector[i] = (double)((rand()) % 10000) / 100;
		
        //Imprime por pantalla conforme se va rellenando
        if((i%10 == 0 || i == total-1) && i != 0 ){
            printf("%.2f\n\n", vector[i]);
        }else{
            printf("%.2f  |   ", vector[i]);
        }

	}
}

//Busca el mínimo de un vector que se le pasa por parametro
double buscarMinimo(double *vector, int total){
    //Valor máximo para evitar errores
    double minimo = __INT_MAX__;

    //recorre el vector en busca del valor minimo
    for(int i = 0; i<total; i++){
        //Cada vez que encuentra un valor menor que min, lo actualiza con este
        if(vector[i] < minimo){
            minimo = vector[i];
        }
    }

    return minimo;
}

int main (int argc, char *argv[]){
	
	int size, rank, i, total;
    double min;
    bool flag = true;
    MPI_Status status;
    //Captura el parametro de la consola, que es la longitud del vector
    sscanf (argv[1], "%d", &total);

    double vector[total];

    MPI_Init (&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &size);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);

    if(size != 2){
        printf("Este programa debe ejecutarse con 2 nucleos.");
        MPI_Finalize();
        exit(-1);
    }

    //Si es el nucleo principal...
    if(rank == 0){
                
        generarvalores(vector, total);
        MPI_Send (vector, total, MPI_DOUBLE, 1, TAG, MPI_COMM_WORLD);
        

        
    }else{//Si no es el nucleo principal...

        MPI_Recv (vector, total, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD, &status);
        
        //Llama al metodo buscarMinimo y guarda el minimo en min
        min = buscarMinimo(vector, total);

        printf("El valor mínimo es %.2f\n\n", min);

    }
    

    MPI_Finalize();
}