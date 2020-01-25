#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
#include "pgm.h"

#define TAG 10
#define tamanoNucleo 3

void convolucion(unsigned char **original, int numfilas, int alto, int **nucleo, int k, unsigned char **salida, int rank) {
   int x, y;
   int suma;
   int i, j;

   for (x=0; x<numfilas; x++){
      for (y=0; y<alto; y++){
	      if (x == 0 || x == numfilas-1 || y == 0 || y == alto-1)
	         salida[x][y] = original[x][y];
	      else{
	         suma = 0;
      	   for (i = 0; i < tamanoNucleo; i++){
               for (j = 0; j < tamanoNucleo; j++){
                 suma = suma + original[(x-1)+i][(y-1)+j] * nucleo[i][j];
              }
           }
	         if(k==0){
               //printf("[Procesador %d](%d, %d)-------k = 0...\n", rank, x, y);
        	      salida[x][y] = suma;
            }else{
               //printf("[Procesador %d](%d, %d)-------k != 0...\n", rank, x, y);
        	      salida[x][y] = suma/k;
            }
	      }
      }
   }
   printf("llego");
}

/* * * * *          * * * * *          * * * * *          * * * * */

int main(int argc, char *argv[]){
   int alto, ancho;
   int k, rank, size, i, j, f/*, fila*/, filainicio, numfilas, numerofilas, excedente;
   unsigned char** original;
   unsigned char** salida;
   unsigned char* aux;
   int **nucleo;
   MPI_Status status;

   MPI_Init (&argc, &argv);
   MPI_Comm_size (MPI_COMM_WORLD, &size);
   MPI_Comm_rank (MPI_COMM_WORLD, &rank);


   nucleo = (int**) GetMem2D(tamanoNucleo, tamanoNucleo, sizeof(int));
   
   for (i = 0; i < tamanoNucleo; i++){
      for (j = 0; j < tamanoNucleo; j++){
         nucleo[i][j] = -1;
      }
   }nucleo[tamanoNucleo/2][tamanoNucleo/2] = 8;

   if (rank == 0){
   
      original = pgmread("lena_original.pgm", &alto, &ancho);

      
      k = 0;
      for (i = 0; i < tamanoNucleo; i++)
         for (j = 0; j < tamanoNucleo; j++)
            k = k + nucleo[i][j];

      
      for (i=1; i<size; i++){

         MPI_Send(&ancho, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
         MPI_Send(&k, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
      }

      numerofilas = (alto-2) / size;
      excedente = (alto-2) % size;
      printf("[Procesador %d] Proceso %d filas\n   ", rank, numerofilas);
      
      filainicio = numerofilas+1;
      
      for (i=1; i<size; i++){
	      
         numfilas = numerofilas;
	      
         if (excedente > 0){
	         numfilas++;
	         excedente--;
	      }
         
         
         MPI_Send(&numfilas, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
         
         for(f=filainicio-1; f<filainicio+numfilas+1; f++){
	         MPI_Send(&original[f][0], ancho, MPI_CHAR, i, TAG, MPI_COMM_WORLD);
         }

 	      printf("[Procesador %d] Filainicio: %d, numfilas: %d\n", i, filainicio, numfilas); 
         filainicio = filainicio + numfilas;
      
      }
      
      numfilas = numerofilas;
      salida = (unsigned char**)GetMem2D(alto, ancho, sizeof(unsigned char));
   
   }else{
      printf ("[Procesador %d]\n", rank);
      
      fflush(stdout);
      
      MPI_Recv (&ancho, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
      printf ("[Procesador %d] Recibido ancho de la imagen: %d\n", rank, ancho);
      
      MPI_Recv (&k, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
      printf ("[Procesador %d] Recibido k: %d\n", rank, k);
   
      MPI_Recv (&numfilas, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);      
      printf ("[Procesador %d] Recibido numfilas: %d\n", rank, numfilas);

      original = (unsigned char **)GetMem2D(numfilas+2, ancho, sizeof(unsigned char));
      printf ("[Procesador %d] Imagen procesada de %dx%d\n", rank, numfilas+2, ancho);
      
      for (f=0; f<numfilas+2; f++){
            MPI_Recv (&original[f][0], ancho, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &status);
         }
         
      salida = (unsigned char **)GetMem2D(numfilas+2, ancho, sizeof(unsigned char));
   }
   printf("soy %d\n", rank);
   convolucion(original, numfilas+2, alto, nucleo, k, salida, rank);
   
   f = numfilas+1;
   
   if (rank == 0){
      
      for (i=1; i<size; i++){
         MPI_Recv (&numfilas, 1, MPI_INT, i, TAG, MPI_COMM_WORLD, &status);
	      aux = (unsigned char *)GetMem2D(ancho, alto, sizeof(unsigned char));
         
         printf("[Procesador %d] Fila recibida:\n", rank);
         for (j=0; j<numfilas; j++){
            printf (" %d", f+j);
            if(j != 0 && (j%20==0 || j == numfilas-1))
               printf("\n");
               
            MPI_Recv (&aux[0], ancho, MPI_CHAR, i, TAG, MPI_COMM_WORLD, &status);
	    
            for (k=0; k<ancho; k++)
		         salida[f+j][k] = aux[k];
         }
	      
         free(aux);

         f += numfilas;
      }

      pgmwrite(salida, "lena_procesada_mpi.pgm", alto, ancho);

      for(i=1; i<size; i++)
	      MPI_Send (&i, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);

      Free2D((void**) original, alto);
      Free2D((void**) salida, alto);
   
   }else {
      
      MPI_Send (&numfilas, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
      
      for (f=1; f<numfilas+1; f++){
         MPI_Send (&salida[f][0], ancho, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
      }
      
      Free2D((void**) original, numfilas+2);
      Free2D((void**) salida, numfilas+2);
      
      MPI_Recv (&k, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
   }

   Free2D((void**) nucleo, tamanoNucleo);

   MPI_Finalize();
}
