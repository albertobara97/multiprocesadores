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
	int largo;
	int alto;
	int k;
}tipodato;

typedef struct{
	tipodato *datos;
	unsigned char **salida;
	int filainicio;
	int numfilas;
}tipohilo;

void *convolucion(void *parametro){
  int x, y, suma, i, j, f;

  tipohilo *h = (tipohilo *) parametro;

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
  unsigned char** Salida;
  tipohilo *mishilos;
  pthread_t *mispthread;
  tipodato misdatos;
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

  misdatos.original = pgmread("lena_original.pgm", &misdatos.largo, &misdatos.alto);
  Salida = (unsigned char**)GetMem2D(misdatos.largo, misdatos.alto, sizeof(unsigned char));

  misdatos.nucleo = (int**) GetMem2D(tamanoNucleo, tamanoNucleo, sizeof(int));
  for (i = 0; i < tamanoNucleo; i++){
    for (j = 0; j < tamanoNucleo; j++){
      misdatos.nucleo[i][j] = -1;
    }
  }misdatos.nucleo[tamanoNucleo/2][tamanoNucleo/2] = 1;
  
  misdatos.k = 0;
  
  for (i = 0; i < tamanoNucleo; i++){
    for (j = 0; j < tamanoNucleo; j++){
      misdatos.k = misdatos.k + misdatos.nucleo[i][j];
    }
  }
  
  mishilos = (tipohilo *) malloc (sizeof(tipohilo) * numhilos);
  mispthread = (pthread_t *) malloc (sizeof(pthread_t) * numhilos);

  fila = 1;
  numerofilas = (misdatos.largo-2) / numhilos;
  excedente = (misdatos.largo-2) % numhilos;
  for (i=0; i<numhilos; i++){
    mishilos[i].datos = &misdatos;
    mishilos[i].filainicio = fila;
    mishilos[i].numfilas = numerofilas;

      if (excedente > 0){
        mishilos[i].numfilas++;
        excedente--;
      }

    mishilos[i].salida = (unsigned char**)GetMem2D(mishilos[i].numfilas, misdatos.alto, sizeof(unsigned char));
    pthread_create (&mispthread[i], NULL, convolucion, &mishilos[i]);
    fila += mishilos[i].numfilas;
    }

  for (n=0; n<numhilos; n++){
	  pthread_join (mispthread[n], 0);
	  for (i=0; i<mishilos[n].numfilas; i++)
    {
      for (j=1; j<misdatos.alto-1; j++)
      {
        Salida[mishilos[n].filainicio + i][j] = mishilos[n].salida[i][j];
      }
    }
  }

  pgmwrite(Salida, "lena_procesada_hilos.pgm", misdatos.largo, misdatos.alto);

  Free2D((void**) misdatos.nucleo, tamanoNucleo);

  Free2D((void**) misdatos.original, misdatos.largo);
  Free2D((void**) Salida, misdatos.largo);
  for (i=0; i<numhilos; i++)
  {
     Free2D ((void**) mishilos[i].salida, mishilos[i].numfilas);
  }
  free (mishilos);
  free (mispthread);
}
