#include "libhash.h"
#include <time.h>

/*
Além da biblioteca, deve ser criado um programa de teste usando pthreads para ilustrar o
funcionamento e testar a biblioteca. Como parâmetros, deve receber o número de threads,
tamanho da tabela, número de blocos e o total de operações a serem
realizadas.
*/

// fazer readme e arrumar makefile...
// numero de operações distribuidas e definidas, passagem de parametros corrigida,..
//
// ./program <nthreads> <tabletam> <nblocks> <noperations>
//
int main(int argc, char *argv[])
{	
		
	if (argc < 5)
	{
		printf("Erro na passagem de parametros.\n");
		exit(1);
	}
	
	srand((unsigned int)time(NULL));

	int NUMTHREADS			= atoi(argv[1]);
	unsigned int m			= atoi(argv[2]);
	unsigned int n			= atoi(argv[3]);
	int noperations			= atoi(argv[4]);

	if (m < n)
	{
		printf("Erro, numero de blocos maior do que a tabela.\n");
		exit(1);
	}
	
	if (NUMTHREADS < 0 || noperations < 0)
	{
		printf("Erro, parâmetros negativos.\n");
		exit(1);
	}

	if (NUMTHREADS > noperations)
	{
		printf("Erro, numero de threads maior que operacoes.\n");
		exit(1);
	}

	int add_operations 		= (int)ceil((double)noperations * 40.0 / 100.0);
	int delete_operations 	= (int)ceil((double)noperations * 30.0 / 100.0);
	int get_operations		= (int)ceil((double)noperations * 10.0 / 100.0);
	int set_operations 		= (int)ceil((double)noperations * 15.0 / 100.0);
	int print_operations 	= (int)ceil((double)noperations * 5.0 / 100.0);
	int printall_operations = (int)ceil((double)noperations * 0.0 / 100.0);

	int sumOperations = (add_operations + delete_operations+get_operations+set_operations+\
		print_operations+printall_operations);

	add_operations += noperations - sumOperations;

	int addStart 		= 0;
	int delStart 		= addStart + add_operations;
	int setStart 		= delStart + delete_operations;
	int getStart 		= setStart + set_operations;
	int printStart 		= getStart + get_operations;
	int printallStart 	= printStart + print_operations;

	pthread_t *workers = new pthread_t[NUMTHREADS];

	Chash<int> hash(m, n);

	unsigned int *keys = new unsigned int[add_operations];

	for(int i = 0; i < add_operations; i++) keys[i] = rand();

	int times = (int)ceil((double)noperations / (double)NUMTHREADS);
	int operations = 0;

	for (int i = 0; i < times; i++)
	{
		for (int j = 0; j < NUMTHREADS; j++)
		{
			if (operations >= printallStart)
			{
				//pthread_create(&workers[j], NULL, Chash<int>::PRINTALL, new Argument<int>(&hash));
			}
			else if (operations >= printStart)
			{
				pthread_create(&workers[j], NULL, Chash<int>::PRINT, new Argument<int>((unsigned int)(keys[operations - printStart]), &hash));
			}
			else if (operations >= getStart)
			{
				Argument<int> arg(keys[operations - getStart], &hash);

				pthread_create(&workers[j], NULL, Chash<int>::GET, &arg);
				pthread_join(workers[j], NULL);

				cout << "GET Key: " << arg.key << "\tData: " << arg.data << endl;
			}
			else if (operations >= setStart)
			{
				pthread_create(&workers[j], NULL, Chash<int>::SET, new Argument<int>((unsigned int)(keys[operations - setStart]), rand(), &hash));
			}
			else if (operations >= delStart)
			{
				pthread_create(&workers[j], NULL, Chash<int>::DELETE, new Argument<int>((unsigned int)(keys[operations - delStart]), &hash));
			}
			else if (operations >= addStart)
			{
				pthread_create(&workers[j], NULL, Chash<int>::ADD, new Argument<int>((unsigned int)(keys[operations]), rand(), &hash));
			}

			operations += 1;
		}

		for (int j = 0; j < NUMTHREADS; j++)
		{
			pthread_join(workers[j], NULL);
		}
	}

	hash._printall();
	
	return EXIT_SUCCESS;
}