#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>
#include<stdbool.h>

#define TAG 1

void generarvalores (double *vector, int total){
	
    srand(time(NULL));

    printf("Numeros generados. %d\n", total);

    for (int i=0; i<total; i++) {

		vector[i] = (double)((rand()) % 10000) / 100;
		
        if((i%10 == 0 || i == total-1) && i != 0 ){
            printf("%.2f\n", vector[i]);
        }else{
            printf("%.2f  |   ", vector[i]);
        }

	}
}

int main (int argc, char *argv[]){
	
	int size, rank;
    
    bool flag = true;
    int total;
    sscanf (argv[1], "%d", &total);

    MPI_Init (&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &size);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);

    if(rank == 0){
        
        double vector[total];
        generarvalores(vector, total);

        MPI_Send (&vector, 1, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
    }else{
        for(1...size){

        }
    }
    

    MPI_Finalize();
}