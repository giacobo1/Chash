// TODO: Testar no Linux.
// TODO: Implementar as demais operações além da inserção.
//
// V PRINTALL (Imprimir tabela)
// V GET (Buscar elemento)
// X SET (Modificar elemento)
// X DELETE (Deletar elemento) Deletar elemento
//
// NOTE: Problemas com 4096 threads simultâneas no Windows.
// NOTE: Windows 8 por si só normalmente não ultrapassa 1000 threads de sistema.

#ifndef LIBHASH_H
#define LIBHASH_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <pthread.h>
#include "semlist.h"
#include "entry.h"

using namespace std;

#define PROB 1
//#define DEBUG

template<class Type>class Chash
{
	unsigned int size;
	unsigned int nBlocks;
	unsigned int insertions;
	unsigned int collisionCounter;
	unsigned int allocateCounter;

	Entry<Type> *table;
	
	SemList blockSems;
	SemList activitySems;

	sem_t allocateMut;
	sem_t allocateSem;

	bool allocating;

	int rehashFunc(unsigned int key, int probing, int *rhst);
	void realloc_add(unsigned int key, Type d);
	Entry<Type> *search(unsigned int key, bool getSemaphores = true);

public:

	Chash(void);
	Chash(const Chash& h);
	Chash(unsigned int m,unsigned int n);
	~Chash(void);

	Chash& operator = (const Chash& h)
	{
		this->nBlock = h.nBlock;
		this->size  = h.size;

		for (unsigned int i = 0; i < this->size; i++) table[i] = h.table[i];

		return *this;
	}


	//
	void _add(unsigned int k, Type n);
    void _delete(unsigned int k);
    Type _get(unsigned int k);
    void _set(unsigned int k, Type n);
    void _print(unsigned int k);
    void _printall(void);

    //
    static void *ADD(void *y);
    static void *DELETE(void *y);
    static void *GET(void *y);
    static void *SET(void *y);
    static void *PRINT(void *y);
    static void *PRINTALL(void *y);
};
//

template <class Type>class Argument
{

public:

	Type data;
	unsigned int key;
	Chash<Type> *h;

	Argument(unsigned int k, Type d, Chash<Type> *x)
	{
		key = k;
		data = d;
		h = x;
	}
};


//
template<class Type>
Chash<Type>::Chash(void)
{
	Chash(2, 4);
}

template<class Type>
Chash<Type>::Chash(const Chash& h)
{
	this->nBlock = h.nBlock;
	this->size   = h.size;

	for (unsigned int i = 0; i < this->size; i++) table[i] = h.table[i];
}

template<class Type>
Chash<Type>::Chash(unsigned int m, unsigned int n)
{
	this->nBlocks			= n;
	this->size				= m * n;
	this->insertions		= 0;
	this->allocateCounter	= 0;
	this->allocating		= false;
	this->collisionCounter	= 0;
	
	sem_init(&allocateMut, 0, 1);
	
	sem_init(&allocateSem, 0, 0);

	for (unsigned int i = 0; i < n; i++)
	{
		blockSems.addSemaphore();
		activitySems.addSemaphore();
	}

	table = new Entry<Type>[size];
}

//
// ===================== façamos depois =======================
//
template<class Type>
Chash<Type>::~Chash(void)
{
	//delete[] table;
}


// ======================================//

template<class Type>
int Chash<Type>::rehashFunc(unsigned int key, int probing, int *rhst)
{
	unsigned int rehashStart = key % size;
    unsigned int aux;

    *rhst += 1;

    if (probing == 0)
    {
       aux = *rhst;
    }
    else if (probing == 1)
    {
        aux = (((*rhst) * (*rhst)) + (*rhst)) / 2;
    }   

    return (rehashStart + aux) % size;
}

template<class Type>
void Chash<Type>::realloc_add(unsigned int key, Type d)
{
	int index;
	int it = 0;

	index = key % size;

	while (true)
	{
		if (!table[index].getOcupied())
		{
			table[index].setKey(key);
			table[index].setData(d);
			table[index].setOcupied(true);

			break;
		}
		else
		{
			index = rehashFunc(key, PROB, &it);
		}
	}
}

template<class Type>
Entry<Type> *Chash<Type>::search(unsigned int key, bool getSemaphores = true)
{
	Entry<Type> *_return = NULL;

	int index;
	int oldIndex;
	int blockIndex;
	int oldBlockIndex;
	int rehashIteraction_t;
	int oldSize;
	int value;

	sem_wait(&allocateMut);
	sem_getvalue(&allocateMut, &value);

	for (int i = value; i < 1; i++)
	{
		sem_post(&allocateMut);
	}

	oldSize = size;
	index = key % size;

	oldIndex = index;

	blockIndex = index % nBlocks;
	oldBlockIndex = blockIndex;

	rehashIteraction_t = -1;

	if (getSemaphores)
	{
		sem_wait(blockSems.getSemaphore(blockIndex));
		sem_wait(activitySems.getSemaphore(blockIndex));
	}

	while (true)
	{
		oldIndex = index;
        index = rehashFunc(key, PROB, &rehashIteraction_t);
		oldBlockIndex = blockIndex;
		blockIndex = index % nBlocks;

		if (oldBlockIndex != blockIndex)
		{
			sem_post(activitySems.getSemaphore(oldBlockIndex));
			sem_post(blockSems.getSemaphore(oldBlockIndex));
			sem_wait(blockSems.getSemaphore(blockIndex));
			sem_wait(activitySems.getSemaphore(blockIndex));
		}

		if (oldSize != size)
		{
			oldSize = size;
			continue;
		}

		if (table[index].getOcupied())
        {
            if (table[index].getKey() == key)
            {
				_return = &table[index];
            	break;
            }
            else
			{
				continue;
			}
        }
		else if (table[index].getErased())
		{
			continue;
		}
		else
		{
			break;
		}
	}

	if (getSemaphores)
	{
		sem_post(activitySems.getSemaphore(blockIndex));
		sem_post(blockSems.getSemaphore(blockIndex));
	}

	return _return;
}

template<class Type>
void Chash<Type>:: _add(unsigned int k, Type n)
{
	float loadFactor;
	int value;
	int index;
	int blockIndex;
	int rehashIteraction_t;
	int oldSize;
	int a = 1, b = 2;
	sem_t *elem = NULL;

	Entry<Type> *oldTable;

	int oldBlockIndex;
	int oldIndex;

	sem_wait(&allocateMut);
	sem_getvalue(&allocateMut, &value);

	for (int i = value; i < 1; i++)
	{
		sem_post(&allocateMut);
	}

	oldSize = size;
	index = k % size;

	oldIndex = index;

	blockIndex = index % nBlocks;
	oldBlockIndex = blockIndex;

	rehashIteraction_t = -1;

	#ifdef DEBUG
	elem = blockSems.getSemaphore(blockIndex);
	if (elem == NULL) printf("NULL POINTER\n");
	#endif

	sem_wait(blockSems.getSemaphore(blockIndex));

	#ifdef DEBUG
	printf("LOCK index: %d block: %d \n", index, blockIndex);
	sem_getvalue(activitySems.getSemaphore(blockIndex), &value);
	#endif
	
	#ifdef DEBUG
	elem = activitySems.getSemaphore(blockIndex);
	if (elem == NULL) printf("NULL POINTER\n");
	#endif

	sem_wait(activitySems.getSemaphore(blockIndex));

	#ifdef DEBUG
	sem_getvalue(activitySems.getSemaphore(blockIndex), &value);
	printf("WORKING index: %d block: %d \n", index, blockIndex);
	#endif

	while (true)
	{
		oldIndex = index;
        index = rehashFunc(k, PROB, &rehashIteraction_t);
		oldBlockIndex = blockIndex;
		blockIndex = index % nBlocks;

		if (oldBlockIndex != blockIndex)
		{

			#ifdef DEBUG
			printf("UNLOCK index: %d block: %d \n", oldIndex, oldBlockIndex);
			#endif

			sem_post(activitySems.getSemaphore(oldBlockIndex));
			sem_post(blockSems.getSemaphore(oldBlockIndex));
			sem_wait(blockSems.getSemaphore(blockIndex));
			sem_wait(activitySems.getSemaphore(blockIndex));

			#ifdef DEBUG
			printf("LOCK index: %d block: %d \n", index, blockIndex);
			#endif

		}

		if (oldSize != size)
		{
			oldSize = size;
			continue;
		}

		if (table[index].getOcupied())
        {
            if(table[index].getKey() == k && table[index].getData() == n)
            {

				#ifdef DEBUG
				printf("ALREADY HERE index: %d block: %d \n", index, blockIndex);
				#endif

            	break;
            }
            else
            {
            	collisionCounter++;

				#ifdef DEBUG
				printf("COLLISION index: %d block: %d \n", index, blockIndex);
				#endif

				continue;
            }
        }
		else
        {
			#ifdef DEBUG
			printf("ADD index: %d block: %d \n", index, blockIndex);
			#endif

            table[index].setKey(k);
            table[index].setData(n);
            table[index].setOcupied(true);
            
            insertions++;

            break;
        }
	}

	#ifdef DEBUG
	printf("UNLOCK index: %d block: %d \n", index, blockIndex);
	#endif

	sem_post(activitySems.getSemaphore(blockIndex));
	sem_post(blockSems.getSemaphore(blockIndex));

	#ifdef DEBUG
	printf("NOT WORKING index: %d block: %d \n", index, blockIndex);
	#endif
	

	sem_wait(&allocateMut);

	loadFactor = (float)insertions / (float)size;

	while (loadFactor > 0.75)
	{
		allocating = true;

		#ifdef DEBUG
		printf("ALLOCATE LOCK index: %d block: %d \n", index, blockIndex);
		#endif

		for (unsigned int i = 0; i < nBlocks; i++)
		{

			#ifdef DEBUG
			sem_getvalue(activitySems.getSemaphore(i), &value);
			#endif

			sem_wait(activitySems.getSemaphore(i));

			#ifdef DEBUG
			sem_getvalue(activitySems.getSemaphore(i), &value);
			#endif
		}

		size *= 2;
		nBlocks *= 2;

		oldTable = table;
		table = new Entry<Type>[size];

		for (unsigned int i = 0; i < size / 2; i++)
		{
			if (oldTable[i].getOcupied())
			{
				realloc_add(oldTable[i].getKey(), oldTable[i].getData());
			}
		}


		if ((unsigned int)blockSems.getSize() < nBlocks)
		{
			for (unsigned int i = 0; i < nBlocks / 2; i++)
			{
				blockSems.addSemaphore();
				activitySems.addSemaphore();
			}
		}


		allocating = false;

		sem_getvalue(&allocateSem, &value);

		for (int n = value; n < 0; n++)
		{
			sem_post(&allocateSem);
		}

		for (unsigned int i = 0; i < nBlocks / 2; i++)
		{

			#ifdef DEBUG
			sem_getvalue(activitySems.getSemaphore(i), &value);
			#endif

			sem_post(activitySems.getSemaphore(i));

			#ifdef DEBUG
			sem_getvalue(activitySems.getSemaphore(i), &value);
			#endif
		}

		#ifdef DEBUG
		printf("ALLOCATE UNLOCK index: %d block: %d insertions: %d \n", index, blockIndex, insertions);
		#endif

		allocateCounter++;

		delete [] oldTable;

		break;
	}

	sem_getvalue(&allocateMut, &value);

	for (int n = value; n < 1; n++)
	{
		sem_post(&allocateMut);
	}

}

template<class Type>
void Chash<Type>:: _delete(unsigned int k)
{

}

template<class Type>
Type Chash<Type>:: _get(unsigned int k)
{
	Type data;
	Entry<Type> *entry = NULL;

	entry = search(k);

	if (entry != NULL)
	{
		data = entry->getData();
	}

	return data;
}

template<class Type>
void Chash<Type>:: _set(unsigned int k, Type n)
{

}

template<class Type>
void Chash<Type>:: _print(unsigned int k)
{

}


template<class Type>
void Chash<Type>:: _printall(void)
{
	for (unsigned int i = 0; i < nBlocks; i++)
	{
		sem_wait(activitySems.getSemaphore(i));
	}

	printf("\n\n================ [ Tabela ] ================\n\n");
	/*for(unsigned int i = 0; i < this->size; i++)
		cout << "index: " << i << "   key: " << table[i].getKey() << "\tdata: " << table[i].getData() << endl;*/

	
	printf(
			"\nSize: %d \nNBlocks: %d \nAlocations: %d \nColisions: %d \nTotal insertions: %d\n"\
			, size, nBlocks, allocateCounter, collisionCounter, insertions
			);

	for (unsigned int i = 0; i < nBlocks; i++)
	{
		sem_post(activitySems.getSemaphore(i));
	}
}



// =====================================/

template<class Type>
void* Chash<Type>::ADD(void *y)
{

	Argument<Type> *a;

	a = static_cast< Argument<Type> * > (y);

	a->h->_add(a->key,a->data);

	pthread_exit(NULL);

	return NULL;
}

template<class Type>
void* Chash<Type>::DELETE(void *y)
{
	pthread_exit(NULL);
}

template<class Type>
void* Chash<Type>::GET(void *y)
{
	Argument<Type> *a;
	Type *data;

	a = static_cast< Argument<Type> * > (y);

	data = a->h->_get(a->key);

	a->data = data();

	pthread_exit(NULL);

	return NULL;
}

template<class Type>
void* Chash<Type>::SET(void *y)
{
	pthread_exit(NULL);
}

template<class Type>
void* Chash<Type>::PRINT(void *y)
{
	pthread_exit(NULL);
}

template<class Type>
void* Chash<Type>::PRINTALL(void *y)
{
	Argument<Type> *a;

	a = static_cast< Argument<Type> * > (y);

	a->h->_printall();

	pthread_exit(NULL);

	return NULL;
}

//*/

#endif
