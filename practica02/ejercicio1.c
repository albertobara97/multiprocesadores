#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_THREADS 2   //inicializamos variable global 
#define NUM_CLIENTES 10 //inicializamos variable global 

int *ingresos;
int *gastos;

void inicioVectores(){
    //Se crean dos punteros de tipo entero y se reserva un espacio en memoria
    ingresos = (int*) malloc(NUM_CLIENTES*sizeof(int));
    gastos = (int*) malloc(NUM_CLIENTES*sizeof(int));
    //Se generan valores para cada cliente
    for(int i = 0; i<NUM_CLIENTES; i++){
        ingresos[i] = 200 - i*4;
        gastos[i] = i*20;
        printf("%i %i \n", ingresos[i], gastos[i]);
    }
}
//Metodo encargado de calcular los ingresos totales
void *calculoIngresos(){
    int *sumaIngresos = malloc(sizeof(int));
    *sumaIngresos = 0;

    for(int i=0; i<NUM_CLIENTES; i++){
        *sumaIngresos += ingresos[i];
    }
}
//Metodo encargado de calcular los gastos totales
void *calculoGastos(){
    int *sumaGastos = malloc(sizeof(int));
    *sumaGastos = 0;

    for(int i=0; i<NUM_CLIENTES; i++){
        *sumaGastos += gastos[i];
    }
}

int main(int argc, char *argv[]){
    int iret1, iret2; //Variables que almacenaran salida de pthread_create()
    pthread_t thread1, thread2; //Se crean 2 objetos de tipo pthread_t.
    printf("1----------------\n");
    //Reserva espacio en memoria para variable de ingresos
    int *tmpIngresos = malloc(sizeof(int));
    //Inicializa tmpIngresos a 0
    tmpIngresos = 0; //inicializa el puntero a nulo.
    //Reserva un espacio en memoria para variable de gastos
    int *tmpGastos = malloc(sizeof(int));
    tmpGastos = 0; //inicializa el puntero a nulo.
    
    printf("2----------------\n");

    inicioVectores();

    printf("3----------------\n");
    //Se crean los hilos y se almacena el valor que devuelven en las 2 variables locales
    iret1 = pthread_create(&thread1, NULL, calculoIngresos, NULL);
    iret2 = pthread_create(&thread2, NULL, calculoGastos, NULL);
    //Comprueba que los hilos se han creado correctamente. Cuando != 0 muestra el error
    if(iret1 || iret2){
        printf("Error, el valor de pthread_create() es %i para iret1, y %i para iret2\n", iret1, iret2);
    }

    printf("4----------------\n");
    //Utiliza el metodo join para esperar a que se termine la ejecucion de los hilos.
    pthread_join(thread1, (void**) &tmpIngresos);
    pthread_join(thread2, (void**) &tmpGastos);

    printf("5----------------\n");
    //Se imprime por pantalla los resultados de las operaciones entre hilos
    printf("Tenemos %i de ingresos\n", *tmpIngresos);
    printf("Tenemos %i de gastos\n", *tmpGastos);
    printf("Resultado -> (%i - %i) = %i\n", *tmpIngresos, *tmpGastos, *(tmpIngresos) - *(tmpGastos));
    //Cierra los hilos.
    pthread_exit(NULL);
}