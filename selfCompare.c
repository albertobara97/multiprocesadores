#include <stdio.h>
#include <pthread.h>

void* check(void* arg){
	pthread_t my_id = pthread_self();
	if(pthread_equal(my_id, *((pthread_t*) arg))){
		printf("Soy yo!\n");
	}else{
		printf("Error\n");
	}
	return 0;
}

int main(int argc, char* argv[]){
	pthread_t thread;
	pthread_create(&thread, 0, check, &thread);
	pthread_join(thread, 0);
	return 0;
}
