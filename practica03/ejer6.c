#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h> 
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#define LONGITUD_CADENA 50
#define TAG 1

int operacionesDisponibles(){
    
    bool flag = true;
    int operacion;
    
    while(flag){
            printf("operacion a realizar?\n\t0. Salir del programa\n\t1. Pasar una palabra a mayuscula\n\t2. Calcular suma de un vector y la raiz cuadrada de la suma \n\t3. Calcular los enteros correspondientes de una frase\n\t4. Realizar todas las operaciones anteriores\n");
            scanf ("%d", &operacion);

            if(operacion > 4 || operacion < 0){
                printf("Debes de escoger entre 0 y 4\n");
            }else{
                flag = false;
            }
        }flag = true;

    return operacion;
}


int main (int argc, char *argv[]){

    int size, rank, operacion, posicionesCadena, tamanoVector;
    double raizCuadrada, suma;
    
    bool flag;
    char cadena[LONGITUD_CADENA];
    MPI_Status status;

	MPI_Init (&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &size);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);

    if (size != 4){
    
        if (rank == 0){
            printf ("\nRequiere de 4 procesadores para su ejecución.\n\n");
        }
        MPI_Finalize();
        exit(-1);
    }
    switch (rank){
        case 0:
        do{
            operacion = operacionesDisponibles();

            for (int i = 1; i<size; i++){
				MPI_Send (&operacion, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
			}
            
            switch(operacion){
                case 0: printf("\nSaliendo del programa...\n\n");
                    MPI_Finalize();
                    exit(0);
				break;

                case 1: printf("Voy a decirle al procesador 1 que te pregunte una letra que luego pasaré a mayuscula\n\n");

                    scanf("%s", cadena);
                    //MPI_Send (&operacion, 1, MPI_INT, 1, TAG, MPI_COMM_WORLD);
                    MPI_Send(&cadena[0], LONGITUD_CADENA, MPI_CHAR, 1, TAG, MPI_COMM_WORLD);
                    MPI_Recv(&cadena[0], LONGITUD_CADENA, MPI_CHAR, 1, TAG, MPI_COMM_WORLD, &status);
                    printf("\nEl mensaje en mayuscula: %s\n", cadena);

                    
                    //MPI_Recv (&cadena[0], 25, MPI_CHAR, 1, TAG, MPI_COMM_WORLD, &status);
                break;

                case 2: printf("voy a enviarle procesador 2 un vector con 10 numeros reales y el ya hara algunos calculos\n\n");
                    double vector[10] = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9, 10.10};
                    tamanoVector = 10;
                    MPI_Send (&tamanoVector, 1, MPI_INT, 2, TAG, MPI_COMM_WORLD);
                    MPI_Send(&vector[0], 10, MPI_DOUBLE, 2, TAG, MPI_COMM_WORLD);
                    MPI_Recv(&raizCuadrada, 1, MPI_DOUBLE, 2, TAG, MPI_COMM_WORLD, &status);
                    printf("\nRaiz cuadrada de la suma de los elementos vector: %.2f\n", raizCuadrada);
                break;

                case 3: printf("Voy a enviarle una señal al procesador 3 para que el ya haga sus cosas\n\n");
                    //MPI_Send (&operacion, 1, MPI_INT, 3, TAG, MPI_COMM_WORLD);
                    MPI_Recv(&posicionesCadena, 1, MPI_INT, 3, TAG, MPI_COMM_WORLD, &status);
                    printf("La posiciones de la frase suman: %i\n", posicionesCadena);
                break;

                case 4: printf("Ea, Todos los procesadores a trabajar. Yo me quedo aqui mirando como trabajais\n\n");
                    
                    //Interaccion con el procesador 1
                    scanf("%s", cadena);
                    //MPI_Send (&operacion, 1, MPI_INT, 1, TAG, MPI_COMM_WORLD);
                    MPI_Send(&cadena[0], LONGITUD_CADENA, MPI_CHAR, 1, TAG, MPI_COMM_WORLD);
                    MPI_Recv(&cadena[0], LONGITUD_CADENA, MPI_CHAR, 1, TAG, MPI_COMM_WORLD, &status);
                    printf("\nEl mensaje en mayuscula: %s\n", cadena);
                    
                    //Interaccion con el procesador 2
                    //double vector[10] = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9, 10.10};
                    //tamanoVector = 10;
                    MPI_Send (&tamanoVector, 1, MPI_INT, 2, TAG, MPI_COMM_WORLD);
                    MPI_Send(&vector[0], 10, MPI_DOUBLE, 2, TAG, MPI_COMM_WORLD);
                    MPI_Recv(&raizCuadrada, 1, MPI_DOUBLE, 2, TAG, MPI_COMM_WORLD, &status);
                    printf("\nRaiz cuadrada de la suma de los elementos vector: %.2f\n", raizCuadrada);

                    //Interaccion con el procesador 3
                    MPI_Recv(&posicionesCadena, 1, MPI_INT, 3, TAG, MPI_COMM_WORLD, &status);
                    printf("La posiciones de la frase suman: %i\n", posicionesCadena);

                break;
            }
        }while(operacion != 0);
        break;
    
        case 1:do{
        //printf("Soy procesador 1, entro siempre\n"); 
        MPI_Recv (&operacion, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
        //flag = true;
        if(operacion == 1 || operacion == 4){
            printf("Seleccionado procesador 1\n");

            MPI_Recv(&cadena[0], LONGITUD_CADENA, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &status);
            
            for (int i = 0; i<LONGITUD_CADENA; i++){
                    //toupper solo funciona caracter a caracter. Recorremos toda la cadena
					cadena[i] = toupper(cadena[i]);
				}
            MPI_Send (cadena, LONGITUD_CADENA, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
            
        }
        }while(operacion != 0);
        break;

        case 2:
        do{
        //printf("Soy procesador 2, entro siempre\n"); 
        //printf("Esperando el receive...");
        MPI_Recv (&operacion, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);

        if(operacion == 2 || operacion == 4){
            printf("Seleccionado procesador 2\n\n");
            
            MPI_Recv (&tamanoVector, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
            double vector[tamanoVector];
            MPI_Recv (&vector[0], 10, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD, &status);
				suma = 0;
				for (int i = 0; i<tamanoVector; i++){
                    
					suma += vector[i];
                    if(i == 0){
                        printf("%.2f", suma);
                    }else{
                        printf(" -> %.2f", suma);
                    }
				}
                printf("\n");
				//MPI_Send (&suma, 1, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
				raizCuadrada = suma; 
				MPI_Send (&raizCuadrada, 1, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
        }
        }while(operacion != 0);
        break;

        case 3:
        do{
        //printf("Soy procesador 3, entro siempre\n"); 
        //printf("Esperando el receive...");
        MPI_Recv (&operacion, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);

        if(operacion == 3 || operacion == 4){
            printf("Seleccionado procesador 3\n\n");
            printf("___________Entrando en funcionalidad 3___________\n\n");
            
            strcpy(cadena, "Entrando en funcionalidad 3");
            
            posicionesCadena = 0;
            for (int i = 0; i<strlen(cadena); i++){
                posicionesCadena += cadena[i];
            }
            MPI_Send (&posicionesCadena, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
        }
        }while(operacion != 0);
        break;
        
    }
    
    MPI_Finalize();

}