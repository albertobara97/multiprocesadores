#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "pgm.h"
#include<stdbool.h>
int tamanoNucleo;
//#define tamanoNucleo 3

//Se crea una estructura global que almacena variables globales
typedef struct{
	unsigned char **original;
	int **nucleo;
	int ancho;
	int alto;
	int k;
}global;

//Se crea la estructura que almacena las variables del hilo
typedef struct{
	global *datos;
	unsigned char **salida;
	int filainicio;
	int numfilas;
  bool nucleoExtendido;
}hilo;

void *convolucion(void *parametro){
  int x, y, suma, i, j, f;

  hilo *h = (hilo *) parametro;
  if(h->nucleoExtendido == true){ //Convolución para una matriz núcleo de 5x5
    f = h->filainicio;
    for (x = 0; x < h->numfilas; x++){
      for (y = 2; y < h->datos->alto-2; y++){
        suma = 0;
        for (i = 0; i < tamanoNucleo; i++){
          for (j = 0; j < tamanoNucleo; j++){
              suma = suma + h->datos->original[(f-2)+i][(y-2)+j] * h->datos->nucleo[i][j];
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
  }else{ //Convolución para una matriz núcleo de 3x3
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
  printf ("Fila inicio %d  y total %d\n", h->filainicio, h->numfilas);
  
}

int nucleosDisponibles(){
    
  bool flag = true;
  int operacion;
  
  while(flag){
          printf("Filtro en función del nucleo\n\t0. Salir del programa\n\t1. Núcleo estándar (3)\n\t");
          printf("2. Núcleo Paso bajo (3)\n\t3. Núcleo Paso alto (3)\n\t4. Núcleo Gaussiano (3)\n\t");
          printf("5. Núcleo Grupo20 (3)\n\t6. Núcleo extendido 5x5 (5)\n");
          scanf ("%d", &operacion);

          if(operacion > 6 || operacion < 0){
              printf("Debes de escoger entre 0 y 6\n");
          }else{
              flag = false;
          }
      }flag = true;

  return operacion;
}

/* * * * *          * * * * *          * * * * *          * * * * */

int main(int argc, char *argv[]){
  int numhilos, i, j, n, fila, numerofilas, excedente, operacion;
  unsigned char** salida;
  hilo *structHilos;
  pthread_t *threads;
  global variables;
  bool flag  =true;

  //Valores de entrada para el número de hilos
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
  
  tamanoNucleo = 3;
  variables.nucleo = (int**) GetMem2D(tamanoNucleo, tamanoNucleo, sizeof(int));

  operacion = nucleosDisponibles(); //Muestra el menú de selección
  switch(operacion){
    case 0:
      printf("\nSaliendo del programa...\n\n");
          exit(0);
    break;
    case 1: 
      printf("Nucleo estándar (3)\n\n");
      //Recorre y rellena la matriz del nucleo
      for (i = 0; i < tamanoNucleo; i++){
        for (j = 0; j < tamanoNucleo; j++){
          variables.nucleo[i][j] = -1;
        }
      }variables.nucleo[tamanoNucleo/2][tamanoNucleo/2] = 1;
    break;
    case 2:
      printf("Nucleo Paso bajo (3)\n\n");
      for (i = 0; i < tamanoNucleo; i++){
        for (j = 0; j < tamanoNucleo; j++){
          variables.nucleo[i][j] = 1;
        }
      }variables.nucleo[tamanoNucleo/2][tamanoNucleo/2] = 0;
    break;
    case 3:
      printf("Núcleo Paso bajo (3)\n\n");
      for (i = 0; i < tamanoNucleo; i++){
        for (j = 0; j < tamanoNucleo; j++){
          variables.nucleo[i][j] = -1;
        }
      }variables.nucleo[tamanoNucleo/2][tamanoNucleo/2] = 8;
    break;
    case 4:
      printf("Núcleo Gaussiano (3)\n\n");
      for (i = 0; i < tamanoNucleo; i++){
        for (j = 0; j < tamanoNucleo; j++){
          if(j%2==0){
              variables.nucleo[i][j] = 2;
          }else{
              variables.nucleo[i][j] = 1;
          }
        }
      }variables.nucleo[tamanoNucleo/2][tamanoNucleo/2] = 4;
    break;
    case 5:
      printf("Núcleo Grupo20 (3)\n\n");
      for (i = 0; i < tamanoNucleo; i++){
        for (j = 0; j < tamanoNucleo; j++){
          variables.nucleo[i][j] = -3;
        }
      }variables.nucleo[tamanoNucleo/2][tamanoNucleo/2] = 10;
    break;
    case 6:
      printf("Núcleo extendido 5x5 (5)\n\n");
      tamanoNucleo = 5;
      variables.nucleo = (int**) GetMem2D(tamanoNucleo, tamanoNucleo, sizeof(int));
      for (i = 0; i < tamanoNucleo; i++){
        for (j = 0; j < tamanoNucleo; j++){
          variables.nucleo[i][j] = -3;
        }
      }variables.nucleo[tamanoNucleo/2][tamanoNucleo/2] = 10;
    break;
  }

  //Suma del núcleo
  variables.k = 0;
  
    for (i = 0; i < tamanoNucleo; i++){
    for (j = 0; j < tamanoNucleo; j++){
      variables.k = variables.k + variables.nucleo[i][j];
    }
  }
  
  structHilos = (hilo *) malloc (sizeof(hilo) * numhilos);
  threads = (pthread_t *) malloc (sizeof(pthread_t) * numhilos);

  //Definimos tamanoNucleo para 3x3 y nucleExtendido 5x5
  if(tamanoNucleo == 3){
    fila = 1;
    numerofilas = (variables.ancho-2) / numhilos;
    excedente = (variables.ancho-2) % numhilos;
  }else{
    fila = 2;
    numerofilas = (variables.ancho-4) / numhilos;
    excedente = (variables.ancho-4) % numhilos;
  }
  //Reparto en función de los hilos
  for (i=0; i<numhilos; i++){
    structHilos[i].datos = &variables;
    structHilos[i].filainicio = fila;
    structHilos[i].numfilas = numerofilas;

    if(tamanoNucleo == 5){
      structHilos[i].nucleoExtendido = true;
    }
    
    if (excedente > 0){
      structHilos[i].numfilas++;
      excedente--;
    }

    structHilos[i].salida = (unsigned char**)GetMem2D(structHilos[i].numfilas, variables.alto, sizeof(unsigned char));
    pthread_create (&threads[i], NULL, convolucion, &structHilos[i]);
    fila += structHilos[i].numfilas;
    }
  //Recorre cada hilo
  for (n=0; n<numhilos; n++){
	  pthread_join (threads[n], NULL);
    printf("[Hilo %d] Filas procesadas:\n", n);
	  //Recorre cada fila
    for (i=0; i<structHilos[n].numfilas; i++){
      printf("%d ", structHilos[n].filainicio + i);
        if(i != 0 && ((i+1)%20==0 || i == structHilos[n].numfilas-1))
          printf("\n");
      //Recorre cada celda de la fila la almacena en la matriz salida de la estructura salida
      for (j=1; j<variables.alto-1; j++){
        
        salida[structHilos[n].filainicio + i][j] = structHilos[n].salida[i][j];
      }
    }
  }

  pgmwrite(salida, "lena_procesada_hilos.pgm", variables.ancho, variables.alto);

  Free2D((void**) variables.nucleo, tamanoNucleo);

  Free2D((void**) variables.original, variables.ancho);
  Free2D((void**) salida, variables.ancho);
  for (i=0; i<numhilos; i++){
     Free2D ((void**) structHilos[i].salida, structHilos[i].numfilas);
  }
  free (structHilos);
  free (threads);
}
