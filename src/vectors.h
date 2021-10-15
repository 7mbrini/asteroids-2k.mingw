#ifndef _VECTORS_H_
#define _VECTORS_H_

#include <vector>
#include <math.h>

#include <assert.h>

#include "maths.h"


struct TVector2
{
	double X, Y;
};

typedef std::vector<TVector2> TVecPoints;
typedef std::vector<TVector2*> TVecPtrPoints;

typedef std::vector<TVecPoints> TVecVecPoints;

void Build(TVector2& Pt);
void Build(TVector2& Pt, double X = 0, double Y = 0);
double Mod(TVector2& Pt);
void Normalize(TVector2& Pt);
TVector2 Add(TVector2& A, TVector2& B);

double Distance(TVector2 Pt1, TVector2 Pt2);
TVector2 Rotate(TVector2& Src, double ThetaDeg);
void Rotate(TVecPoints& VecPts, double ThetaDeg);
TVector2 Translate(TVector2& Src, TVector2 Translation);
void Translate(TVecPoints& Src, TVector2 Translation);

void Rotate(TVecVecPoints& VecPts, double ThetaDeg);
void Translate(TVecVecPoints& VecPts, TVector2 Translation);

#endif


