#DEBUG  =  -g 
DEBUG   =  
FLAGS  = -Wall -march=x86-64
#FLAGS  = -Wall -march=x86-64 -O3  
CC     = mpicc
INC    = -I.
LIB    = -L. -L/usr/lib64 -pthread -lm

default : 
	@echo "make programa" 

#-------------------------------------------------------------------------------
prueba2 : prueba2.c 
	$(CC) $(DEBUG) $(FLAGS) -o prueba2 prueba2.c  $(INC)