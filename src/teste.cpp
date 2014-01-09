#include "libhash.h"
#include <time.h>

#define NUMTHREADS 8
#define TIMES 1

int main(int argc, char *argv[])
{
	Chash<int> hash((unsigned int)(8),(unsigned int)(1));

	srand(time(NULL));
	
	pthread_t w[NUMTHREADS];

	for (int j = 0; j < TIMES; j++)
	{
		for (int i = 0; i < NUMTHREADS; i++)
		{
			pthread_create(&w[i], NULL, Chash<int>::ADD, new Argument<int>((unsigned int)(i), rand(), &hash));
		}

		for (int i = 0; i < NUMTHREADS; i++) pthread_join(w[i], NULL);
	}

	hash._printall();

	pthread_create(&w[5], NULL, Chash<int>::SET, new Argument<int>((unsigned int)(5), 14, &hash));
	pthread_join(w[5], NULL);

	hash._printall();
	
	system("pause");

	return 0;
}