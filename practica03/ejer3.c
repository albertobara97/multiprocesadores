#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>
#include<stdbool.h>

#define TAG 1

//Genera los valores aleatorios que le hayamos indicado por consola
void generarvalores (double *vector, int total){
	
    //Inicializa la semilla que genera los numeros aleatorios 
    //srand(time(NULL));

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

//Calcula el producto escalar del vector1 y el vector 2
double calcularProductoEscalar(double *vector1, double *vector2, int tamano){

    double productoEscalar = 0;
    //int tamano = sizeof(vector1) / sizeof(vector1[0]);
    printf("tamaño de los arrays: %d\n", tamano);
    printf("El proceso del producto escalar ha sido: ");
    for(int i = 0; i < tamano; i++){
        //resultado del producto escalar se suma al total
        productoEscalar += (vector1[i] * vector2[i]);
        if(i < tamano-1){
            printf(" %.2f ->", productoEscalar);
        }else{
            printf(" %.2f", productoEscalar);
        }
    }
    printf("\n\n");

    return productoEscalar;
}

int main (int argc, char *argv[]){
	
	int size, rank, total, reparto, resto, repartoLocal, inicioArray;
    double productoEscalar, resultado = 0, resultadoLocal;
    
    MPI_Status status;
    //Captura el parametro de la consola, que es la longitud del vector
    //sscanf (argv[1], "%d", &total);

    //Genera vectores
    double *vector1;
    double *vector2;

    MPI_Init (&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &size);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);

    if(size > 4){
        printf("Limitación de hardware, maximo 4 procesadores");
        MPI_Finalize();
        exit(-1);
    }

    //Si es el nucleo principal...
    if(rank == 0){
        
        bool flag = true;
        
        //Interactua con el usuario para preguntar por el parametro de los elementos del array
        while(flag){
        printf("Numero de elementos a generar para ambos arrays: \n");
        scanf ("%d", &total);
        if(total < 1){
            printf("Valores > 0\n");
        }else{
            flag = false;
        }
    }

        vector1 = (double *) malloc (sizeof(double) * total);
		vector2 = (double *) malloc (sizeof(double) * total);
              
        generarvalores(vector1, total);
        generarvalores(vector2, total);
        
        //Calcula el reparto de cada procesador
        reparto = total / (size-1);
        //Calcula el resto de la operacion anterior
        resto = total % (size-1);

        inicioArray = 0;
        
        //Para cada procesador (exceptuando el master)
        for(int i = 1; i<size; i++){

            //Reparto local se re-inicia en el bucle para no acumular los valores del resto
            repartoLocal = reparto;
            
            if(resto > 0){
                repartoLocal++;
                resto--;
            }

            MPI_Send (&repartoLocal, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
            MPI_Send (&vector1[inicioArray], repartoLocal, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD);
            MPI_Send (&vector2[inicioArray], repartoLocal, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD);
            
            inicioArray += repartoLocal;
        }

        //MPI_Recv(&productoEscalar, 1, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD, &status);

        for (int i = 1; i < size; i++)
		{
			MPI_Recv(&productoEscalar, 1, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD, &status);
			resultado = resultado + productoEscalar;
		}
        
        printf("Resultado final %.2f\n\n", resultado);
        
    }else{//Si no es el nucleo principal...

        MPI_Recv (&repartoLocal, 1, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD, &status);

        vector1 = (double *) malloc (sizeof(double) * repartoLocal);
		vector2 = (double *) malloc (sizeof(double) * repartoLocal);
        
        
        MPI_Recv (&vector1[0], repartoLocal, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD, &status);
        MPI_Recv (&vector2[0], repartoLocal, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD, &status);

        printf("Realizando producto escalar con el procesador %d\n", rank);
        srand(time(NULL));
        //Reliza el producto escalar de los vectores
        productoEscalar = calcularProductoEscalar(vector1, vector2, repartoLocal);

        MPI_Send(&productoEscalar, 1, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);

    }
    

    MPI_Finalize();
}