#ifndef ENTRY_H
#define ENTRY_H

template <class T>class Entry
{
	unsigned int key;
	T data;
	bool ocupied;

public:

	Entry(void)
	{
		this->key = 0;
		this->data = (T)NULL;
		this->ocupied = false;
	}

	Entry(unsigned int k, T d, bool o)
	{
		this->key = k;
		this->data = d;
		this->ocupied = o;
	}

	Entry(const Entry& e)
	{		
		this->key = e.key;
		this->data = e.data;
		this->ocupied = e.ocupied;
	}

	Entry& operator = (const Entry& e)
	{

		this->key = e.getKey();
		this->data = e.getData();
		this->ocupied = e.getOcupied();

		return *this;
	}

	~Entry(void){}


	void setData(T  d){ this->data = d;}	
	void setKey(unsigned int k){this->key = k;}
	void setOcupied(bool o){this->ocupied = o;}
	
	T getData(void) { return data;}
	unsigned int getKey(void) {return key;}
	bool getOcupied(void){return ocupied;}
};



#endif