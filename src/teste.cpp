#include "libhash.h"
#include <time.h>

#define NUMTHREADS 64

int main(int argc, char *argv[])
{
	pthread_t w[NUMTHREADS];
	pthread_t printWorker;

	Chash<int> hash((unsigned int)(8),(unsigned int)(1));

	srand(time(NULL));
	
	for(int i = 0; i < NUMTHREADS / 2; i++)
	{
		pthread_create(&w[i], NULL, Chash<int>::ADD, new Argument<int>((unsigned int)(rand()), rand(), &hash));
	}

	pthread_create(&printWorker, NULL, Chash<int>::PRINTALL, new Argument<int>(0, 0, &hash));
	pthread_join(printWorker, NULL);

	for(int i = NUMTHREADS / 2; i < NUMTHREADS; i++)
	{
		pthread_create(&w[i], NULL, Chash<int>::ADD, new Argument<int>((unsigned int)(rand()), rand(), &hash));
	}

	for (int i = 0; i < NUMTHREADS; i++) pthread_join(w[i],NULL);
	
	hash._printall();

	system("pause");

	return 0;
}