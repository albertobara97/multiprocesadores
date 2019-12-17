#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int* crearVectorAleatorio(int tam){
	int* vec = malloc(sizeof(int)*tam);
	for(int i = 0; i<tam; i++)
		vec[i] = rand() % 66;
	return vec;
}

typedef struct tarea{
	int* vector;
	int ini;
	int fin;
} tarea;

int resultadoGlobal = 0;
pthread_mutex_t mutex;

int seqCheck(const int* vec, int tam){
	int res = 0;
	for(int i = 0; i<tam; i++)
		res += vec[i];
	return res;
}

void* sumaPar(void* args){
	tarea* deberes = (tarea*) args;
	int resultadoLocal = 0;
	for(int i = deberes->ini; i<deberes->fin; i++){
		resultadoLocal += (deberes->vector)[i];
	}
	pthread_mutex_lock(&mutex);
	resultadoGlobal += resultadoLocal;
	pthread_mutex_unlock(&mutex);
}

int main(int argc, char* argv[]){
	if(argc != 3){
		printf("Llamada: ./prog Longitud nHilos\n");
	}else{
		int longitud = atoi(argv[1]);
		int nHilos = atoi(argv[2]);
		int* vector = crearVectorAleatorio(longitud);

		tarea* deberes = malloc(sizeof(tarea)*nHilos);
		pthread_t* hilos = malloc(sizeof(pthread_t)*nHilos);
		pthread_mutex_init(&mutex, 0);

		int pack = longitud / nHilos;
		int offset = longitud % nHilos;
		for(int i = 0; i<nHilos; i++){//Alerta... no estamos gestionando el excedente aun...
			deberes[i].vector = vector;
			deberes[i].ini = i*pack;
			deberes[i].fin = i*pack + pack;
			pthread_create(&hilos[i], 0, sumaPar, &deberes[i]);		
		}
		for(int i = 0; i<nHilos; i++){
			pthread_join(hilos[i], 0);
		}
		printf("Resultado Paralelo: %d\n", resultadoGlobal);		
		printf("Resultado Secuencial: %d\n", seqCheck(vector, longitud));		

		free(hilos);
		free(deberes);
		free(vector);
		pthread_mutex_destroy(&mutex);
	}
	return 0;
}
