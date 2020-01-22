/*
   primera version del proyecto con hilos

   voy a dividir la tarea en una serie de hilos (valor introducido como parametro al programa

   para hacer la division, se puede hacer por filas, por columnas o pasar todo a una matriz de 1 dimensión y dividimos el vector resultante en trozos

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "pgm.h"

/* defino el tipo de datos con la información que tengo que pasar a cada hilo
es decir: la imagen original, el nucleo, la imagen de salida, el numero de filas, el numero de columnas, fila por la que empiezo, cuantas filas tengo que convolucionar
me voy a definir una estructura con la información común a todos los hilos y luego la estructura especifica de cada hilo que utiliza la misma información común
*/

typedef struct
{
	unsigned char **original;
	int **nucleo;
	int largo;
	int alto;
	int k; //la suma del nucleo
}tipodato;

typedef struct
{
	tipodato *datos;
	unsigned char **salida;
	int filainicio;
	int numfilas;
}tipohilo;



void *convolucion(void *parametro) {
  int x, y;
  int suma;
  int i, j, f;
  tipohilo *h = (tipohilo *) parametro;

  printf ("\nFila inicio %d  y total %d", h->filainicio, h->numfilas);
  fflush (stdout);
  f = h->filainicio;
  for (x = 0; x < h->numfilas; x++){
    for (y = 1; y < h->datos->alto-1; y++){
      suma = 0;
      for (i = 0; i < 3; i++){
        for (j = 0; j < 3; j++){
            suma = suma + h->datos->original[(f-1)+i][(y-1)+j] * h->datos->nucleo[i][j];
        }
      }
      if(h->datos->k==0)
        h->salida[x][y] = suma;
      else
        h->salida[x][y] = suma/h->datos->k;
    }
    f++;
  }
}

/* * * * *          * * * * *          * * * * *          * * * * */

int main(int argc, char *argv[]){
  //int Largo, Alto;
  int numhilos, i, j, n, fila, numerofilas, excedente;
  //unsigned char** Original;
  unsigned char** Salida;
  //int **nucleo;
  tipohilo *mishilos;
  pthread_t *mispthread;
  tipodato misdatos;


  if (argc != 2)
  {
     printf ("\nError en la llamada, hay que indicar el numero de hilos\n\n");
     exit(1);
  }
  sscanf (argv[1], "%d", &numhilos);

  misdatos.original = pgmread("lena_original.pgm", &misdatos.largo, &misdatos.alto);
  Salida = (unsigned char**)GetMem2D(misdatos.largo, misdatos.alto, sizeof(unsigned char));

  misdatos.nucleo = (int**) GetMem2D(3, 3, sizeof(int));
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      misdatos.nucleo[i][j] = -1;
  misdatos.nucleo[1][1] = 1;
  //calculo la suma del nucleo
  misdatos.k = 0;
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      misdatos.k = misdatos.k + misdatos.nucleo[i][j];

  mishilos = (tipohilo *) malloc (sizeof(tipohilo) * numhilos);
  mispthread = (pthread_t *) malloc (sizeof(pthread_t) * numhilos);

  //convolucion(Original, nucleo, Salida, Largo, Alto);
  fila = 1;
  numerofilas = (misdatos.largo-2) / numhilos;
  excedente = (misdatos.largo-2) % numhilos;
  for (i=0; i<numhilos; i++)
  {
	mishilos[i].datos = &misdatos;
	mishilos[i].filainicio = fila;
	mishilos[i].numfilas = numerofilas;
	if (excedente > 0)
	{
		mishilos[i].numfilas++;
		excedente--;
	}
	mishilos[i].salida = (unsigned char**)GetMem2D(mishilos[i].numfilas, misdatos.alto, sizeof(unsigned char));
	pthread_create (&mispthread[i], NULL, convolucion, &mishilos[i]);
	fila = fila + mishilos[i].numfilas;
  }

  for (n=0; n<numhilos; n++)
  {
	pthread_join (mispthread[n], 0);
	for (i=0; i<mishilos[n].numfilas; i++)
	{
		for (j=1; j<misdatos.alto-1; j++)
		{
			Salida[mishilos[n].filainicio + i][j] = mishilos[n].salida[i][j];
		}
	}
  }

  pgmwrite(Salida, "lena_procesada.pgm", misdatos.largo, misdatos.alto);

  Free2D((void**) misdatos.nucleo, 3);

  Free2D((void**) misdatos.original, misdatos.largo);
  Free2D((void**) Salida, misdatos.largo);
  for (i=0; i<numhilos; i++)
  {
     Free2D ((void**) mishilos[i].salida, mishilos[i].numfilas);
  }
  free (mishilos);
  free (mispthread);
}
