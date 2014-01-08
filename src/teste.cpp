#include "libhash.h"
#include <time.h>

#define NUMTHREADS 64

int main(int argc, char *argv[])
{
	Chash<int> hash((unsigned int)(8),(unsigned int)(1));

	srand(time(NULL));
	
	pthread_t w[NUMTHREADS];

	unsigned int *keys = new unsigned int[NUMTHREADS];
	unsigned int *data = new unsigned int[NUMTHREADS];

	for (int i = 0; i < NUMTHREADS; i++)
	{
		keys[i] = (unsigned int)(rand() % 10000);
		data[i] = (unsigned int)(rand() % 10000);
	}
	
	for (int i = 0; i < NUMTHREADS / 2; i++)
	{
		pthread_create(&w[i], NULL, Chash<int>::ADD, new Argument<int>(keys[i], data[i], &hash));
	}

	printf("\n\n");

	for (int i = NUMTHREADS / 2; i < NUMTHREADS; i++)
	{
		printf("%d\t%d\n", hash._get(keys[i - NUMTHREADS / 2]), data[i - NUMTHREADS / 2]);
		pthread_create(&w[i], NULL, Chash<int>::ADD, new Argument<int>((unsigned int)(rand()), rand(), &hash));
	}

	for (int i = 0; i < NUMTHREADS; i++) pthread_join(w[i],NULL);
	
	hash._printall();

	system("pause");

	return 0;
}