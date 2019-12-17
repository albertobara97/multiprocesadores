#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include<stdbool.h>
int numFilas;
typedef struct object
{
	double **matriz;
	int columnaInicio;
	int numColumnas;
	double suma;
}object;

void generardatos (double **matriz, int filas, int columnas)
{
	int i, j;

	for (i=0; i<filas; i++) 
	{
		for (j=0; j<columnas; j++)
		{
			matriz[i][j] = (double)((rand()) % 1000000) / 10000;;
            printf("matrix[%d][%d] = %.4f\t", i, j, matriz[i][j]);
		}
        printf("\n");
	}
}

void *sumarmatriz (void *parametro){
	int i, j;
	object *dato = (object*) parametro;
	
	dato->suma = 0;
	for (i=0; i<numFilas; i++){
		for (j=dato->columnaInicio; j<dato->columnaInicio+dato->numColumnas; j++){
			dato->suma = dato->suma + dato->matriz[i][j];
		}
	}
	return &dato->suma;
}

int main (int argc, char *argv[]){
	int numThreads, filas, i, inicio, numColumnas, resto;
	double **matriz;
	object *datos;
	pthread_t *hilos;
	double sumatotal;
    bool flag = true;

    while(flag){
        printf("Numero de hilos a usar:");
        scanf ("%d", &numThreads);
        if(numThreads < 1){
            printf("valores > 0\n");
        }else{
            flag = false;
        }
    }
    flag = true;

    while(flag){
        printf("Numero de filas de la matriz:");
        scanf ("%d", &filas);
        if(filas < 1){
            printf("valores > 0\n");
        }else{
            flag = false;
        }
    }
    flag = true;

    while(flag){
        printf("Numero de columnas de la matriz:");
        scanf ("%d", &numColumnas);
        if(numColumnas < 1){
            printf("valores > 0\n");
        }else{
            flag = false;
        }
    }
    printf("--------1--------\n");
	srand (time(NULL));

	matriz = (double **) malloc (sizeof(double*) * filas);
    numFilas = filas;
	for (i=0; i<filas; i++){
		matriz[i] = (double *) malloc (sizeof (double) * numColumnas);
	}
    printf("--------2--------\n");

	generardatos (matriz, filas, numColumnas);

    printf("--------3--------\n");

	hilos = (pthread_t *) malloc (numThreads * sizeof (pthread_t));
	datos = (object *) malloc (numThreads * sizeof(object));
    inicio = 0;
	numColumnas = numColumnas / numThreads;
	resto = numColumnas % numThreads;

	for (i=0; i<numThreads; i++){
		datos[i].matriz = matriz;
		datos[i].columnaInicio = inicio;
        printf("\nnumFilas %d\n", numFilas);
        printf("columnaInicio thread %d : %d\n", i, inicio);
		datos[i].numColumnas = numColumnas;
        printf("columnaFinal thread %d : %d\n", i, (inicio+numColumnas));
	
		if (resto > 0){
			datos[i].numColumnas++;
			resto--;
		}
		printf("\nLas columnas que muestra el hilo %d son de la columna %d a la columna %d\n", i, inicio, datos[i].numColumnas+inicio);
		inicio = inicio + datos[i].numColumnas;
		pthread_create (&hilos[i], 0, sumarmatriz, &datos[i]);
	}	

	sumatotal = 0;
	for (i=0; i<numThreads; i++){
		pthread_join (hilos[i], 0);
		sumatotal = sumatotal + datos[i].suma;
	}
    
	printf ("\nSuma total = %.4f\n", sumatotal);
    printf("--------4--------\n");

	for (i=0; i<filas; i++){
		free (matriz[i]);
	}

	free (matriz);
	free (hilos);
	free (datos);
}
