#ifndef VECTOR_FUNCTIONS
#define VECTOR_FUNCTIONS

#include "data_types.h"


#define X(a,b) ((a.x*b.y)-(b.x*a.y))

inline float dot(V2 a,V2 b)
{
	return a.x*b.x + b.y*a.y;
}

V2 perp(V2 a)
{
	return V(-a.y,a.x);
}
float len(V2 a)
{
	return sqrt(a.x*a.x + a.y*a.y);
}
float len(V3 a)
{
	return sqrt(a.x*a.x + a.y*a.y +a.z*a.z);
}
float normalize(V2 *a)
{
	float l = len(*a);
	(*a) *=1.f/l;
	return l;
}
float to_radians(float degrees)
{
	return (3.141592f/180.f)*degrees;
}
V2 rotate(V2 a, float angle)
{
	//1 0 -> cos(@) sin(@)
	//0 1 -> -sin(@) cos(@)
	V2 ret = {cos(angle)*a.x+sin(angle)*a.y,-sin(angle)*a.x + cos(angle)*a.y};
	return ret;
}

float getDistanceFromLine(V2 a, V2 b, V2 p)
{
	float base = len(a-b);

	V2 ap = (a-p);
	V2 ab = (a-b);

	float area  = X(ap,ab);

	float height = area/base;
	return height;
	//1/2*height*base = area/2;
}
#endif
