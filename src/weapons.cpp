/*!****************************************************************************

	@file	weapons.h
	@file	weapons.cpp

	@brief	Weapons modelling

	@noop	author:	Francesco Settembrini
	@noop	last update: 23/6/2021
	@noop	e-mail:	mailto:francesco.settembrini@poliba.it

******************************************************************************/

#include <windows.h>

#include "weapons.h"



/*!****************************************************************************
* @brief	Builds the missile
* @param	pMissile Pointer to the missile data structure
* @param	pVM Pointer to the video manager data structure
******************************************************************************/
void Build(TMissile* pMissile, TVideoManager* pVM)
{
	assert(pMissile);
	assert(pVM);

	pMissile->Pos = TVector2{-10,-10};
	pMissile->Vel = TVector2{0,0};
	pMissile->bArmed = false;
	pMissile->pVM = pVM;

	pMissile->pShip = nullptr;
}

/*!****************************************************************************
* @brief	Builds the missile
* @param	pMissile Pointer to the missile data structure
* @param	pVM Pointer to the video manager data structure
* @param	Pos Initial position of the missile
* @param	Vel Initial velocity of the missile
******************************************************************************/
void Build(TMissile* pMissile, TVideoManager* pVM, TVector2 Pos, TVector2 Vel)
{
	assert(pMissile);
	assert(pVM);

	pMissile->Pos = Pos;
	pMissile->Vel = Vel;
	pMissile->bArmed = false;
	pMissile->pVM = pVM;

	pMissile->pShip= nullptr;
}

/*!****************************************************************************
* @brief	Gets the missile position
* @param	pMissile Pointer to the missile data structure
* @return	The position of the missile
******************************************************************************/
TVector2 GetPos(TMissile* pMissile)
{
	assert(pMissile);

	return pMissile->Pos;
}

/*!****************************************************************************
* @brief	Gets the status of missile
* @param	pMissile Pointer to the missile data structure
* @return	Returns true if the missile is armed, false otherwise
******************************************************************************/
bool IsArmed(TMissile* pMissile)
{
	assert(pMissile);

	return pMissile->bArmed;
}

/*!****************************************************************************
* @brief	Arms the missile
* @param	pMissile Pointer to the missile data structure
* @param	Pos The position of the missile
* @param	Vel The velocity of the missile
******************************************************************************/
void Arm(TMissile* pMissile, TVector2 Pos, TVector2 Vel)
{
	assert(pMissile);

	pMissile->Pos = Pos;
	pMissile->Vel = Vel;
	pMissile->bArmed = true;
	pMissile->Color = RGB(255,255,255);
}

/*!****************************************************************************
* @brief	Updates by time the missile status
* @param	pMissile Pointer to the missile data structure
* @param	Dt The value for the delta time
******************************************************************************/
void Update(TMissile* pMissile, double Dt)
{
	assert(pMissile);
	assert(pMissile->pVM);

	pMissile->Pos.X += pMissile->Vel.X * Dt;
	pMissile->Pos.Y += pMissile->Vel.Y * Dt;
/*
	SDL_SetRenderDrawColor(pMissile->pRenderer,
		GetRValue(pMissile->Color), GetGValue(pMissile->Color), GetBValue(pMissile->Color), 255);

	SDL_RenderDrawPoint(pMissile->pRenderer, pMissile->Pos.X, pMissile->Pos.Y);
*/

	DrawPoint(pMissile->pVM, pMissile->Pos, pMissile->Color);
}

/*!****************************************************************************
* @brief	Deletes a bunch of missiles referenced by a vector of pointers
* @param	Missiles Referernce to a list of pointers to missiles
******************************************************************************/
void Clear(TVecPtrMissiles& Missiles)
{
	for(int i=0; i<Missiles.size(); i++)
	{
		if( Missiles[i] )
		{
			delete Missiles[i];
			Missiles[i] = nullptr;
		}
	}

	Missiles.clear();
}
