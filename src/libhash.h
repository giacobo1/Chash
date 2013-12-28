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

	static void ADD(void);


};








#endif