#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "pgm.h"
#include<stdbool.h>

#define tamanoNucleo 3

typedef struct{
	unsigned char **original;
	int **nucleo;
	int ancho;
	int alto;
	int k;
}tipoDato;

typedef struct{
	tipoDato *datos;
	unsigned char **salida;
	int filainicio;
	int numfilas;
}hilo;

void *convolucion(void *parametro){
  int x, y, suma, i, j, f;

  hilo *h = (hilo *) parametro;

  printf ("\nFila inicio %d  y total %d", h->filainicio, h->numfilas);
  f = h->filainicio;
  for (x = 0; x < h->numfilas; x++){
    for (y = 1; y < h->datos->alto-1; y++){
      suma = 0;
      for (i = 0; i < tamanoNucleo; i++){
        for (j = 0; j < tamanoNucleo; j++){
            suma = suma + h->datos->original[(f-1)+i][(y-1)+j] * h->datos->nucleo[i][j];
        }
      }
      if(h->datos->k==0){
        h->salida[x][y] = suma;
      }else{
        h->salida[x][y] = suma/h->datos->k;
      }
    }
    f++;
  }
}

/* * * * *          * * * * *          * * * * *          * * * * */

int main(int argc, char *argv[]){
  int numhilos, i, j, n, fila, numerofilas, excedente;
  unsigned char** salida;
  hilo *structHilos;
  pthread_t *mispthread;
  tipoDato variables;
  bool flag  =true;

  while(flag){
        printf("Numero de hilos a usar: ");
        scanf ("%d", &numhilos);
        if(numhilos < 1){
            printf("Valores > 0\n");
        }else{
            flag = false;
        }
    }

  variables.original = pgmread("lena_original.pgm", &variables.ancho, &variables.alto);
  salida = (unsigned char**)GetMem2D(variables.ancho, variables.alto, sizeof(unsigned char));

  variables.nucleo = (int**) GetMem2D(tamanoNucleo, tamanoNucleo, sizeof(int));
  for (i = 0; i < tamanoNucleo; i++){
    for (j = 0; j < tamanoNucleo; j++){
      variables.nucleo[i][j] = -1;
    }
  }variables.nucleo[tamanoNucleo/2][tamanoNucleo/2] = 1;
  
  variables.k = 0;
  
  for (i = 0; i < tamanoNucleo; i++){
    for (j = 0; j < tamanoNucleo; j++){
      variables.k = variables.k + variables.nucleo[i][j];
    }
  }
  
  structHilos = (hilo *) malloc (sizeof(hilo) * numhilos);
  mispthread = (pthread_t *) malloc (sizeof(pthread_t) * numhilos);

  fila = 1;
  numerofilas = (variables.ancho-2) / numhilos;
  excedente = (variables.ancho-2) % numhilos;
  for (i=0; i<numhilos; i++){
    structHilos[i].datos = &variables;
    structHilos[i].filainicio = fila;
    structHilos[i].numfilas = numerofilas;

      if (excedente > 0){
        structHilos[i].numfilas++;
        excedente--;
      }

    structHilos[i].salida = (unsigned char**)GetMem2D(structHilos[i].numfilas, variables.alto, sizeof(unsigned char));
    pthread_create (&mispthread[i], NULL, convolucion, &structHilos[i]);
    fila += structHilos[i].numfilas;
    }

  for (n=0; n<numhilos; n++){
	  pthread_join (mispthread[n], NULL);
	  for (i=0; i<structHilos[n].numfilas; i++)
    {
      for (j=1; j<variables.alto-1; j++)
      {
        salida[structHilos[n].filainicio + i][j] = structHilos[n].salida[i][j];
      }
    }
  }

  pgmwrite(salida, "lena_procesada_hilos.pgm", variables.ancho, variables.alto);

  Free2D((void**) variables.nucleo, tamanoNucleo);

  Free2D((void**) variables.original, variables.ancho);
  Free2D((void**) salida, variables.ancho);
  for (i=0; i<numhilos; i++)
  {
     Free2D ((void**) structHilos[i].salida, structHilos[i].numfilas);
  }
  free (structHilos);
  free (mispthread);
}
