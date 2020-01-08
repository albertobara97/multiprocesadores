#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>

#define N 10
#define TAG 1

void generarvalores (int *ingresos, int *gastos){
	
	int i;

	for (i=0; i<N; i++) {

		printf("Ingresos | Gastos del cliente %d\n", i+1);
		ingresos[i] = (double)((rand()) % 100);
		gastos[i] = (double)((rand()) % 100);
		printf("   %d\t |   %d\n", ingresos[i], gastos[i]);
	}
}

double calcularsuma (int *a, int num){
	
	double total;
	

	for (int i=0; i<num; i++){
	
		total = total + a[i];
	}

	return total;
}

int main (int argc, char *argv[]){
	
	int ingresos[N];
	int gastos[N];	
	int size, rank, ini, repartoNucleo, resto, numelementos;
	double sumaIngresos, sumaGastos, totalIngresos, totalGastos, total;
	MPI_Status status;

	MPI_Init (&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &size);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	
	if (rank == 0){
	
		srand(time(NULL));

		//Generamos valores aleatorios		
		generarvalores(ingresos, gastos);

		//Repartimos valores entre nucleos
		repartoNucleo = N / size;
		resto = N % size;
		ini = repartoNucleo;
	
		for (int i=1; i<size; i++){
	
			numelementos = repartoNucleo;
	
			if (resto > 0){
	
				numelementos++;
				resto--;
			}
	
			MPI_Send (&numelementos, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
			MPI_Send (&ingresos[ini], numelementos, MPI_INT, i, TAG, MPI_COMM_WORLD);
			MPI_Send (&gastos[ini], numelementos, MPI_INT, i, TAG, MPI_COMM_WORLD);
			
			ini = ini + numelementos;
		}
	
		numelementos = repartoNucleo;
	
		sumaIngresos = calcularsuma (ingresos, numelementos);
		sumaGastos = calcularsuma (gastos, numelementos);

	}else {
	
		MPI_Recv (&numelementos, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
		MPI_Recv (ingresos, numelementos, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
		MPI_Recv (gastos, numelementos, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
	}
	

	
	if (rank == 0){
	
		totalIngresos = sumaIngresos;
		totalGastos = sumaGastos;
	
		for (int i=1; i<size; i++){
	
			MPI_Recv (&sumaIngresos, 1, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD, &status);
			MPI_Recv (&sumaGastos, 1, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD, &status);
			totalIngresos = totalIngresos + sumaIngresos;
			totalGastos = totalGastos + sumaGastos;
		}
	
		printf ("\nTotal ingresos: %.2f", totalIngresos);
		printf ("\nTotal gastos: %.2f", totalGastos);
		total = totalIngresos - totalGastos;
		printf ("\nEl balance final es: %.2f\n", total);
	
	}else {
	
		MPI_Send (&sumaIngresos, 1, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
		MPI_Send (&sumaGastos, 1, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
	}

	MPI_Finalize();
}


