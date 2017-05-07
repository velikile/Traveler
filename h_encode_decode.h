//huffman encode decode 
//utf-8
#ifndef H_ENCODE_DECODE
#define H_ENCODE_DECODE

// #define PRINT
#include "data_types.h"

#define CHAR_COUNT 256 
#define MAX_STREAM_SIZE 1024

struct h_table
{
	u_char data;
	u_char bit_count;
};

float * create_histogram(char * stream);
u_char* encode(char * stream);
BNode create_tree(float* histogram);
void print_bits(u_char);
void create_two_nodes_tree(BNode ** t,float a,int ai,float b,int bi,float *histogram);

float * create_histogram(char * stream)
{
	float * ret = mal(float,CHAR_COUNT);
	zeros(ret,float,CHAR_COUNT);
	
	int total = 0;
	for (int i = 0; stream[i]; ++i)
	{
		ret[stream[i]]+=1.f;
		total++;
	}
	printf("total %d\n",total);
	for (int i = 0; i < CHAR_COUNT; ++i)
	{
		ret[i] /= total;
	}
	return ret;
}

void huff_print_codes(BNode * node ,u_char encoding ,u_char dir,int depth ,h_table* n = 0)
{
	encoding = (encoding | 1<<(depth-1))*dir | (encoding & ~(1<<(depth-1)))*(!dir);
	
	if(is_leaf(node))
	{		
		if(!n)
		{
			print_bits(encoding);
			printf(" %c %d\n",*(char*)(node->data),depth);
		}
		else
		{
			n[*(u_char*)node->data].data = encoding;
			n[*(u_char*)node->data].bit_count = depth;
		}
		return;
	}
	huff_print_codes(node->left,encoding,0,depth+1,n);
	huff_print_codes(node->right,encoding,1,depth+1,n);
}

h_table * get_encoding_table(BNode tree)
{
	h_table * ret = mal(h_table,CHAR_COUNT);
	zeros(ret,h_table,CHAR_COUNT);
	huff_print_codes(&tree,0,0,0,ret);
	return ret;
}
void print_bits(u_char a)
{
	for (u_char i = 0; i < 8; i++)
	{
		printf("%u ",(a&(u_char)(1<<(7-i)))!=0);
	}
}
u_char* encode(char * stream)
{
	float *histogram = create_histogram(stream);
	BNode tree = create_tree(histogram);
	h_table * t  = get_encoding_table(tree);
	u_char * packed = mal(u_char,MAX_STREAM_SIZE);
	zeros(packed,char,MAX_STREAM_SIZE);
	int on_current = 0;
	int packed_index = 0;

	for (int i = 0; stream[i]; ++i)
	{
		u_char e = t[stream[i]].data;
		printf("%c\n", stream[i]);
		u_char d = t[stream[i]].bit_count;
		if(on_current+d<=8)
		{
			#ifdef PRINT
			print_bits(packed[packed_index]);printf("      ");
			#endif
			packed[packed_index] |=  e<<(on_current);
			#ifdef PRINT
			print_bits(packed[packed_index]);printf("\n");
			#endif
			on_current += d;
			if(on_current==8)
			{
				on_current=0;
				packed_index++;
			}
		}
		else 
		{	
			packed[packed_index] |= e<<(on_current);
			packed[++packed_index]= e>>(8-on_current);
			#ifdef PRINT
			print_bits(packed[packed_index-1]);printf("        ");print_bits(packed[packed_index]); printf("\n");
			#endif
			on_current = d-(8-on_current);
		}
	}
	free(t);
	free(histogram);
	return packed;
}

BNode create_tree(float * histogram)
{
	#ifdef PRINT
		printf("1\n");
	#endif
	BNode ** tree = mal(BNode*,CHAR_COUNT);
	
	for (int i = 0; i < CHAR_COUNT; ++i)
	{ 
		if(histogram[i]>0)
		{
			tree[i] = mal(BNode,1);

			zero(tree[i],BNode);

			tree[i]->data = mal(char,1);
			(*(char*)tree[i]->data) = (char)i;
		}
		else 
			tree[i] = 0;
	}
	do
	{
		int mini[] = {-1,-1};
		float min[] = {1.f,1.f};
		for (int i = 0; i < CHAR_COUNT; ++i)
		{
			if(histogram[i]<=max(min[0],min[1]) && histogram[i]>0)
			{
				float maxmin= max(min[0],min[1]);
				if(maxmin==min[0])
				{
					min[0] = histogram[i];
					mini[0] = i;
				}
				else
				{
					min[1] = histogram[i];
					mini[1] = i;
				}
			}
		}
		#ifdef PRINT
			printf("2 %c %c %f %f \n",mini[0],mini[1],min[0],min[1] );
		#endif
		if(mini[0] == -1 && mini[1]>=1 || mini[1] == -1  && mini[0]>=1)
		{
			BNode ret;
			if(mini[0]!=-1)
				ret =*tree[mini[0]];
			else if(mini[1]!=-1)
				ret= *tree[mini[1]];
			free(tree);
			return ret;
		}
		if(min[0]>min[1])
		{
			create_two_nodes_tree(tree,min[1],mini[1],min[0],mini[0],histogram);
		}
		else 
		{
			create_two_nodes_tree(tree,min[0],mini[0],min[1],mini[1],histogram);
		}
		#ifdef PRINT
		printf("3\n");
		#endif

	}while(true);

	//return **tree;
	//create a node from two of the miniumum values and add them to the array 
}

void create_two_nodes_tree(BNode ** t,float a,int ai,float b,int bi,float *histogram)
{
	BNode mergedNode={0};
	mergedNode.left = mal(BNode,1);
	*(mergedNode.left) = *(t[ai]);
	mergedNode.right = mal(BNode,1);
	*(mergedNode.right) = *(t[bi]);

	free(&t[ai]);
	free(&t[bi]);
	t[ai] = mal(BNode,1);
	t[bi] = 0;
	*t[ai] = mergedNode;
	histogram[ai] = a+b;
	histogram[bi] = 0;
}

u_char *decode ( u_char * stream ,int size,BNode tree)
{
	u_char * ret = mal(u_char,MAX_STREAM_SIZE);
	int count =0;
	BNode *run = &tree;
	u_char *cur = stream;
	int d = 0;
	for (int i = 0; i < size; ++i)
	{
		while(run)
		{
			if(d==8)
			{
				d = 0;
				cur++;
			}
			u_char current = (*cur)>>(d++);
			if(current & 1)
				run = run->right;
			else 
				run = run->left;
			if(is_leaf(run))
			{
				print_bits(*cur);
				printf("%c\n", *((char*)run->data));

				ret[i] = *((char*)run->data);
				run = &tree;
				break;
			}
			
			
		}
	}
	return ret;	
}

#endif