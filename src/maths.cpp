/*!****************************************************************************

	@file	maths.h
	@file	maths.cpp

	@brief	Maths routines

	@noop	author:	Francesco Settembrini
	@noop	last update: 23/6/2021
	@noop	e-mail:	mailto:francesco.settembrini@poliba.it

******************************************************************************/

#include <assert.h>
#include <algorithm>

#include <math.h>

#include "commdefs.h"
#include "maths.h"


/*!****************************************************************************
* @brief	Generates a random value in a specified range
* @param	Val The interval absolute limits in which a value can be generated
******************************************************************************/
double Rand(double Val)
{
	return double( Val -  2.0 * rand()/double(RAND_MAX) * Val );
}

/*!****************************************************************************
* @brief	Generates a random value in a specified range
* @param	Val The interval limits in which a value can be generated
******************************************************************************/
double AbsRand(double Val)
{
	return double( rand() / double(RAND_MAX) * Val );
}

/*!****************************************************************************
* @brief	Randomly generates a unitary value with sign
* @return	Returns an +1 or -1 value
******************************************************************************/
int RandSign()
{
	double RandVal = -1.0 + 2.0*rand()/double(RAND_MAX);
	
	return Sign(RandVal);
}

/*!****************************************************************************
* @brief	Gets the sign of a specified value
* @return	Returns an +1 or -1 value
******************************************************************************/
int Sign(double Val)
{
	return Val >= 0 ? 1.0 : -1.0;
}


/*!****************************************************************************
* @brief	Reorders a vector of integer values
* @param	VecInts A reference to a vector of integers
* @note		Uses the Bubble Sort algorithm
******************************************************************************/
void Sort(TVecIntegers& VecInts)
{
	for(unsigned i=0; i<VecInts.size(); i++)
	{
		for(unsigned j = i+1; j<VecInts.size(); j++)
		{
			if( VecInts[i] > VecInts[j] )
			{
				std::swap(VecInts[i], VecInts[j]);
			}
		}
	}
}
