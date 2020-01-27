#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
#include "pgm.h"
#include <stdbool.h>

#define TAG 10
#define tamanoNucleo 3

void convolucion(unsigned char **original, int numfilas, int ancho, int **nucleo, int k, unsigned char **salida, int rank) {
   int x, y;
   int suma;
   int i, j;
   //printf("%d\n ", rank);

   for (x=0; x<numfilas; x++){
      for (y=0; y<ancho; y++){
	      if (x == 0 || x == numfilas-1 || y == 0 || y == ancho-1)
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
}

int nucleosDisponibles(){
    
    bool flag = true;
    int operacion;
    
    while(flag){
            printf("Filtro en función del nucleo\n\t0. Salir del programa\n\t1. Núcleo estándar\n\t2. Núcleo Paso bajo\n\t3. Núcleo Paso alto\n\t4. Núcleo Laplaciano \n\t5. Núcleo Grupo20\n");
            scanf ("%d", &operacion);

            if(operacion > 5 || operacion < 0){
                printf("Debes de escoger entre 0 y 5\n");
            }else{
                flag = false;
            }
        }flag = true;

    return operacion;
}

/* * * * *          * * * * *          * * * * *          * * * * */

int main(int argc, char *argv[]){
   
   int alto, ancho, operacion, k, rank, size, i, j, f, filainicio, numfilas, numerofilas, excedente;
   unsigned char** original;
   unsigned char** salida;
   unsigned char* aux;
   int **nucleo;
   MPI_Status status;
   bool flag;

   MPI_Init (&argc, &argv);
   MPI_Comm_size (MPI_COMM_WORLD, &size);
   MPI_Comm_rank (MPI_COMM_WORLD, &rank);

   //Reserva memoria para la matriz del nucleo
   nucleo = (int**) GetMem2D(tamanoNucleo, tamanoNucleo, sizeof(int));
   
   
   if (rank == 0){
   
      original = pgmread("lena_original.pgm", &alto, &ancho);
      
      operacion = nucleosDisponibles();
      switch(operacion){
         /*case 0:
            printf("\nSaliendo del programa...\n\n");
               MPI_Finalize();
               //exit(0);
         break;*/
         case 1: 
            printf("Nucleo estándar\n\n");
            //Recorre y rellena la matriz del nucleo
            for (i = 0; i < tamanoNucleo; i++){
               for (j = 0; j < tamanoNucleo; j++){
                  nucleo[i][j] = -1;
               }
            }nucleo[tamanoNucleo/2][tamanoNucleo/2] = 1;
         break;
         case 2:
            printf("Nucleo Paso bajo\n\n");
            for (i = 0; i < tamanoNucleo; i++){
               for (j = 0; j < tamanoNucleo; j++){
                  nucleo[i][j] = 1;
               }
            }nucleo[tamanoNucleo/2][tamanoNucleo/2] = 0;
         break;
         case 3:
            printf("Núcleo Paso bajo\n\n");
            for (i = 0; i < tamanoNucleo; i++){
               for (j = 0; j < tamanoNucleo; j++){
                  nucleo[i][j] = -1;
               }
            }nucleo[tamanoNucleo/2][tamanoNucleo/2] = 8;
         break;
         case 4:
            printf("Núcleo Gaussiano\n\n");
            for (i = 0; i < tamanoNucleo; i++){
               for (j = 0; j < tamanoNucleo; j++){
                  if(j%2==0){
                     nucleo[i][j] = 2;
                  }else
                  {
                     nucleo[i][j] = 1;
                  }
                  
               }
            }nucleo[tamanoNucleo/2][tamanoNucleo/2] = 4;
         break;
         case 5:
            printf("Núcleo Grupo20\n\n");
            for (i = 0; i < tamanoNucleo; i++){
               for (j = 0; j < tamanoNucleo; j++){
                  nucleo[i][j] = -3;
               }
            }nucleo[tamanoNucleo/2][tamanoNucleo/2] = 10;
         break;
      }
      for(i = 1; i<size; i++){
         MPI_Send(&operacion, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
         
      }
      if(operacion == 0){
         MPI_Finalize();
         exit(0);
      }
      
      k = 0;
      for (i = 0; i < tamanoNucleo; i++){
         for (j = 0; j < tamanoNucleo; j++){
            k = k + nucleo[i][j];
         }
      }

      
      for (i=1; i<size; i++){
         for(j = 0; j<tamanoNucleo; j++){
            MPI_Send(&nucleo[j][0], tamanoNucleo, MPI_INT, i, TAG, MPI_COMM_WORLD);
         }
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
         //if(i == 3) numfilas -=1;
         for(f=filainicio-1; f<filainicio+numfilas+1; f++){
	         /*if(i == 3 && f == filainicio+numfilas-1){
               printf("llego %d\n", f);
               MPI_Send(&original[f-5][0], ancho, MPI_CHAR, i, TAG, MPI_COMM_WORLD);
            }else{*/
               MPI_Send(&original[f][0], ancho, MPI_CHAR, i, TAG, MPI_COMM_WORLD);
            //}
         }

 	      printf("[Procesador %d] Filainicio: %d, numfilas: %d\n", i, filainicio, numfilas); 
         filainicio = filainicio + numfilas;
      
      }
      
      numfilas = numerofilas;
      salida = (unsigned char**)GetMem2D(alto, ancho, sizeof(unsigned char));
   
   }else{
      printf ("[Procesador %d]\n", rank);
      
      //fflush(stdout);
      MPI_Recv (&operacion, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
      if(operacion==0){
         MPI_Finalize();
         exit(0);
      }
      for(i = 0; i<tamanoNucleo; i++){
         MPI_Recv (&nucleo[i][0], tamanoNucleo, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
      }
      
      MPI_Recv (&ancho, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
      printf ("[Procesador %d] Recibido ancho de la imagen: %d\n", rank, ancho);
      
      MPI_Recv (&k, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
      printf ("[Procesador %d] Recibido k: %d\n", rank, k);
   
      MPI_Recv (&numfilas, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);      
      printf ("[Procesador %d] Recibido numfilas: %d\n", rank, numfilas);

      original = (unsigned char **)GetMem2D(numfilas+2, ancho, sizeof(unsigned char));
      printf ("[Procesador %d] Imagen procesada de %dx%d\n", rank, numfilas+2, ancho);
      //if(rank == 3) numfilas -= 1;
      for (f=0; f<numfilas+2; f++){
            MPI_Recv (&original[f][0], ancho, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &status);
            /*if(rank == 3 && f == numfilas+1){
               for(int t = 0; t<ancho; t++){
                  original[f][t] = original[f-1][t];
               }
            }*/
         }
         
      salida = (unsigned char **)GetMem2D(numfilas+2, ancho, sizeof(unsigned char));
   }
   //printf("Alto: %d\n", alto);
   //if(rank == 3) numfilas -= 1;
   convolucion(original, numfilas+2, ancho, nucleo, k, salida, rank);
   
   f = numfilas+1;
   
   if (rank == 0){
      
      for (i=1; i<size; i++){
         MPI_Recv (&numfilas, 1, MPI_INT, i, TAG, MPI_COMM_WORLD, &status);
	      aux = (unsigned char *)GetMem2D(ancho, alto, sizeof(unsigned char));
         
         printf("[Procesador %d] Fila recibida:\n", rank);
         for (j=0; j<numfilas; j++){
            printf (" %d", f+j);
            if(j != 0 && ((j+1)%20==0 || j == numfilas-1))
               printf("\n");
               
            MPI_Recv (&aux[0], ancho, MPI_CHAR, i, TAG, MPI_COMM_WORLD, &status);
	    
            for (k=0; k<ancho; k++){
		         salida[f+j][k] = aux[k];
            }
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
      //Envía convoluciones a procesador 0
      for (f=0; f<numfilas; f++){
         MPI_Send (&salida[f][0], ancho, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
      }
      
      Free2D((void**) original, numfilas+2);
      Free2D((void**) salida, numfilas+2);
      
      MPI_Recv (&k, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
   }

   Free2D((void**) nucleo, tamanoNucleo);

   MPI_Finalize();
}
