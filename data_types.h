#ifndef DATA_TYPES
#define DATA_TYPES

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define MAX_INT 2147483647

#define worldRectsWidth 10
#define worldRectsHeight 10

int tree_texture_width = 1000,
	tree_texture_height= 750;

#define tree_size 100

#define arr_size(arr)(sizeof(arr)/sizeof(arr[0]))
#define max(x,y)(x-y>=0?x:y)
#define min(x,y)(x-y<=0?x:y)
#define sq(x)((x)*(x))
#define swap(type,arr,i,j){type t = arr[i];arr[i] = arr[j]; arr[j] = t;}
#define mal(type,size) (type*)malloc(sizeof(type)*size)
#define zero(ptr,type) memset(ptr,0,sizeof(type))
#define zeros(ptr,type,size) memset(ptr,0,size*sizeof(type))

#pragma pack(push,1)

#define DQ(p) struct dqueue_ ##p{ dqueue_ ##p* back;dqueue_ ##p* front; dqueue_ ##p* prev; dqueue_ ##p* next; p val;};
DQ(int)
#undef DQ
#define DQ(p) dqueue_##p

typedef unsigned char u_char;
typedef unsigned short ushort;
typedef unsigned int uint;

bool is_empty(DQ(int)*a);

int pop_back(DQ(int)* a);
int peek_back(DQ(int)*a);
void push_back(int a,DQ(int)*b);

int pop_front(DQ(int)* a);
int peek_front(DQ(int)*a);
void push_front(int a,DQ(int)*b);

int width = 1000, height =750;

struct rgbpixel
{
	u_char r;
	u_char b;
	u_char g;
};
struct rgbapixel
{
	u_char r;
	u_char g;
	u_char b;
	u_char a;
};
struct bmp_header
{
	ushort signature;
	uint filesize;
	uint reserved;
	uint dataoffset;
	uint infoheadersize;
	uint width;
	uint height;
	ushort Planes;
	ushort bitCount;
	uint compression;
};

struct BMPpackedPixels
{
	rgbpixel a;
	rgbpixel b;
	ushort padding;
};
#pragma pack(pop)

struct Image
{
	uint width;
	uint height;
	u_char bitDepth;
	void * data;
};

struct fileData
{
	int sizeinBytes;
	void * data;
};

union B3 
{
	struct{bool a;bool b;bool c;};
	struct{bool l;bool m;bool r;};
};
union V2
{
	struct{float x; float y;};
	struct{float a; float b;};
	struct{float u; float v;};
	struct{float w; float h;};
	float  e[2];
};
union V2q //quad
{
	struct{V2 a;V2 b; V2 c ;V2 d;};
	V2 data[4];
};
union V2p // pair
{
	struct {V2 a; V2 b;};
	struct {V2 p;V2 d;};
	V2 data[2];
	bool active;
};
union V3
{
	struct{float x; float y;float z;};
	struct{float a; float b;float c;};
	struct{float w; float h;float d;};
	float e[3];
};

struct BNode
{
	
	int leftCount;
	int rightCount;
	int height;
	union 
	{
		struct
		{	
			BNode * left;
			BNode * right;
			BNode * prev;
		};
		struct
		{
			BNode * lr[2];
		};
	};
	void * data;
};

bool is_empty(DQ(int)*a)
{
	return a && a->back == a;
}

int pop_back(DQ(int)* a)
{
	//first queue element is always empty;
	assert(!is_empty(a));
	if(a->back == a->front)
		a->front = a;
	int ret = a->back->val;

	DQ(int)* prev  = a->back->prev;
	free(a->back);
	a->back = prev;
	prev->next = 0;
	return ret;
}
int peek_back(DQ(int)*a)
{
	return a->back->val;
}
void init_back_front(DQ(int) * a)
{
	a->front = a;
	a->back  = a;
}
void push_back(int a,DQ(int)*b)
{
	if(!b->back)
		b->back = b;
	DQ(int) toAdd ={0};

	toAdd.prev = b->back;
	toAdd.val = a;
	b->back->next = mal(DQ(int),1);
	b->back =b->back->next;
	if(b == b->front)
		b->front = b->next;

	*(b->back) =  toAdd;	
}

int pop_front(DQ(int)* a)
{
	assert(!(a->front == a));
	if(a->back == a->front)
		a->back = a;
	int ret = a->front->val;
	DQ(int)* next  =a->front->next;
	if(next)
	{
		next->prev = a->front->prev;
		free(a->front);
		a->front = next;
	}
	else
	{
		//first element
		free(a->front);
		a->front = a;
		a->back = a;

	}
	return ret;
}
int peek_front(DQ(int)*a)
{
	return a->front->val;
}
void push_front(int a,DQ(int)*b)
{
	DQ(int) toAdd ={0};

	toAdd.next = b->front;
	toAdd.prev = b;
	toAdd.val = a;

	b->front->prev = mal(DQ(int),1);
	if(b == b->back)
	{
		b->back = b->next;
	}
	b->front = b->front->prev;
	*(b->front) =  toAdd;
}


bool is_leaf(BNode * n)
{
	return (n && !n->left && !n->right && n->data);
}

BNode rotate_left(BNode*);
BNode freeLeftRight(BNode*);
BNode rotate_right(BNode*);
BNode freeRightLeft(BNode*);
void initNodeInt(BNode ** c,BNode *p,void * d)
{
	(*c) = mal(BNode,1);
	(*c)->leftCount = 0;
	(*c)->rightCount = 0;
	(*c)->left = 0;
	(*c)->right = 0;
	(*c)->prev =p;
	(*c)->data = d;
}

void AddValue(BNode * node,void * d,int (*compr)(void*,void*),void (*initNode)(BNode**,BNode*,void *))
{
	BNode * current = node;
	BNode * prev = node->prev;
	int height = 0;
	while(current)
	{
		if(current->data)
		{
			height++;
			if(compr(d,current->data) == 1)
			{
				prev = current;
				if(current->right)
				{
					current->rightCount++;
					current = current->right;
				}
				else
				{
					node->rightCount++;
					initNode(&current->right,prev,d);
					if(height>node->height)
					{
						node->height = height;
					}
					return;
				}
			}
			else
			{
				prev=current;
				if(current->left)
				{
					current->leftCount++;
					current = current->left;
				}
				else
				{
					node->leftCount++;
					initNode(&current->left,prev,d);
					if(height>node->height)
						node->height = height;
					
					return;
				}
			}
		}
		else //at head
		{
			current->data = d;
			return;
		}

	}
}

BNode rotate_left(BNode * n)
{
		if(n && n->right)
		{
			BNode *newN = n->right;
			n->right = newN->left;
			BNode * prev = n->prev;
			newN->prev = n->prev;
			n->prev = newN;
			newN->left = mal(BNode,1);
			*(newN->left) = *n;
			return *newN;
		}
		return *(n);
}
BNode freeLeftRight(BNode* oldRoot)
{
	if(oldRoot->data)
		free(oldRoot->data);
	if(oldRoot->left)
	{
		if(oldRoot->right)
		{	
			free(oldRoot->left->right);
			oldRoot->left->right = 0;
		}
	}
}
BNode rotate_right(BNode * n)
{
		if(n && n->left)
		{
			BNode *newN = n->left;
			newN->prev = n->prev;
			n->left = newN->right;
			n->prev = newN;
			newN->right = mal(BNode,1);
			*(newN->right) = *n;
			return *newN;
		}
		return *(n);
}
BNode freeRightLeft(BNode* oldRoot)
{
	if(oldRoot->data)
		free(oldRoot->data);
	if(oldRoot->right)
	{
		if(oldRoot->left)
		{	
			free(oldRoot->right->left);
			oldRoot->right->left = 0;
		}
	}
}


int treeCountPrint =0;
void printInt(void* d)
{	
	printf("%d, %d, %p\n",*(int*)d, treeCountPrint++,d);
}
void printChar(void * d)
{
	if(d)
	{
		printf("if %p, %c\n",d,*(char*)d);
	}

}

void inOrderPrint(BNode *node,void (*printer)(void*))
{
	if(node)
	{	
		inOrderPrint(node->left,printer);
		printer(node->data);
		inOrderPrint(node->right,printer);
	}
}


inline bool operator!(B3 e)
{
	return !(e.l||e.r||e.m);
}
inline float operator *(V2 a,V2 b)
{
	return a.x*b.x + a.y*b.y;
}
inline V2 operator *(float a,V2 b)
{
	V2 ret = {a*b.x ,a*b.y};
	return ret;
}
inline V2 operator -(V2 a,V2 b)
{
	V2 ret = {a.x-b.x ,a.y-b.y};
	return ret;
}

inline void operator -=(V2 &a,V2 b)
{
	a.x-=b.x;
	a.y-=b.y;
}

inline V2 operator +(V2 a,V2 b)
{
	V2 ret = {a.x+b.x ,a.y+b.y};
	return ret;
}

inline void operator +=(V2 &a,V2 b)
{
	a.x+=b.x;
	a.y+=b.y;
}


inline void operator*=(V2 &a,float b)
{
	a.x *= b;
	a.y *= b;
}


V2 V(float x,float y){ V2 ret ={x,y};return ret;}
V3 V(float x,float y,float z){ V3 ret ={x,y,z};return ret;}

#endif
