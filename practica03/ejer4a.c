#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>
#include<stdbool.h>
#include<string.h>

#define TAG 1

//Genera los valores aleatorios que le hayamos indicado por consola
void generarvalores (int **matriz, int ancho, int alto){
	
    //Inicializa la semilla que genera los numeros aleatorios 
    //srand(time(NULL));

    printf("Matriz a autogenerar de %dx%d\n", ancho, alto);
    
    char *visual;
    visual = (char *) malloc (sizeof (char) * (alto*alto));
    
    //Rellena la matriz
    for (int i = 0; i < alto; i++) {

        for(int j = 0; j < ancho; j++){
            
            strcat(visual, "-----");
            matriz[i][j] = (int)((rand()) % 10000) / 100;
            
            //Imprime por pantalla conforme se va rellenando
            if(j == ancho-1){
                
                printf("%d\n%s\n", matriz[i][j], visual);
            }else{
                printf("%d  |   ", matriz[i][j]);
            }

        }
        strcpy(visual, "-----");
		
	}
}

int main (int argc, char *argv[]){

    int size, rank, ancho, alto, reparto, resto, repartoLocal, inicioArray;
    double productoEscalar, resultado = 0, resultadoLocal;
    
    MPI_Status status;
    //Captura el parametro de la consola, que es la longitud del vector
    //sscanf (argv[1], "%d", &total);

    //Genera vectores
    int **matriz;

    MPI_Init (&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &size);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);

    if(size != 4){
        printf("Requeridos 4 procesadores.");
        MPI_Finalize();
        exit(-1);
    }

    //Si es el nucleo principal...  
    if(rank == 0){
        
        bool flag = true;
        
        //Interactua con el usuario para preguntar por el parametro de los elementos del array
        while(flag){
            printf("Ancho de la matriz? \n");
            scanf ("%d", &ancho);
            //printf("Alto de la matriz? \n");
            //scanf("%d", &alto);
            if(ancho < 1 /*|| alto < 1*/){
                printf("Valores > 0\n");
            }else{
                flag = false;
            }
        }flag = true;

        while(flag){
            printf("Alto de la matriz? \n");
            scanf ("%d", &alto);
            if(alto < 1){
                printf("Valores > 0\n");
            }else{
                flag = false;
            }
        }

        if(ancho > alto){
            matriz = (int **) malloc (sizeof(int*) * ancho);
        }else{
            matriz = (int **) malloc (sizeof(int*) * alto);
        }

        for (int i = 0; i < alto; i++){
		matriz[i] = (int *) malloc (sizeof (int) * alto);
	    }
        
        srand(time(NULL));
        generarvalores(matriz, ancho, alto);
        
        reparto = ancho / size;
        resto = ancho % size;

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
            MPI_Send (&matriz[inicioArray][0], repartoLocal, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD);
            
            inicioArray += repartoLocal;
        }

    }else{

        MPI_Recv (&repartoLocal, 1, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD, &status);

        vector = (int *) malloc (sizeof(double) * repartoLocal);
		
        
        MPI_Recv (&vector1[0], repartoLocal, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD, &status);
        MPI_Recv (&vector2[0], repartoLocal, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD, &status);

    }

    MPI_Finalize();
}