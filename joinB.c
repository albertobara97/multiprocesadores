#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define LOCAL_RES_SIZE 4

void* funcionHebrada(void* arg){
	int id = *((int*) arg);
	int* local_res = malloc(sizeof(int)*LOCAL_RES_SIZE);
	for(int i = 0; i<LOCAL_RES_SIZE; i++){
		local_res[i] = id;
	}
	return local_res;//pthread_exit(local_res);
}

int main(int argc, char* argv[]){
	if(argc!=2){
		printf("Llamada: ./prog num_hilos\n");
	}else{
		int nHilos = atoi(argv[1]);
		pthread_t* hilos = malloc(sizeof(pthread_t)*nHilos);
		int* args = malloc(sizeof(int)*nHilos);
		for(int i = 0; i<nHilos; i++){
			args[i] = i;
			pthread_create(&hilos[i], 0, funcionHebrada, &args[i]);
		}
		void* TMPres = 0; 
		int* resHilo = 0;
		for(int i = 0; i<nHilos; i++){
			pthread_join(hilos[i], &TMPres);
			resHilo = (int*) TMPres;
			for(int j = 0; j<LOCAL_RES_SIZE; j++){
				printf("%d ", resHilo[j]);
			}
			printf("\n");
			free(resHilo);
		}
		free(args);
		free(hilos);
	}
}
