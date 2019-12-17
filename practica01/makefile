#DEBUG  =  -g 
DEBUG   =  
FLAGS  = -Wall -march=x86-64
#FLAGS  = -Wall -march=x86-64 -O3  
CC     = gcc
INC    = -I.
LIB    = -L. -L/usr/lib64 -pthread -lm

default : 
	@echo "make programa" 

#-------------------------------------------------------------------------------
hello : hello.c 
	$(CC) $(DEBUG) $(FLAGS) -o hello hello.c  $(INC) $(LIB)

#-------------------------------------------------------------------------------
join : join.c 
	$(CC) $(DEBUG) $(FLAGS) -o join join.c  $(INC) $(LIB)

#-------------------------------------------------------------------------------
ejercicio1 : ejercicio1.c 
	$(CC) $(DEBUG) $(FLAGS) -o ejercicio1 ejercicio1.c  $(INC) $(LIB)

#-------------------------------------------------------------------------------
ejercicio2 : ejercicio2.c 
	$(CC) $(DEBUG) $(FLAGS) -o ejercicio2 ejercicio2.c  $(INC) $(LIB)

#-------------------------------------------------------------------------------
ejercicio3 : ejercicio3.c 
	$(CC) $(DEBUG) $(FLAGS) -o ejercicio3 ejercicio3.c  $(INC) $(LIB)

#-------------------------------------------------------------------------------
ejercicio4 : ejercicio4.c 
	$(CC) $(DEBUG) $(FLAGS) -o ejercicio4 ejercicio4.c  $(INC) $(LIB)

#-------------------------------------------------------------------------------				
clean:
	rm -f *.o *~ *.bck *.bak 

#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
