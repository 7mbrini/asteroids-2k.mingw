/*!****************************************************************************

	@file	asteroids.h
	@file	asteroids.cpp

	@brief	Asteroids modelling

	@noop	author:	Francesco Settembrini
	@noop	last update: 23/6/2021
	@noop	e-mail:	mailto:francesco.settembrini@poliba.it

******************************************************************************/

#include <math.h>

#include "maths.h"
#include "asteroids.h"
#include "commdefs.h"
#include "utils.h"


#define ASTEROID_MAXVERTS	16


/*!****************************************************************************
* @brief	Builds the asteroid
* @param	pAsteroid Pointer to the asteroid data structure
* @param	pVM Pointer to the video manager data structure
* @param	nClass Class identifier of the asteroid (e.g.: big, medium, small)
* @param	Pos The initial position of the asteroid
* @param	Radius The size of the asteroid
******************************************************************************/
void Build(TAsteroid* pAsteroid, TVideoManager* pVM,
	enAsteroidClass nClass, TVector2 Pos, TVector2 Vel, double Radius)
{
	assert(pVM);

	pAsteroid->pVM = pVM;

	pAsteroid->Radius = Radius;
	pAsteroid->nClass = nClass;
	pAsteroid->Pos = Pos;
	pAsteroid->Vel = Vel;
	pAsteroid->bAlive = true;
	pAsteroid->Color = RGB(255,255,255);

	pAsteroid->Shape = RandShape(Radius);

	pAsteroid->Rot = 0;
	pAsteroid->DRot = rand()/double(RAND_MAX) * Mod(Vel) * 0.25 * RandSign();
}

/*!****************************************************************************
* @brief	Returns the asteroid status
* @param	pAsteroid Pointer to the asteroid data structure
* @return	True if the asteroid is available, false otherwise
******************************************************************************/
bool IsAlive(TAsteroid* pAsteroid)
{
	return pAsteroid->bAlive;
}

/*!****************************************************************************
* @brief	Checks if the asteroid is colliding in a specified position
* @param	pAsteroid Pointer to the asteroid data structure
* @return	True if the asteroid is colliding, false otherwise
******************************************************************************/
bool Collide(TAsteroid* pAsteroid, TVector2 Pt)
{
	return bool( Distance(Pt, pAsteroid->Pos) <= pAsteroid->Radius );
}

/*!****************************************************************************
* @brief	Marks the availability of the asteroid
* @param	pAsteroid Pointer to the asteroid data structure
******************************************************************************/
void Crash(TAsteroid* pAsteroid)
{
	pAsteroid->bAlive = false;
}

/*!****************************************************************************
* @brief	Sets the position of the asteroid
* @param	pAsteroid Pointer to the asteroid data structure
* @param	Pos The position to be set for the asteroid
******************************************************************************/
void SetPos(TAsteroid* pAsteroid, TVector2 Pos)
{
	pAsteroid->Pos = Pos;
}

/*!****************************************************************************
* @brief	Gets the position of the asteroid
* @param	pAsteroid Pointer to the asteroid data structure
* @return	The position of the asteroid
******************************************************************************/
TVector2 GetPos(TAsteroid* pAsteroid)
{
	return pAsteroid->Pos;
}

/*!****************************************************************************
* @brief	Gets the veclocity of the asteroid
* @param	pAsteroid Pointer to the asteroid data structure
* @return	The velocity of the asteroid
******************************************************************************/
TVector2 GetVel(TAsteroid* pAsteroid)
{
	return pAsteroid->Vel;
}

/*!****************************************************************************
* @brief	Gets the class of the asteroid
* @param	pAsteroid Pointer to the asteroid data structure
* @return	The clas of the asteroid (i.e. small, medium or big)
******************************************************************************/
enAsteroidClass GetClass(TAsteroid* pAsteroid)
{
	return pAsteroid->nClass;
}

/*!****************************************************************************
* @brief	Generates randomly an asteroid shape
* @param	Size The size of the asteroid to be generated
******************************************************************************/
TVecPoints RandShape(double Size)
{
	TVecPoints Pts;
	double Angle = 0;
	double DAngle = 360.0/ASTEROID_MAXVERTS;

	for(int i=0; i<ASTEROID_MAXVERTS; ++i)
	{
		Pts.push_back( TVector2{
			Size * cos(Angle * M_PI/180.0f + 0.5*rand()/double(RAND_MAX)),
			Size * sin(Angle * M_PI/180.0f + 0.5*rand()/double(RAND_MAX))
			});

		Angle += DAngle;
	}

	return Pts;
}

/*!****************************************************************************
* @brief	Updates by time the asteroid status
* @param	pAsteroid Pointer to the asteroid data structure
* @param	Dt The value for the delta time
******************************************************************************/
void Update(TAsteroid* pAsteroid, double Dt)
{
	assert(pAsteroid->pVM);

	pAsteroid->Pos.X += pAsteroid->Vel.X * Dt;
	pAsteroid->Pos.Y += pAsteroid->Vel.Y * Dt;

	TVecPoints Shape = pAsteroid->Shape;

	pAsteroid->Rot += pAsteroid->DRot;

	Rotate(Shape, pAsteroid->Rot);
	Translate(Shape, pAsteroid->Pos);

/*
	SDL_SetRenderDrawColor(pAsteroid->pRenderer,
		GetRValue(pAsteroid->Color), GetGValue(pAsteroid->Color), GetBValue(pAsteroid->Color), 255);

	for (int i = 1; i < Shape.size(); ++i)
	{
		SDL_RenderDrawLine(pAsteroid->pRenderer,
			Shape[i-1].X, Shape[i-1].Y, Shape[i].X, Shape[i].Y);
	}
											// close the polyline
	SDL_RenderDrawLine(pAsteroid->pRenderer,
		Shape[Shape.size()-1].X, Shape[Shape.size()-1].Y, Shape[0].X, Shape[0].Y);
*/

	DrawLines(pAsteroid->pVM, Shape, 0, pAsteroid->Color, true);
}

