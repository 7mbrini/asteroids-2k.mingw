#ifndef _MATHS_H_
#define _MATHS_H_

#include <vector>
#include <math.h>

#include <assert.h>


#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

#define DEG2RAD(Val)  ((Val) * M_PI / 180.0 )
#define RAD2DEG(Val)  ((Val) * 180.0 / M_PI )

typedef std::vector<int> TVecIntegers;


int RandSign();
int Sign(double Val);
double Rand(double Val);
double AbsRand(double Val);

void Sort(TVecIntegers& VecInts);


#endif

