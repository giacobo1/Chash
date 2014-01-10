// TODO: tudo que ta escrito no .cpp
// TODO: mais testes

#ifndef LIBHASH_H
#define LIBHASH_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <pthread.h>
#include <cmath>
#include "semlist.h"
#include "entry.h"


using namespace std;

#define PROB 0

template<class Type>class Chash
{
	unsigned int size;
	unsigned int nBlocks;
	unsigned int insertions;
	unsigned int collisionCounter;
	unsigned int allocateCounter;

	unsigned int allocSize;
	unsigned int allocNBlocks;

	Entry<Type> *table;
	
	SemList blockSems;
	SemList activitySems;

	sem_t allocateMut;
	sem_t allocateSem;

	bool allocating;
	bool deallocating;

	int rehashFunc(unsigned int key, int probing, int *rhst, unsigned int tableSize);
	void realloc_add(unsigned int key, Type d);
	Entry<Type> *search(unsigned int key);

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

    inline unsigned int getSize(void)const{return size;}
    inline bool isDeallocating(void)const {return deallocating;}
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

	Argument(void)
	{
		data = (Type)NULL;
		h = NULL;
		key = 0;
	}

	Argument(unsigned int k, Type d, Chash<Type> *x)
	{
		key = k;
		data = d;
		h = x;
	}
	Argument(Chash<Type> *x)
	{
		h = x;
	}

	Argument(unsigned int k, Chash<Type> *x)
	{
		key = k;
		h = x;
	}

	Argument(Type d)
	{
		data = d;
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
	this->deallocating	 	= false;
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
	deallocating = true;

	for (unsigned int i = 0; i < nBlocks; i++)
	{			
		sem_wait(blockSems.getSemaphore(i));
		sem_wait(activitySems.getSemaphore(i));
	}

	delete[] table;

	sem_destroy(&allocateMut);
	sem_destroy(&allocateSem);


}


// ======================================//

template<class Type>
int Chash<Type>::rehashFunc(unsigned int key, int probing, int *rhst, unsigned int tableSize)
{
	unsigned int rehashStart = key % tableSize;
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

    return (rehashStart + aux) % tableSize;
}

template<class Type>
void Chash<Type>::realloc_add(unsigned int key, Type d)
{
	int index;
	int it = 0;

	index = key % allocSize;

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
			index = rehashFunc(key, PROB, &it, allocSize);
		}
	}
}

template<class Type>
Entry<Type> *Chash<Type>::search(unsigned int key)
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

	sem_wait(blockSems.getSemaphore(blockIndex));
	sem_wait(activitySems.getSemaphore(blockIndex));

	while (true)
	{
		oldIndex = index;
        index = rehashFunc(key, PROB, &rehashIteraction_t, size);
		oldBlockIndex = blockIndex;
		blockIndex = index % nBlocks;

		if (oldBlockIndex != blockIndex)
		{
			sem_post(activitySems.getSemaphore(oldBlockIndex));
			sem_post(blockSems.getSemaphore(oldBlockIndex));
			sem_wait(blockSems.getSemaphore(blockIndex));
			sem_wait(activitySems.getSemaphore(blockIndex));
		}

		if ((unsigned int)oldSize != size)
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

	sem_post(activitySems.getSemaphore(blockIndex));
	sem_post(blockSems.getSemaphore(blockIndex));

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
	sem_t *elem = NULL;
	Entry<Type> *oldTable;

	int oldBlockIndex;
	int oldIndex;

	oldSize = size;
	index = k % size;

	oldIndex = index;

	blockIndex = index % nBlocks;
	oldBlockIndex = blockIndex;

	rehashIteraction_t = -1;

	sem_wait(blockSems.getSemaphore(blockIndex));
	sem_wait(activitySems.getSemaphore(blockIndex));

	while (true)
	{
		oldIndex = index;
        index = rehashFunc(k, PROB, &rehashIteraction_t, size);
		oldBlockIndex = blockIndex;
		blockIndex = index % nBlocks;

		if (oldBlockIndex != blockIndex)
		{
			sem_post(activitySems.getSemaphore(oldBlockIndex));
			sem_post(blockSems.getSemaphore(oldBlockIndex));
			sem_wait(blockSems.getSemaphore(blockIndex));
			sem_wait(activitySems.getSemaphore(blockIndex));
		}

		if ((unsigned int)oldSize != size)
		{
			oldSize = size;
			continue;
		}

		if (table[index].getOcupied())
        {
            if(table[index].getKey() == k && table[index].getData() == n)
            {
            	break;
            }
            else
            {
            	collisionCounter++;
				continue;
            }
        }
		else
        {
            table[index].setKey(k);
            table[index].setData(n);
            table[index].setOcupied(true);
        
            insertions++;

            break;
        }
	}
	sem_post(activitySems.getSemaphore(blockIndex));
	sem_post(blockSems.getSemaphore(blockIndex));

	sem_wait(&allocateMut);

	sem_getvalue(&allocateMut, &value);

	loadFactor = (float)insertions / (float)size;

	while (loadFactor > 0.75 && !allocating)
	{
		allocating = true;

		for (unsigned int i = 0; i < nBlocks; i++)
		{
			sem_wait(activitySems.getSemaphore(i));
		}

		allocSize = size * 2;
		
		allocNBlocks = nBlocks * 2;

		oldTable = table;
		table = new Entry<Type>[allocSize];

		for (unsigned int i = 0; i < allocSize / 2; i++)
		{
			if (oldTable[i].getOcupied())
			{
				realloc_add(oldTable[i].getKey(), oldTable[i].getData());
			}
		}


		if ((unsigned int)blockSems.getSize() < allocNBlocks)
		{
			for (unsigned int i = 0; i < nBlocks; i++)
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

		for (unsigned int i = 0; i < nBlocks; i++)
		{

			#ifdef DEBUG
			sem_getvalue(activitySems.getSemaphore(i), &value);
			#endif

			sem_post(activitySems.getSemaphore(i));
		}

		allocateCounter++;

		size = allocSize;
		nBlocks = allocNBlocks;

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
	int index;
	int oldIndex;
	int blockIndex;
	int oldBlockIndex;
	int rehashIteraction_t;
	int oldSize;
	int value;

	bool exists = true;

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

	sem_wait(blockSems.getSemaphore(blockIndex));
	sem_wait(activitySems.getSemaphore(blockIndex));

	while (true)
	{
		oldIndex = index;
        index = rehashFunc(k, PROB, &rehashIteraction_t, size);
		oldBlockIndex = blockIndex;
		blockIndex = index % nBlocks;

		if (oldBlockIndex != blockIndex)
		{
			sem_post(activitySems.getSemaphore(oldBlockIndex));
			sem_post(blockSems.getSemaphore(oldBlockIndex));
			sem_wait(blockSems.getSemaphore(blockIndex));
			sem_wait(activitySems.getSemaphore(blockIndex));
		}

		if ((unsigned int)oldSize != size)
		{
			oldSize = size;
			continue;
		}

		if (table[index].getOcupied())
        {
            if (table[index].getKey() == k)
            {
				table[index].setOcupied(false);
				table[index].setErased(true);
				table[index].setKey(0);
				table[index].setData((Type)NULL);
				insertions-= 1;

            	break;
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

	sem_post(activitySems.getSemaphore(blockIndex));
	sem_post(blockSems.getSemaphore(blockIndex));

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
	int index;
	int oldIndex;
	int blockIndex;
	int oldBlockIndex;
	int rehashIteraction_t;
	int oldSize;
	int value;

	bool exists = true;

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

	sem_wait(blockSems.getSemaphore(blockIndex));
	sem_wait(activitySems.getSemaphore(blockIndex));

	while (true)
	{
		oldIndex = index;
        index = rehashFunc(k, PROB, &rehashIteraction_t, size);
		oldBlockIndex = blockIndex;
		blockIndex = index % nBlocks;

		if (oldBlockIndex != blockIndex)
		{
			sem_post(activitySems.getSemaphore(oldBlockIndex));
			sem_post(blockSems.getSemaphore(oldBlockIndex));
			sem_wait(blockSems.getSemaphore(blockIndex));
			sem_wait(activitySems.getSemaphore(blockIndex));
		}

		if ((unsigned int)oldSize != size)
		{
			oldSize = size;
			continue;
		}

		if (table[index].getOcupied())
        {
            if (table[index].getKey() == k)
            {
				table[index].setData(n);
            	break;
            }
        }
		else if (table[index].getErased())
		{
			if (exists) continue;

			table[index].setKey(k);
            table[index].setData(n);
            table[index].setOcupied(true);
            
            insertions++;

            break;
		}
		else
		{
			if (exists)
			{
				exists = false;
				rehashIteraction_t = -1;
				continue;
			}

			table[index].setKey(k);
            table[index].setData(n);
            table[index].setOcupied(true);
            
            insertions++;

            break;
		}
	}

	sem_post(activitySems.getSemaphore(blockIndex));
	sem_post(blockSems.getSemaphore(blockIndex));
}

template<class Type>
void Chash<Type>:: _print(unsigned int k)
{
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
	index = k % size;

	oldIndex = index;

	blockIndex = index % nBlocks;
	oldBlockIndex = blockIndex;

	rehashIteraction_t = -1;

	sem_wait(blockSems.getSemaphore(blockIndex));
	sem_wait(activitySems.getSemaphore(blockIndex));

	while (true)
	{
		oldIndex = index;
        index = rehashFunc(k, PROB, &rehashIteraction_t, size);
		oldBlockIndex = blockIndex;
		blockIndex = index % nBlocks;

		if (oldBlockIndex != blockIndex)
		{
			sem_post(activitySems.getSemaphore(oldBlockIndex));
			sem_post(blockSems.getSemaphore(oldBlockIndex));
			sem_wait(blockSems.getSemaphore(blockIndex));
			sem_wait(activitySems.getSemaphore(blockIndex));
		}

		if ((unsigned int)oldSize != size)
		{
			oldSize = size;
			continue;
		}

		if (table[index].getOcupied())
        {
            if (table[index].getKey() == k)
            {
				cout << " Index: " << index << "\tBlock: " << blockIndex << "\tData: " << table[index].getData() << endl;
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
			printf("index: %d \t block: %d \t dado: (NULL)\n",index,blockIndex );
			break;
		}
	}

	sem_post(activitySems.getSemaphore(blockIndex));
	sem_post(blockSems.getSemaphore(blockIndex));
}

template<class Type>
void Chash<Type>:: _printall(void)
{
	for (unsigned int i = 0; i < nBlocks; i++)
	{
		sem_wait(activitySems.getSemaphore(i));
	}

	printf("\n\n================ [ Tabela ] ================\n\n");
	for(unsigned int i = 0; i < this->size; i++)
	{
		cout << "index: " << i << "   key: " << table[i].getKey() << "\tdata: " << table[i].getData()<< endl;

	}	
		
	
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

	if(!a->h->isDeallocating())
	{			
		a->h->_add(a->key,a->data);
	}

	delete a;

	return NULL;
}

template<class Type>
void* Chash<Type>::DELETE(void *y)
{

	Argument<Type> *a;

	a = static_cast< Argument<Type> * > (y);

	if(!a->h->isDeallocating())
	{			
		a->h->_delete(a->key);
	}

	delete a;

	return NULL;


	pthread_exit(NULL);
}

template<class Type>
void* Chash<Type>::GET(void *y)
{
	Argument<Type> *a;
	Type data;

	a = static_cast< Argument<Type> * > (y);


	if(!a->h->isDeallocating())
	{	
		data = a->h->_get(a->key);

		a->data = data;

	}	

	return NULL;
}

template<class Type>
void* Chash<Type>::SET(void *y)
{
	Argument<Type> *a;

	a = static_cast< Argument<Type> * > (y);


	if(!a->h->isDeallocating())
	{
		a->h->_set(a->key, a->data);

	}	
	delete a;

	return NULL;
}

template<class Type>
void* Chash<Type>::PRINT(void *y)
{
	Argument<Type> *a;

	a = static_cast< Argument<Type> * > (y);


	if(!a->h->isDeallocating())
	{
		a->h->_print(a->key);
	}

	delete a;

	return NULL;
}

template<class Type>
void* Chash<Type>::PRINTALL(void *y)
{
	Argument<Type> *a;

	a = static_cast< Argument<Type> * > (y);


	if(!a->h->isDeallocating())
	{

		a->h->_printall();
	}	

	delete a;

	return NULL;
}

//*/

#endif
