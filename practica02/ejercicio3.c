#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include<stdbool.h>
pthread_mutex_t mutex;
double total;

typedef struct object{
	double *array1;
	double *array2;
	int inicio;
	int numelementos;
}object;

void generardatos (double *array1, double *array2, int values){
	int i;
	for (i=0; i<values; i++) {
		array1[i] = (double)((rand()) % 1000000) / 10000;
		array2[i] = (double)((rand()) % 1000000) / 10000;
        printf("array1[%d]: %.4f\n", i, array1[i]);
        printf("array2[%d]: %.4f\n\n", i, array2[i]);
	}
}

void *productoescalar (void *parametro){
	object *d = (object *) parametro;
	double suma;
	suma = 0;

	for (int i=d->inicio; i<d->inicio+d->numelementos; i++){
		suma = suma + d->array1[i] * d->array2[i];

	}
	pthread_mutex_lock (&mutex);
	total = total + suma;
	pthread_mutex_unlock (&mutex);
}

int main (int argc, char *argv[]){
	int threads, values, ini, num, resto, rc;
	double minimo;
	object *datos;
	pthread_t *hilos;
	double *array1, *array2;
	bool flag = true;

    while(flag){
        printf("Numero de elementos:");
        scanf ("%d", &values);
        if(values < 1){
            printf("No me toques los huevos, valores > 0\n");
        }else{
            flag = false;
        }
    }

	if(values == 123){
		printf("\n ♥ ♥ ♥ Te queremos Juani ♥ ♥ ♥ \n\n");
	}

    flag = true;
    while(flag){
        printf("Numero de hilos a usar:");
        scanf ("%d", &threads);
        if(threads < 1){
            printf("No me toques los huevos, valores > 0\n");
        }else{
            flag = false;
        }
    }

	if(threads == 321){
		printf("\n(core dumped)\n\n");
		exit(1);
	}

    printf("--------1--------\n");
	printf ("Hilos utilizados: %d\n", threads);
	printf("Numero de elementos a repartir: %d\n", values);
	
	array1 = (double *) malloc (values * sizeof(double));
	array2 = (double *) malloc (values * sizeof(double));
	hilos = (pthread_t *) malloc (threads * sizeof (pthread_t));
	datos = (object *) malloc (threads * sizeof(object));

	srand(time(NULL));

	generardatos (array1, array2, values);
	
	printf("--------2--------\n");

	pthread_mutex_init(&mutex, NULL);

	ini = 0;
	num = values / threads;
	resto = values % threads;

	printf("Carga de elementos por cada hilo: %d\n", num);

	for (int i = 0; i<threads; i++){
		datos[i].array1 = &array1[0];
		datos[i].array2 = &array2[0];
		datos[i].inicio = ini;
		datos[i].numelementos = num;
		printf("El hilo %d muestra los elementos desde la posicion %d hasta la posicion %d\n", i, ini, num+ini);
		if (resto > 0){
			datos[i].numelementos++;
			resto--;
		}
		ini = ini + datos[i].numelementos;
		rc = pthread_create (&hilos[i], NULL, productoescalar, &datos[i]);
	}

	printf("--------3--------\n");

	for (int i=0; i<threads; i++){
		pthread_join (hilos[i], NULL);
	}

	printf ("\nEl producto escalar es: %.4f\n\n", total);

	pthread_mutex_destroy (&mutex);
	pthread_exit(NULL);

	free(array1);
	free(array2);
	free(hilos);
	free(datos);
}