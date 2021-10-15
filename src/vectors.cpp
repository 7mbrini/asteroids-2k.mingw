/*!****************************************************************************

	@file	vectors.h
	@file	vectors.cpp

	@brief	Vectors routines

	@noop	author:	Francesco Settembrini
	@noop	last update: 23/6/2021
	@noop	e-mail:	mailto:francesco.settembrini@poliba.it

******************************************************************************/

#include <assert.h>
#include <math.h>

#include "vectors.h"


/*!****************************************************************************
* @brief	Builds the vector with values of Pt
* @param	Pt Referernce to a vector data structure
******************************************************************************/
void Build(TVector2& Pt)
{
	Pt.X = Pt.Y = 0;
}

/*!****************************************************************************
* @brief	Builds the vector with values of X and Y
* @param	Pt Referernce to a vector data structure
******************************************************************************/
void Build(TVector2& Pt, double X, double Y)
{
	Pt.X = X;
	Pt.Y = Y;
}

/*!****************************************************************************
* @brief	Returng the length of the vector
* @param	Pt Referernce to a vector data structure
******************************************************************************/
double Mod(TVector2& Pt)
{
	return sqrt(Pt.X * Pt.X + Pt.Y * Pt.Y);
}

/*!****************************************************************************
* @brief	Normalizes the vector
* @param	Pt Referernce to a vector data structure
******************************************************************************/
void Normalize(TVector2& Pt)
{
	double Module = Mod(Pt);

	if (Module != 0)
	{
		Pt.X /= Module;
		Pt.Y /= Module;
	}
}

/*!****************************************************************************
* @brief	Calculates the Sum of two vectors
* @param	A Referernce to a vector data structure
* @param	B Referernce to a vector data structure
* @return	The sum of vectors
******************************************************************************/
TVector2 Add(TVector2& A, TVector2& B)
{
	TVector2 Result;

	Result.X = A.X + B.X;
	Result.Y = A.Y + B.Y;

	return Result;
}

/*!****************************************************************************
* @brief	Calculates the euclidean distance between two points
* @param	Pt1 Referernce to a vector data structure
* @param	Pt2 Referernce to a vector data structure
* @return	The euclidean distance between two poins
******************************************************************************/
double Distance(TVector2 Pt1, TVector2 Pt2)
{
	return sqrt((Pt2.X - Pt1.X) * (Pt2.X - Pt1.X) + (Pt2.Y - Pt1.Y) * (Pt2.Y - Pt1.Y));
}

/*!****************************************************************************
* @brief	Rotates a point around the axis origin
* @param	Src Referernce to a vector data structure
* @param	ThetaDeg The angle of rotation, in degrees
* @return	The input vector rotated by ThetaDeg
******************************************************************************/
TVector2 Rotate(TVector2& Src, double ThetaDeg)
{
	double SinTheta = sin(ThetaDeg * M_PI / 180.0f);
	double CosTheta = cos(ThetaDeg * M_PI / 180.0f);

	double X = Src.X * CosTheta + Src.Y * SinTheta;
	double Y = -Src.X * SinTheta + Src.Y * CosTheta;

	return TVector2{X, Y};
}

/*!****************************************************************************
* @brief			Rotates a list of points around the axis origin
* @param[in,out]	VecPts Referernce to a list of points data structures
* @param			ThetaDeg The angle of rotation, in degrees
******************************************************************************/
void Rotate(TVecPoints& VecPts, double ThetaDeg)
{
	for (unsigned int i = 0; i < VecPts.size(); ++i)
	{
		VecPts[i] = Rotate(VecPts[i], ThetaDeg);
	}
}

/*!****************************************************************************
* @brief			Rotates a list of a list of points around the axis origin
* @param[in,out]	Src Referernce to a list of list of points data structures
* @param			ThetaDeg The angle of rotation, in degrees
******************************************************************************/
void Rotate(TVecVecPoints& VecPts, double ThetaDeg)
{
	for(int i=0; i<VecPts.size(); ++i)
	{
		Rotate(VecPts[i], ThetaDeg);
	}
}

/*!****************************************************************************
* @brief	Translates a point 
* @param	Src Referernce to a vector data structure
* @param	Translation The value for translation
* @return	The input vector translated
******************************************************************************/
TVector2 Translate(TVector2& Src, TVector2 Translation)
{
	return TVector2{Src.X + Translation.X, Src.Y + Translation.Y};
}

/*!****************************************************************************
* @brief	Translates a list of points
* @param	Src Referernce to a list of points data structures
* @param	Translation The value for translation
******************************************************************************/
void Translate(TVecPoints& Src, TVector2 Translation)
{
	for(int i = 0; i < Src.size(); ++i)
	{
		Src[i] = Translate(Src[i], Translation);
	}
}

/*!****************************************************************************
* @brief	Translates a list of a list of points
* @param	Src Referernce to a list of list of points data structures
* @param	Translation The value for translation
******************************************************************************/
void Translate(TVecVecPoints& VecPts, TVector2 Translation)
{
	for(int i=0; i<VecPts.size(); ++i)
	{
		Translate(VecPts[i], Translation);
	}
}

