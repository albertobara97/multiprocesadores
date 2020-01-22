/*
   primera version del proyecto con hilos

   voy a dividir la tarea en una serie de hilos (valor introducido como parametro al programa

   para hacer la division, se puede hacer por filas, por columnas o pasar todo a una matriz de 1 dimensión y dividimos el vector resultante en trozos

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
#include "pgm.h"

/* defino el tipo de datos con la información que tengo que pasar a cada hilo
es decir: la imagen original, el nucleo, la imagen de salida, el numero de filas, el numero de columnas, fila por la que empiezo, cuantas filas tengo que convolucionar
me voy a definir una estructura con la información común a todos los hilos y luego la estructura especifica de cada hilo que utiliza la misma información común
*/

#define TAG 10
#define MAX 3

void convolucion(unsigned char **original, int numfilas, int largo, 
		int **nucleo, int k, unsigned char **salida) {
  int x, y;
  int suma;
  int i, j;

  //copio la original en la salida
  for (x=0; x<numfilas; x++) {
    for (y=0; y<largo; y++) {
	if (x == 0 || x == numfilas-1 || y == 0 || y == largo-1)
	   salida[x][y] = original[x][y];
	else {
	   suma = 0;
      	   for (i = 0; i < MAX; i++){
              for (j = 0; j < MAX; j++){
                 suma = suma + original[(x-1)+i][(y-1)+j] * nucleo[i][j];
              }
           }
	   if(k==0)
        	salida[x][y] = suma;
           else
        	salida[x][y] = suma/k;
	}
    }
  }
  /*for (x = 1; x < numfilas-1; x++){
    for (y = 1; y < largo-1; y++){
      suma = 0;
      for (i = 0; i < MAX; i++){
        for (j = 0; j < MAX; j++){
            suma = suma + original[(x-1)+i][(y-1)+j] * nucleo[i][j];
        }
      }
      if(k==0)
        salida[x][y] = suma;
      else
        salida[x][y] = suma/k;
    }
  }*/
}

/* * * * *          * * * * *          * * * * *          * * * * */

int main(int argc, char *argv[]){
  int largo, alto;
  int k, rank, size, i, j, f, fila, filainicio, numfilas, numerofilas, excedente;
  unsigned char** original;
  unsigned char** salida;
  unsigned char* aux;
  int **nucleo;
  MPI_Status status;

  /*if (argc != 2)
  {
     printf ("\nError en la llamada, hay que indicar el numero de hilos\n\n");
     exit(1);
  }
  sscanf (argv[1], "%d", &numhilos);*/

   MPI_Init (&argc, &argv);
   MPI_Comm_size (MPI_COMM_WORLD, &size);
   MPI_Comm_rank (MPI_COMM_WORLD, &rank);


   nucleo = (int**) GetMem2D(MAX, MAX, sizeof(int));
   for (i = 0; i < MAX; i++)
      for (j = 0; j < MAX; j++)
         nucleo[i][j] = -1;
   nucleo[MAX/2][MAX/2] = 1;

   if (rank == 0)
   {
      //leo la imagen
      original = pgmread("lena_original.pgm", &largo, &alto);

      //calculo la suma del nucleo
      k = 0;
      for (i = 0; i < MAX; i++)
         for (j = 0; j < MAX; j++)
            k = k + nucleo[i][j];

      //envio la informacion comun a todos los procesos
      for (i=1; i<size; i++)
      {
         //MPI_Send (&largo, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
         MPI_Send (&alto, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
         MPI_Send (&k, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
      }

      //reparto de tareas
      numerofilas = (largo-2) / size;//(alto-2) / size;
      excedente = (largo-2) % size; //(alto-2) % size;
      printf ("\nEl proceso 0 trata %d filas", numerofilas); fflush (stdout);
      filainicio = numerofilas+1;
      for (i=1; i<size; i++)
      {
	 //filainicio = fila;
	 numfilas = numerofilas;
	 if (excedente > 0)
	 {
	    numfilas++;
	    excedente--;
	 }
         //MPI_Send (&filainicio, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
         MPI_Send (&numfilas, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
         //a cada proceso le tengo que pasar las filas que le tocan mas la
         //fila anterior mas la fila siguiente
         for (f=filainicio-1; f<filainicio+numfilas+1; f++)
         {
            //MPI_Send (&original[f][0], largo, MPI_CHAR, i, TAG, MPI_COMM_WORLD);
	    MPI_Send (&original[f][0], alto, MPI_CHAR, i, TAG, MPI_COMM_WORLD);
         }
 	 printf ("\nproceso: %d filainicio: %d y numfilas: %d", i, filainicio, numfilas); fflush (stdout);
         filainicio = filainicio + numfilas;
      }
      numfilas = numerofilas;
      salida = (unsigned char**)GetMem2D(largo, alto, sizeof(unsigned char));
   }
   else 
   {
	printf ("\nen rank %d", rank);fflush(stdout);
         //MPI_Recv (&largo, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
         MPI_Recv (&alto, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
	printf ("\nen rank %d recibo alto: %d", rank, alto);fflush(stdout);
         MPI_Recv (&k, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
	printf ("\nen rank %d recibo k: %d", rank, k);fflush(stdout);
         //MPI_Recv (&filainicio, 1, MPI_INT, i, TAG, MPI_COMM_WORLD, &status);
         MPI_Recv (&numfilas, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
	printf ("\nen rank %d recibo numfilas: %d", rank, numfilas);fflush(stdout);
         original = (unsigned char **)GetMem2D (numfilas+2, alto, sizeof(unsigned char));
	 printf ("\ncreo %dx%d en proceso %d", numfilas+2, alto, rank);
         for (f=0; f<numfilas+2; f++)
         {
            MPI_Recv (&original[f][0], alto, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &status);
         }
         //pgmwrite(original, "lenax.pgm", largo, numfilas+2);
         salida = (unsigned char **)GetMem2D (numfilas+2, alto, sizeof(unsigned char));
   }
   convolucion (original, numfilas+2, alto, nucleo, k, salida);
   f = numfilas+1;
   if (rank == 0)
   {
      for (i=1; i<size; i++)
      {
         MPI_Recv (&numfilas, 1, MPI_INT, i, TAG, MPI_COMM_WORLD, &status);
	 aux = (unsigned char *)GetMem (alto, sizeof(unsigned char));
         for (j=0; j<numfilas; j++)
         {
            printf ("\nrecibiendo fila: %d", f+j); fflush (stdout);
            MPI_Recv (&aux[0], alto, MPI_CHAR, i, TAG, MPI_COMM_WORLD, &status);
	    for (k=0; k<alto; k++)
		salida[f+j][k] = aux[k];
            //f++;
         }
	 free (aux);
         f = f + numfilas;
      }

      pgmwrite(salida, "lena_procesada_mpi.pgm", largo, alto);

      Free2D((void**) original, largo);
      Free2D((void**) salida, largo);
      for (i=1; i<size; i++)
	MPI_Send (&i, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
   }
   else 
   {
      MPI_Send (&numfilas, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
      for (f=1; f<numfilas+1; f++)
      {
         MPI_Send (&salida[f][0], alto, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
      }
      Free2D((void**) original, numfilas+2);
      Free2D((void**) salida, numfilas+2);
      MPI_Recv (&k, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
   }

   Free2D((void**) nucleo, MAX);

}
