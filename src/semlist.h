#ifndef SEMLIST_H
#define SEMLIST_H

#include <semaphore.h>

class SemListElement
{
	SemListElement *next;
	sem_t sem;

public:

	SemListElement()
	{
		sem_init(&sem, 0, 1);
		next = NULL;
	}

	~SemListElement()
	{
		
	}

	
	void destroy()
	{
		sem_destroy(&sem);
	}
	

	SemListElement *createNext()
	{
		next = new SemListElement();
		return next;
	}

	SemListElement *getNext()
	{
		return next;
	}

	sem_t *getSem()
	{
		return &sem;
	}
};

class SemList
{
	SemListElement *start;
	SemListElement *end;

	int elements;

public:

	SemList()
	{
		elements = 0;
		start = NULL;
		end = NULL;
	}

	~SemList()
	{
		SemListElement *start_delete = start;

		while(start)
		{			
			start = start->getNext();			
			start_delete->destroy();
			start_delete = start;
		}
	}

	void addSemaphore()
	{
		if (start == NULL)
		{
			start = new SemListElement();
			end = start;
		}
		else
		{
			end = end->createNext();
		}

		elements += 1;
	}

	sem_t *getSemaphore(int index)
	{
		sem_t *sem = NULL;
		SemListElement *elem = NULL;

		if (elements - 1 < index) sem = NULL;
		else
		{
			elem = start;

			for (int i = 0; i < elements; i++)
			{
				if (i == index)
				{
					sem = elem->getSem();
					break;
				}
				else
				{
					elem = elem->getNext();
				}
			}
		}

		return sem;
	}

	int getSize()
	{
		return elements;
	}
};

#endif