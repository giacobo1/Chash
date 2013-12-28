#ifndef LIBHASH_H
#define LIBHASH_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <pthread.h>

using namespace std;


template <class T>class Entry
{
	unsigned int key;
	T data;

public:

	Entry(void)
	{
		this->key = 0;
		this->data = (T)NULL;
	}
	Entry(const Entry& e)
	{		
		this->key = e.key;
		this->data = e.data;
	}

	Entry& operator = (const Entry& e)
	{

		this->key = e.getKey();
		this->data = e.getData();		

		return *this;
	}

	~Entry(void){}


	inline void setData(T  d){ this->data = d;}	
	inline void setKey(unsigned int k){this->key = k;}
	
	inline T getData(void)const { return data;}
	inline unsigned int getKey(void)const {return key;}
};


template<class Type>class Chash
{
	unsigned int size;
	unsigned int block;

	Entry<Type> *table;

public:

	//
	Chash(void);
	Chash(const Chash& h);
	Chash(unsigned int m,unsigned int n);
	~Chash(void);
	//

	Chash& operator = (const Chash& h)
	{
		this->block = h.block;
		this->size  = h.size;

		for(unsigned int i = 0; i < this->size; i++)table[i] = h.table[i];

		return *this;
	}


	//
	void _add(int k, Type n);
    void _delete(int k);
    void _get(int k);
    void _set(int k, Type n);
    void _print(int k);
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

template<class Type>
Chash<Type>::Chash(void)
{

}
template<class Type>
Chash<Type>::Chash(const Chash& h)
{
	this->block = h.block;
	this->size  = h.size;

	for(unsigned int i = 0; i < this->size; i++)table[i] = h.table[i];


}
template<class Type>
Chash<Type>::Chash(unsigned int m,unsigned int n)
{
	if(( m % n) == 0)
	{
		this->block		= n;
		this->size		= m*n;
	}		
	else
		printf("Erro 1\n");

	table = new Entry<Type>[size];

}
template<class Type>
Chash<Type>::~Chash(void)
{
	delete[] table;
}


// ======================================//



template<class Type>
void Chash<Type>:: _add(int k, Type n)
{

}

template<class Type>
void Chash<Type>:: _delete(int k)
{

}

template<class Type>
void Chash<Type>:: _get(int k)
{

}

template<class Type>
void Chash<Type>:: _set(int k, Type n)
{

}

template<class Type>
void Chash<Type>:: _print(int k)
{

}


template<class Type>
void Chash<Type>:: _printall(void)
{
	for(unsigned int i = 0; i < this->size; i++)
		cout << table[i].getData() << " " << table[i].getKey() << endl; 
}



// =====================================/

template<class Type>
void* Chash<Type>::ADD(void *y)
{
	pthread_exit(NULL);
}

template<class Type>
void* Chash<Type>::DELETE(void *y)
{
	pthread_exit(NULL);
}

template<class Type>
void* Chash<Type>::GET(void *y)
{
	pthread_exit(NULL);
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
	pthread_exit(NULL);
}

//

#endif