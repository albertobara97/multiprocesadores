#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include<stdbool.h>


typedef struct object{
	double *array;
	int inicio;
	int elementosHilo;
	double min;
}object;

void generardatos (double *array, int numElementos){
	int i;

	for (i=0; i<numElementos; i++) {
		array[i] = (double)((rand()) % 1000000) / 10000;
        printf("array[%d] = %f\n", i, array[i]);
	}
}

void *calcularminimo (void *parametro){
	int i;
	object *d = (object *) parametro;

	d->min = d->array[d->inicio];
	for (i=d->inicio+1; i<d->inicio+d->elementosHilo; i++){
		if (d->array[i] < d->min){
			d->min = d->array[i];
            printf("minLocal = %.4f\n", d->array[i]);
        }
	}
}

int main (int argc, char *argv[])
{
	int numThreads, numElementos, ini, num, resto, rc;
	double minimo;
	object *datos;
	pthread_t *threads;
	double *array;
    bool flag = true;

    while(flag){
        printf("Número de hilos: ");
        scanf ("%d", &numThreads);
        if(numThreads < 1){
            printf("valores > 0\n");
        }else{
            flag = false;
        }
    }
    flag = true;
    while(flag){
        printf("Número de elementos: ");
        scanf ("%d", &numElementos);
        if(numElementos < 1){
            printf("valores > 0\n");
        }else{
            flag = false;
        }
    }
    printf("--------1--------\n");
	array = (double *) malloc (numElementos * sizeof(double));
	threads = (pthread_t *) malloc (numThreads * sizeof (pthread_t));
	datos = (object *) malloc (numThreads * sizeof(object));
	
    srand (time(NULL));
	generardatos (array, numElementos);

	ini = 0;
	num = numElementos / numThreads;
	resto = numElementos % numThreads;

    printf("--------2--------\n");

	for (int i=0; i<numThreads; i++){
		datos[i].array = &array[0];
		datos[i].inicio = ini;
		datos[i].elementosHilo = num;
		if (resto > 0){
			datos[i].elementosHilo++;
			resto--;
		}
		printf("El hilo %d muestra los elementos desde la posicion %d hasta la posicion %d\n", i, ini, datos[i].elementosHilo+ini);
		ini = ini + datos[i].elementosHilo;
		rc = pthread_create (&threads[i], 0, calcularminimo, &datos[i]);
	}

	for (int i=0; i<numThreads; i++){
		pthread_join (threads[i], 0);
		if (i == 0)
			minimo = datos[i].min;
		else{
			if (datos[i].min < minimo)
				minimo = datos[i].min;
		}
	}
    printf("--------3--------\n");

	printf ("El menor de los elementos es %.4lf\n", minimo);

	free (array);
	free (threads);
	free (datos);
}