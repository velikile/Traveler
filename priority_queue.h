#ifndef PRIORITY_QUEUE
#define PRIORITY_QUEUE
#include "data_types.h"

struct priority_data
{
	int p;
	void *data;
};
struct p_q
{
	priority_data * queue;
	int max_size;
	int current_size;
};
bool fixmax(p_q a,int i);


p_q pq(int size)
{
	p_q ret;
	ret.queue = mal(priority_data,size);
	ret.max_size = size;
	return ret;
}
void freePq(p_q &a)
{
	a.current_size = 0;
	a.max_size =0;
	free(a.queue);
}
int left(int i)
{
	return 2*i+1;
}
int right(int i)
{
	return 2*i+2;
}
int parent(int i)
{
	return (i-1)/2;
}
priority_data left(p_q a, int i)
{
	if(a.current_size>2*i+1)
		return a.queue[2*i+1];
}
priority_data right(p_q a, int i)
{
	if(a.current_size>right(i))
		return a.queue[right(i)];
	else
	{
		priority_data q = {0,0};
		return q;
	}
}
bool is_leaf(p_q a,int i)
{
	return (2*i +1 > a.current_size);
}

bool insert(p_q &a,priority_data *b,int size)
{
	if(a.max_size >= size)
	{
		for (int i = 0; i < size; ++i)
		{
			a.queue[i]=b[i];
			a.current_size++;
		}
		return true;
	}
	return false;
}
priority_data pop_max(p_q &a)
{
	if(a.current_size>0)
	{
		swap(priority_data,a.queue,--a.current_size,0);

		fixmax(a,0);
		return a.queue[a.current_size+1];
	}

	priority_data q = {0,0};
	return q;
}

bool push(p_q &a,priority_data b)
{
	if(a.max_size > a.current_size)
	{
		a.queue[++a.current_size]=b;

		int elementP = a.current_size;
		while(elementP)
		{
			elementP = parent(a.current_size);
			fixmax(a,elementP);

		}
		return true;
	}
	return false;
}

bool fixmax(p_q a,int i)
{
	if(!is_leaf(a,i))
	{
		int r = right(a,i).p;
		int l = left(a,i).p;
		if(a.queue[i].p>=r&&a.queue[i].p>=l)
			return true;
		else 
		{
			int maxP = max(max(r,l),a.queue[i].p);
			if(maxP == a.queue[i].p)
			{
				return true;
			}
			else if(maxP == r && right(i) < a.current_size)
			{
				swap(priority_data,a.queue,i,right(i));
				return fixmax(a,right(i));
			}
			else 
			{
				swap(priority_data,a.queue,i,left(i));
				fixmax(a,left(i));
			}
			
			//move current node to the largest leaf and do the fix max on it 
		}
	}
	return true;
}

#endif