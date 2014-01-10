#include "libhash.h"
#include <time.h>

#define NUMTHREADS 16
#define TIMES 1

int main(int argc, char *argv[])
{


	
	Chash<int> hash((unsigned int)(8),(unsigned int)(1));

	srand(time(NULL));
	
	pthread_t  w[NUMTHREADS];
	pthread_t  p[NUMTHREADS];

	pthread_t  s[3];
	pthread_t _g[3];

	Argument<int> *g = new Argument<int>[3];

	g[0].h = &hash;
	g[1].h = &hash;
	g[2].h = &hash;

	g[0].key = 5;	
	g[1].key = 6;
	g[2].key = 9;

	for (int i = 0; i < NUMTHREADS; i++)pthread_create(&w[i], NULL, Chash<int>::ADD, new Argument<int>((unsigned int)(i), rand(), &hash));
	
	for (int i = 0; i < NUMTHREADS; i++) pthread_join(w[i], NULL);
	
	hash._printall();
	

	/*


	for (int i = 0; i < NUMTHREADS; i++)
	{
		pthread_create(&p[i], NULL, Chash<int>::PRINT, new Argument<int>((unsigned int)(i), &hash));
		//sleep(1);
	}
	for (int i = 0; i < NUMTHREADS; i++) pthread_join(p[i], NULL);

	hash._printall();

	pthread_create(&s[0], NULL, Chash<int>::SET, new Argument<int>((unsigned int)(5),14,&hash));
	pthread_create(&s[1], NULL, Chash<int>::SET, new Argument<int>((unsigned int)(6),14,&hash));
	pthread_create(&s[2], NULL, Chash<int>::SET, new Argument<int>((unsigned int)(9),14,&hash));
	

	hash._printall();

	cout << "\n" << g[0].data << " "<< g[1].data << " "<< g[2].data << " " << endl; 
	
	*/
	return 0;
}