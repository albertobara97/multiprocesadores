CC     = gcc
CCMPI = mpicc
FLAGS  = -Wall
default : main
LIB    = -L. -L/usr/lib64 -pthread -lm

#-------------------------------------------------------------------------------
main : makefile sinbordes.c pgm.o
	$(CC) sinbordes.c pgm.o -o sinbordes

pgm.o : pgm.c pgm.h
	$(CC) -c -o pgm.o pgm.c

#-------------------------------------------------------------------------------

mpi : makefile versionMpi.c pgm.o
	$(CCMPI) $(FLAGS) versionMpi.c pgm.o -o mpi
#-------------------------------------------------------------------------------

hilos : makefile versionHilos.c pgm.o
	$(CC) $(FLAGS) versionHilos.c pgm.o -o hilos $(LIB)

#-------------------------------------------------------------------------------
clean:
	rm -f *.o *~ *.bck *.bak sinbordes
