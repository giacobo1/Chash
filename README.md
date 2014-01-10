Chash
=====
Universidade Federal de Pelotas
Bacharelado em Ciência da Computação

Bruno G. Pinto 		- 11107588 - bgpinto@inf.ufpel.edu.br
Vinícius R. Santos 	- 11105054 - vrdsantos@inf.ufpel.edu.br

Link para o repositório utilizado: https://github.com/giacobo1/Chash

1 # Considerações Gerais

A biblioteca está toda implementada dentro do arquivo libhash.h por razão do problema de instanciação de templates no c++. Achamos mais conveniente resolver desta forma, pois fica mais fácil de utilizar a biblioteca com qualquer tipo de dado. Existem três classes auxiliares que separamos para melhorar a organização da implementação:

SemList:

A classe SemList é utilizada para abstrair e dar flexibilidade a sincronização dos blocos da hash. Esta classe implementa uma lista encadeada de semaforos, que cresce dinâmicamente conforme a necessidade.

Entry:

A classe Entry é utilizada para abstrair o tipo de dado que será parametrizado e irá compor a tabela hash em si. 

Argument:

A classe Argument é utilizada como interface para passagem de parâmetros para que as threads pudessem manipular um objeto da classe Chash.

Chash:

Esta é a classe que implementa a tabela hash. Para que as threads manipulassem um objeto da classe Chash, usamos métodos estáticos da classe para ser usado na criação das threads, visto que a chamada da API pthread não aceita funções membro. Dentro destes métodos estáticos é feito o casting do objeto passado e, finalmente, ocorre a passagem de parâmetro para a chamada de um método público do objeto referenciado. A estratégia é usar as funções estáticas como "wrappers" para a criação das threads e o casting.

A sincronizaçao é feita entre blocos, onde cada nodo da lista de semáforos (chamada blockSems) representa um bloco. Quando ocorre colisão a thread libera o semáforo do bloco atual e entra no semáforo do índice do bloco calculado pela função de rehash. Existe, ainda, outra lista de semáforo de atividades que representa as threads que estão trabalhando no momento, esta lista é utilizada para a sincronização de realocação, pois a thread que irá realocar toda a tabela deve aguardar àquelas threads que estão trabalhando no momento. Para que apenas uma thread realoque a tabela existe um semáforo que, junto com uma flag, garantem a exclusão mútua e a sincronização para que a realocação ocorra.

A tabela hash implementada é do tipo endereçamento aberto e suporta dois tipos de tratamento de colisão, são eles: tratamento de colisão linear (setando a macro PROB = 0) e tratamento de colisão quadrática (setando a macro PROB = 1).

2 # Instruções para Compilar e Rodar

Para compilar o programa teste, basta entrar na pasta ../Chash/src/ e digitar o comando: make.

Para rodar o programa teste por meio do make, basta entrar na pasta ../Chash/src/ e digitar o comando: 

make test x=value m=value n=value op=value 

Onde x é o número de threads, m é o tamanho da tabela, n é o número de blocos, op é o número de operações e value é um valor arbitrário.

Para rodar o programa teste sem usar o make, basta entrar na pasta ../Chash/src/ e digitar o comando:

	./teste x m n op

Onde x é o número de threads, m é o tamanho da tabela, n é o número de blocos, op é o número de operações e value é um valor arbitrário.

Para limpar os arquivos objeto gerados na compilação, basta entrar na pasta ../Chash/src/ e digitar o comando:

	make clear_o


Para limpar o arquivo executável gerado na compilação, basta entrar na pasta ../Chash/src/ e digitar o comando:

	make clear_exe	

Para limpar o arquivo executável e os arquivos objeto gerados na compilação, basta entrar na pasta ../Chash/src/ e digitar o comando:

	make clear_all


3 # Descrição do arquivo teste		

A proposta do arquivo teste é demonstrar como utilizar todas as funções da biblioteca, bem como a passagem de parâmetros. Para manipular o objeto Chash utilizando threads, deve-se seguir as chamadas de função e criação de objetos da mesma forma como apresentado neste arquivo fonte.

Neste teste, optou-se por particionar as operações de modo que se faça 40% de inserções, 30% de delete, 10% de gets, 15% de set, 5% print e , no final, imprimimos a tabela, de forma que as operações manipulem a tabela hash concorrentemente e o mecanismo de sincronização seja testado de forma coerente.

    