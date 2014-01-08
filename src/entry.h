#ifndef ENTRY_H
#define ENTRY_H

template <class T>class Entry
{
	unsigned int key;
	T data;
	bool ocupied;
	bool erased;

public:

	Entry(void)
	{
		this->key = 0;
		this->data = (T)NULL;
		this->ocupied = false;
		this->erased = false;
	}

	Entry(unsigned int k, T d, bool o, bool e = false)
	{
		this->key = k;
		this->data = d;
		this->ocupied = o;
		this->erased = e;
	}

	Entry(const Entry& e)
	{		
		this->key = e.key;
		this->data = e.data;
		this->ocupied = e.ocupied;
		this->erased = e.erased;
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
	void setErased(bool e){this->erased = e;}
	
	T getData(void) { return data;}
	unsigned int getKey(void) {return key;}
	bool getOcupied(void){return ocupied;}
	bool getErased(void){return erased;}
};



#endif