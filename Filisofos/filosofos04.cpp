#include<stdio.h>
#include<pthread.h>

// gcc filosofos04.cpp -pthread

#define NUM_FILOSOFOS 5

void *filosofo (void *arg);

int main(void)
{
	//char filo01[20] = "Sócrates";
	//char filo02[20] = "Pitágoras";

	pthread_t filosofos[NUM_FILOSOFOS];
	int identificadores[NUM_FILOSOFOS];

	int i;

	for (i=0; i<NUM_FILOSOFOS; i++)	{
		identificadores[i] = i;
		pthread_create (&filosofos[i], NULL, &filosofo, &identificadores[i]);
	}


	for (i=0; i<NUM_FILOSOFOS; i++)	{
		pthread_join(filosofos[i], NULL);
	}

	return 0;
}

void *filosofo (void *arg) 
{
	int arg2 = *((int *)arg);
	for (int i=1; true; i++) {
		printf ("Filósofo-%d estoy comiendo : %d \n", arg2, i);
	}
}
