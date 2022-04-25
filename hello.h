#ifndef HELLO_H
#define HELLO_H

#include "vector_functions.h"
#include <SDL.h>
#include <time.h>

typedef SDL_Rect Rect;
typedef SDL_Color Color;

#undef main

struct LLRect
{
	LLRect * next;
	LLRect * prev;
	Rect e;
};

struct worldRect
{
	Rect pos;
	LLRect head;
};

union GUIelement
{
	struct
	{
		Rect pos_size;
	
		bool hot;
		bool active;
	};
};

V2q getRectVecs(SDL_Rect r);
V2 intersectionPoint(V2,V2,V2,V2);
V2p getIntersectionPoints(Rect, V2, V2, V2,V2);

V2 getNearestNormal(Rect,int,int);
V2 getClosestToPointInRect(V2, V2, V2, V2,Rect);
V2 getMidPointForRect(Rect);
V2q getRectVecs(Rect);

bool RemoveLastListElement(LLRect * head);
void RemoveFromList(Rect * d ,LLRect * head);

bool isPointOnVector(V2,V2,V2);

bool timeToUpdate(int&,int,bool);
bool drawListRectsAndChangePositionForDraggedMousePosition(SDL_Renderer* ,
														   LLRect *,
														   B3,
														   V2,
														   Rect**,
														   SDL_Texture*,V2);

unsigned int reverseBits(unsigned int);
void addRectToList(Rect rectToAdd,LLRect* toAddTo);
void writeToFile(const char* name,char * data,int size);
Image readBMP(const char * path);
Image produceRGBAPackedOutBMP(fileData);
int getBitBoundryDelta(int,int);
fileData readFile(const char * path);

void simple_btree_renderer(BNode *, SDL_Renderer*,V2,V2p);
void render_btree(SDL_Renderer*,SDL_Texture*,BNode*,V2,V2p,void (*BtreeRenderer)(BNode*,SDL_Renderer*,V2,V2p));

int intcompr(void * a,void* b)
{
	int aInt = *((int *)a);
	int bInt = *((int *)b);

	return aInt>bInt?1:aInt<bInt?-1:0;
}

inline bool operator>(V2 b, Rect a)
{
	return b.x>a.x &&
		   b.y>a.y &&
		   b.x<a.x+a.w &&
		   b.y<a.y+a.h;	
}
inline bool operator == (Rect a,Rect b)
{
	return a.x==b.x && a.y==b.y &&a.w == b.w&&a.h == b.h;
}

inline Rect operator +(Rect a,V2 b)
{
	Rect ret = {a.x+b.x ,a.y+b.y,a.w,a.h};
	return ret;
}
inline void operator+=(Rect &a,V2 b)
{
	a.x += b.x;
	a.y += b.y;
}

Rect R(float x,float y, float w, float h){Rect ret ={x,y,w,h};return ret;}

bool inRect(Rect,int,int);
bool inRect(Rect,V2);
bool inRect(Rect,Rect);

bool inRect(Rect a, int X, int Y)
{
	V2 test = {X,Y};
	return test>a;
}
bool inRect(Rect a, V2 b)
{
	return b.x>a.x &&
		   b.y>a.y &&
		   b.x<a.x+a.w &&
		   b.y<a.y+a.h;	
}
bool inRect(Rect a, Rect b)
{
	return !((a.x>b.x&&a.x>b.x+b.w)||(a.y>b.y&&a.y>b.y+b.h)||
			(a.x<b.x&&a.x+a.w<b.x)||(a.y<b.y&&a.y+a.h<b.y));
}

float getDistanceFromPoint(SDL_Rect a, V2 b)
{
	V2 c = {a.x-b.x,a.y-b.y};
	V2 d = {a.x+a.w-b.x,a.y-b.y};
	V2 e = {a.x-b.x,a.y+a.h-b.y};
	V2 f = {a.x+a.w-b.x,a.y+a.h-b.y};

	float distC = len(c);
	float distD = len(d);
	float distE = len(e);
	float distF = len(f);
	// return (distC+distD+distE+distF)/4;
	return min(min(distC,distD),min(distE,distF));
}


bool AddToList(LLRect *addition ,LLRect * head)
{
	if(!head)
		return false;
	LLRect * node = head;
	while(node->next)
	{
		node = node->next;
	}
	addition->prev = node;
	addition->next = 0;
	node->next = addition;	

	return true;
}

unsigned int reverseBits(unsigned int input)
{
	unsigned int reverse = input;
	unsigned int count = sizeof(input)*8 - 1;

	input>>=1;
	while(input)
	{
		reverse<<=1;
		reverse |= input & 1;
		input >>= 1;
		count--;
	}
	return reverse << count;
}

int getBitBoundryDelta(int num,int bitCount)
{
	if (num % bitCount)
		return bitCount - num%bitCount;
	else 
		return 0;
}

void writeToFile(const char* name,char * data,int size)
{
	FILE * fileHandle = fopen(name,"wb");
	fwrite(data,sizeof(char),size,fileHandle);
	fclose(fileHandle);
}	

fileData readFile(const char * path)
{
	FILE * file = fopen(path,"rb");	

	fileData ret ={};	

        if(!file)
        {
            return ret;
        }

	fseek(file,0,SEEK_END);
	ret.sizeinBytes = ftell(file);
	ret.data = malloc(ret.sizeinBytes);
	rewind(file);
	fread(ret.data,ret.sizeinBytes,1,file);
	return ret;
}
Image produceRGBAPackedOutBMP(fileData d)
{
	Image ret={0};
        if(d.sizeinBytes == 0)
            return ret;
	bmp_header h = *((bmp_header*)d.data);
	printf("%c%c\n",h.signature,*((u_char*)&h.signature+1));
	printf("%u\n",h.filesize);
	printf("%u\n",h.dataoffset);
	printf("%u\n",h.infoheadersize);
	printf("%u,%u\n",h.width,h.height);
	printf("%u\n",h.bitCount);
	printf("%u\n",h.compression);
	printf("%u\n",h.filesize-h.infoheadersize);

	int counter = 0;
	int rowCount =0;
	
	ret.width = h.width;
	ret.height = h.height;
	ret.bitDepth  = h.bitCount;
	ret.data= malloc(sizeof(rgbapixel)*h.width*h.height);
	rowCount = h.height-1;
	float maxDistance = sqrt(sq((float)h.width/2.f)+sq((float)rowCount/2.f));
	printf("maxDistance: %f\n",maxDistance);
	while(rowCount>=0)
	{
			rgbpixel rowPixels[h.width];
			memcpy(&rowPixels,((u_char*)d.data+h.dataoffset+counter),sizeof(rgbpixel)*h.width);
			counter += (h.width)*sizeof(rgbpixel) + getBitBoundryDelta(h.width,sizeof(rgbapixel));

			for (int i = 0; i < h.width; i++)
			{
				rgbapixel *px = (rgbapixel*)ret.data + (rowCount*h.width + i);
				float x = i;
				float y = rowCount-1;
				float distance = sqrt(sq(x-(float)h.width/2.f)+sq(y-(float)h.height/2.f));
				if(distance/(maxDistance/1.5f)>1.f){
					px->r = 0;
				}
				else
					px->r = (1.f-distance/(maxDistance/1.5f))*(float)255;
				px->b = rowPixels[i].b; // argb;
				px->g = rowPixels[i].g;
				px->a = rowPixels[i].r;
			}
			rowCount--;
	}
	
	return ret;
}

void addRectToList(Rect rectToAdd,LLRect* toAddTo)
{
	LLRect *addition = mal(LLRect,1);
    addition->e = rectToAdd;
	addition->next =0;
   AddToList(toAddTo,addition);
}

#define reduceX 1.f/2.f

#define reduceY 1.f/1.f

void simple_btree_renderer(BNode * n ,SDL_Renderer * r,V2 CurrentPos,V2p LeftRightPair)
{
	if(n)
	{
		SDL_SetRenderDrawColor(r,255,255,255,255);
		Rect toRedner = {CurrentPos.x-2,CurrentPos.y-2,5,5};
		SDL_RenderDrawRect(r,&toRedner);
		if(n->left)
		{
			SDL_SetRenderDrawColor(r,0,255,0,255);
			SDL_RenderDrawLine(r,
							   CurrentPos.x,
							   CurrentPos.y,
							   CurrentPos.x+LeftRightPair.a.x,
							   CurrentPos.y+LeftRightPair.a.y);
			V2p leftRightTempPair = {LeftRightPair.a,LeftRightPair.b};
			leftRightTempPair.a.x*=reduceX;
			leftRightTempPair.b.x*=reduceX;
			leftRightTempPair.a.y*=reduceY;
			leftRightTempPair.b.y*=reduceY;
			simple_btree_renderer(n->left,r,CurrentPos+LeftRightPair.a,leftRightTempPair);
		}
		if(n->right)
		{
			SDL_SetRenderDrawColor(r,255,0,0,255);
			
			SDL_RenderDrawLine(r,
							   CurrentPos.x,
							   CurrentPos.y,
							   CurrentPos.x+LeftRightPair.b.x,
							   CurrentPos.y+LeftRightPair.b.y);
			V2p leftRightTempPair = {LeftRightPair.a,LeftRightPair.b};
			leftRightTempPair.a.x*=reduceX;
			leftRightTempPair.b.x*=reduceX;
			leftRightTempPair.a.y*=reduceY;
			leftRightTempPair.b.y*=reduceY;
			
			simple_btree_renderer(n->right,r,CurrentPos+LeftRightPair.b,leftRightTempPair);
		}
	}

}

void render_btree(SDL_Renderer *r,
				  SDL_Texture *t,
				  BNode* b,
				  V2 startPos,
				  V2p LeftRightPair,
				  void (*BtreeRenderer)(BNode*,SDL_Renderer*,V2,V2p))
{
	SDL_SetRenderTarget(r,t);
	BtreeRenderer(b,r,startPos,LeftRightPair);
	SDL_SetRenderTarget(r,0);
}

bool drawListRectsAndChangePositionForDraggedMousePosition(
					SDL_Renderer* r,
				    LLRect *head,
					B3 mouseBState,
					V2 mousePos,
					SDL_Rect** grabbedRect,
					SDL_Texture *texture,
					V2 offset=V(0,0))
{
	LLRect* node  =head;
	SDL_Rect* selected = 0;
	float closest = MAX_INT;
	int roll=20;;
	while(node)
	{
		Rect toDrawRect = node->e;
		toDrawRect.x+=offset.x;
		toDrawRect.y+=offset.y;
		if(!(!mouseBState))
		{
			if(inRect(toDrawRect,mousePos) && !(*grabbedRect))
			{
				float dist = getDistanceFromPoint(toDrawRect,mousePos);
				printf("%f>%f\n",closest,dist);
				if(closest>dist)
				{   
					closest = dist;
					selected = &node->e;
				}
			}
		}
		int mBlue  =  0x00000000;
		int mGreen =  0x00000000;
		int mRed   =  0x00000000;
		
		if(node->e.x!=0 || node->e.y!=0)
		{

		int counter = 0;
		int x= 0,y=0;
		int w =2,h=2;
		
		//SDL_RenderDrawRect(r,&node->e);
		
		//SDL_RenderCopyEx(r, texture, 0,&(toDrawRect),roll+=20,0,SDL_FLIP_NONE);
		SDL_RenderCopyEx(r, texture, 0,&(toDrawRect),0,0,SDL_FLIP_NONE);
		SDL_RenderDrawRect(r,&toDrawRect);
	}
		node=node->next;
	}
	if(!mouseBState)
	{
		(*grabbedRect) =0;
	}
	else 
	{
		if(selected)
		{
			selected->x-=selected->w/2;
			selected->y-=selected->h/2;
			(*grabbedRect) = selected;	
		}
	}
}

V2 getClosestToPointInRect(V2 p , V2 a, V2 b, V2 c , SDL_Rect r)
{
	float l[3]={len(p-a),len(p-b),len(p-c)};
	V2 v[3]={a,b,c};
	float minValue = min(l[0],min(l[1],l[2]));
	int minIndex = -1;
	for (int i = 0; i < 3; ++i)
	{
		if (minValue == l[i] && inRect(r,v[i]))
			return v[i];
	}
	for (int i = 0; i < 3; ++i)
	{
		if (minValue == l[i])
			minIndex=i;
		if (inRect(r,v[i]))
			return v[i];
	}
	return v[minIndex];
}

V2 getNearestNormal(SDL_Rect r,int x,int y)
{
	V2 retVal;
	V2q Vecs = getRectVecs(r);
	V2 vec;
	
	float minLen = MAX_INT;
	float lenV = 0;
	for (int i = 0; i < 4; ++i)
	{
		vec.x=x;
		vec.y=y;
		float lengthAcrossVecs =vec*Vecs.data[i];
		float lenA = normalize(&Vecs.data[i]);
		float lenVec = normalize(&vec);
		vec.x-=(Vecs.data[i].x*lengthAcrossVecs);
		vec.y-=(Vecs.data[i].y*lengthAcrossVecs);
		lenV = len(vec);
		if(lenV<minLen)
		{
			minLen =lenV;
			retVal.x=(vec.y/lenV);
			retVal.y=(vec.x/lenV);
		}
	}
	printf("%d,%d,%f\n",retVal.x,retVal.y,lenV);
	return retVal;
}
V2q getRectVecs(SDL_Rect r)
{
	V2q ret;
	ret.a.x = r.w;
	ret.a.y = 0;
	ret.b.x = 0;
	ret.b.y = r.h;
	ret.c.x = -r.w;
	ret.c.y = 0;
	ret.d.x = 0;
	ret.d.y = -r.h;
	return ret;

}
 

V2p getIntersectionPoints(SDL_Rect r, V2 a, V2 b, V2 c,V2 dir)
{
	V2q rects =getRectVecs(r);
	V2 bacacb[3] = {b-a,c-a,c-b};
	V2 start = {r.x,r.y};
	V2 aab[3] = {a,a,b};
	V2 abc[3] = {a,b,c};
	V2p S = {0};
	S.active =0;
	V2 p[12] ={0};
	int pairCount = 0;

	// get the two points of intersection such that the length between them is the largest of all points
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			int index = i*4 + j;
			p[index] =intersectionPoint(aab[i],bacacb[i],start,rects.data[j]);
			if (p[index].x!=0||p[index].y!=0)
			{
				S.active = true;
				S.data[pairCount++]= p[index];
				if(pairCount == 2)
					return S;
			}
			start += rects.data[j];
		}
	}
	return S;
}	
V2 getMidPointForRect(SDL_Rect r)
{
	return V(r.x+r.w/2, r.y+r.h/2);
}
V2 intersectionPoint(V2 a,V2 b,V2 c,V2 d )
{
	//a +tb = c+sd;
	//aXb =cXb +s(dXb);
	// t(bXd) = cXd-aXd
	V2 ret;
	float s = (X(a,b)-X(c,b));
	float dxb = X(d,b);
	float t = (X(c,d)-X(a,d));
	float bxd = X(b,d);
	if(dxb == 0 || bxd ==0)
	{
		return a;
	}
	s/=dxb;
	if(s>1||s<0)
	{
		ret.x = 0;
		ret.y = 0;
	}
	else  
	{
		t/= bxd;
		if (t<=1 && t>=0)
		{
			ret.x = c.x+s*d.x;
			ret.y = c.y+s*d.y;
		}
		else 
		{
			ret.x = 0;
			ret.y = 0;
		}
	}
	return ret;
}
bool isPointOnVector(V2 a,V2 b,V2 c)
{
	float t = -(X(b,c))/X(c,a);
	return !(t<0|| t>1);
}
bool timeToUpdate(int &lastTime,int interval,bool update = false)
{
	int currentTime = SDL_GetTicks();
	if((currentTime-lastTime)>interval)
	{
		if(update)
			lastTime = currentTime;
		return true;
	}
	return false;
}
void translateTriangle(float & x,float &y,
					   float &x0,float & y0,
					   float & x1,float& y1,
					   float dirX,float dirY)
{
	x+=dirX;x0+=dirX;x1+=dirX;
    y+=dirY;y0+=dirY;y1+=dirY;
}
bool updateState(bool key,int &lastUpdated,int sleepTimeAfterStateChange,bool &stateName)
{
	if(key && timeToUpdate(lastUpdated,sleepTimeAfterStateChange,true))
   		stateName = !stateName;
   	return stateName;
}

bool RemoveLastListElement(LLRect * head)
{
	if(!head)
	{
		return false;
	}
	LLRect *node = head;
	while(node->next)
	{
		node = node->next;
	}
	if(node->prev)
	{
			node->prev->next = 0;
			free(node);
			return true;
	}

}
void RemoveFromList(Rect *d ,LLRect * head)
{
	if(!head)
		return;
	LLRect * node = head;
	LLRect * currentToRemove=0;
	while(node)
	{
		if(*d == node->e)
		{			
			if(node->prev)
			{
				if(node->next)
				{
					node->next->prev = node->prev;
					node->prev->next = node->next;
				}
				else
					node->prev->next = 0;
			}
			currentToRemove = node;
			break;
		}		
			node = node->next;
	}
	 if(currentToRemove!=head)
		free(currentToRemove);
}
void rotate_ship(float& x,float& y,float& x0,float& y0,float& x1,float& y1,float midPointX,float midPointY,float roll)
{	  
	  float tx =0,ty=0;

	  float cosroll = cosf(roll);
	  float sinroll = sinf(roll);

	  x-=midPointX;
	  y-=midPointY;
	  
	  tx=x*cosroll-y*sinroll;
	  ty=x*sinroll+y*cosroll;
	  x = tx+midPointX;
	  y = ty+midPointY;

	  x1-=midPointX;
	  y1-=midPointY;
	  tx=x1*cosroll-y1*sinroll;
	  ty=x1*sinroll+y1*cosroll;
	  x1 = tx+midPointX;
	  y1 = ty+midPointY;
	  
	  x0-=midPointX;
	  y0-=midPointY;
	  tx=x0*cosroll-y0*sinroll;
	  ty=x0*sinroll+y0*cosroll;
	  x0 = tx+midPointX;
	  y0 = ty+midPointY;
}

void render_fire(SDL_Renderer *r, V2p &shipState,float dirX, float dirY, V2 toAddRender,bool dir,int fireSize,float baseAngle,int detailed )
{
	for (int i = -detailed/2; i < detailed/2; ++i)
	{
		float rr = (rand()%((i!=0?i:1)))%fireSize;
		float ang = (6.078)/(i!=0?i:1)+to_radians(baseAngle);

		V2 dirRotated = {rr*-dirX,rr*-dirY};

		if(dir)
			dirRotated = rotate(dirRotated,-ang);
		else 
			dirRotated = rotate(dirRotated,ang);
		V2 particle = {(shipState.a.x+dirRotated.x+toAddRender.x),
					   (shipState.a.y+dirRotated.y+toAddRender.y)};
		//particle = perp(particle);
		// int yello = (r b g)(1-t) + t(r g b)
		int c = rand()%4;

	    if(c==1)
	   		SDL_SetRenderDrawColor(r,255,255,0,SDL_ALPHA_OPAQUE);
	   	else if(c==2)
	   		SDL_SetRenderDrawColor(r,255,128,0,SDL_ALPHA_OPAQUE);
	   	else if(c==0)
	   		SDL_SetRenderDrawColor(r,255,128,128,SDL_ALPHA_OPAQUE);
	   	else
			SDL_SetRenderDrawColor(r,128,128,255,SDL_ALPHA_OPAQUE);
			SDL_RenderDrawLine(r,shipState.a.x+toAddRender.x,
							 shipState.a.y+toAddRender.y,
							 particle.x,
							 particle.y);
	}


}


#endif
