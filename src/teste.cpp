#include "libhash.h"


//receber o número de threads, tamanho da tabela, 
//número de blocos e o total de operações
int main(int argc, char **argv)
{

	
	if(argc > 1)
	{
		Chash<int> h(atoi(argv[2]),atoi(argv[3]));

		h._printall();


	}
	else
		printf("Erro na passagem de argumentos.\n");


	return EXIT_SUCCESS;
}