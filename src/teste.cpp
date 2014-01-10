#include "libhash.h"
#include <time.h>

/*
Além da biblioteca, deve ser criado um programa de teste usando pthreads para ilustrar o
funcionamento e testar a biblioteca. Como parâmetros, deve receber o número de threads,
tamanho da tabela, número de blocos e o total de operações a serem
realizadas.
*/

// numero de operações distribuidas e definidas, passagem de parametros corrigida,..
int main(int argc, char *argv[])
{	
	srand(time(NULL));

	int noperations = atoi(argv[3]);
	int NUMTHREADS  = atoi(argv[1]);

	int add_operations 		= ceil(noperations * 40/100);
	int delete_operations 	= ceil(noperations * 30/100);
	int get_operations		= ceil(noperations * 5/100);
	int set_operations 		= ceil(noperations * 15/100);
	int print_operations 	= ceil(noperations * 5/100);
	int printall_operations = ceil(noperations * 5/100);

	int sumOperations = (add_operations + delete_operations+get_operations+set_operations+\
		print_operations+printall_operations);

	add_operations += noperations - sumOperations;
	
	// ver.. nao sei..
	int addCount 		= add_operations 	  % NUMTHREADS;
	int delCount 		= delete_operations   % NUMTHREADS;
	int setCount 		= set_operations 	  % NUMTHREADS;
	int getCount 		= get_operations 	  % NUMTHREADS;
	int printCount 		= print_operations    % NUMTHREADS;
	int printallCount 	= printall_operations % NUMTHREADS;

	pthread_t *t_add 	  = new pthread_t[addCount];
	pthread_t *t_del 	  = new pthread_t[delCount];
	pthread_t *t_get 	  = new pthread_t[setCount];
	pthread_t *t_set 	  = new pthread_t[getCount];
	pthread_t *t_print 	  = new pthread_t[printCount];
	pthread_t *t_printall = new pthread_t[printallCount];

	Chash<int> hash((unsigned int)(atoi(argv[2])),(unsigned int)(atoi(argv[3])));	

	unsigned int keys[add_operations];

	for(int i = 0; i < add_operations; i++)keys[i] = rand();

	// nao sei se ta certo essa passagem no add... e ver se sempre vai começar em 0...	
	for (int i = 0; i < addCount; i++)pthread_create(&t_add[i], NULL, Chash<int>::ADD, new Argument<int>((unsigned int)(keys[i]),rand(), &hash));	
	for (int i = 0; i < delCount; i++)pthread_create(&t_del[i], NULL, Chash<int>::DELETE, new Argument<int>((unsigned int)(keys[i]), &hash));	
	for (int i = 0; i < setCount; i++)pthread_create(&t_set[i], NULL, Chash<int>::,SET, new Argument<int>((unsigned int)(keys[i]),rand(), &hash));	
	
	// get tem que usar vetor.. ,ver
	for (int i = 0; i < getCount; i++)pthread_create(&t_get[i], NULL, Chash<int>::GET, new Argument<int>((unsigned int)(keys[i]),&hash));	
	for (int i = 0; i < printCount; i++)pthread_create(&t_print[i], NULL, Chash<int>::PRINT, new Argument<int>((unsigned int)(keys[i]), &hash));	
	for (int i = 0; i < printallCount; i++)pthread_create(&t_printall[i], NULL, Chash<int>::PRINTALL, new Argument<int>(&hash));

	for (int i = 0; i < addCount; i++)pthread_join(t_add[i], NULL);	
	for (int i = 0; i < delCount; i++)pthread_join(t_del[i], NULL);	
	for (int i = 0; i < setCount; i++)pthread_join(t_set[i], NULL);	
	for (int i = 0; i < getCount; i++)pthread_join(t_get[i], NULL);	
	for (int i = 0; i < printCount; i++)pthread_join(t_print[i], NULL);	
	for (int i = 0; i < printallCount; i++)pthread_join(t_printall[i], NULL);

	delete[] t_add; 	  
	delete[] t_del;	  
	delete[] t_get; 	  
	delete[] t_set;	  
	delete[] t_print; 	 
	delete[] t_printall;

	return EXIT_SUCCESS;
}