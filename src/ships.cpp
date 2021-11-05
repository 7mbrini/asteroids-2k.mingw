/*!****************************************************************************

	@file	ships.h
	@file	ships.cpp

	@brief	Spaceships modelling

	@noop	author:	Francesco Settembrini
	@noop	last update: 23/6/2021
	@noop	e-mail:	mailto:francesco.settembrini@poliba.it

******************************************************************************/

#include <windows.h>
#include <mmsystem.h>
#include <assert.h>
#include <math.h>

//#include <sdl2/sdl.h>

#include "maths.h"
#include "ships.h"
#include "game.h"
#include "vectors.h"


#define SHIELDTICKS		100	///< Durata (in ticks) dello scudo difensivo


/*!****************************************************************************
* @brief	Builds the ship
* @param	pShip Pointer to the ship data structure
* @param	pVM Pointer to the VideoManager
* @param	pSM Pointer to the SoundManager
* @param	nClass Ship class (small, medium, big)
* @param	Size Size of the ship
* @param	Pos Initial position of the ship
* @param	Vel Initial velocity of the ship
******************************************************************************/
void Build(TShip* pShip, TVideoManager *pVM, TSoundManager* pSM,
	 enShipClass nClass, TVector2 Size, TVector2 Pos, TVector2 Vel)
{
	assert(pShip);
	assert(pSM);

	pShip->pVM = pVM;
	pShip->pSM = pSM;

	pShip->Pos = Pos;
	pShip->Vel = Vel;
	pShip->Size = Size;
	pShip->bAlive = true;
	pShip->bVisible = false;
	pShip->Rot = pShip->Impulse = 0;
	pShip->Color = RGB(255, 255, 255);
	pShip->nImpulseTicks = pShip->nExplosionTicks = 0;

	pShip->bShield = false;
	pShip->nShieldTick = 0;

	pShip->nClass = nClass;

	if( GetClass(pShip) == scHuman )
	{
												// human ship
												// build the frame
		{
			pShip->Shape.clear();
			TVecPoints Ship;

			Ship.push_back(TVector2{0.5, -0.5});
			Ship.push_back(TVector2{0, 0.5});
			Ship.push_back(TVector2{-0.5, -0.5});
			Ship.push_back(TVector2{0, -0.25});
			Ship.push_back(TVector2{0.5, -0.5});

			for (unsigned int i = 0; i < Ship.size(); ++i)
			{
				Ship[i].X *= Size.X;
				Ship[i].Y *= Size.Y;
			}

			pShip->Shape.push_back(Ship);
		}
												// build the engine
		{
			pShip->Engine.clear();
			TVecPoints Engine;

			Engine.push_back(TVector2{-0.25, -0.25});
			Engine.push_back(TVector2{-0.1, -0.3});
			Engine.push_back(TVector2{0, -0.75});
			Engine.push_back(TVector2{0.1, -0.3});
			Engine.push_back(TVector2{0.25, -0.25});

			for (unsigned int i = 0; i < Engine.size(); ++i)
			{
				Engine[i].X *= Size.X;
				Engine[i].Y *= Size.Y;
			}
		
			pShip->Engine.push_back(Engine);
		}
												// build the shield
		{
			pShip->Shield.clear();	
			TVecPoints Shield;

			int nItems = 64;

			double DAngle = 2.0*M_PI/double(nItems);
			double ShieldSize = Size.X * 1.25;

			for(int i=0; i<nItems; i++)
			{
				Shield.push_back(TVector2 { cos(i*DAngle)*ShieldSize, sin(i*DAngle)*ShieldSize } );
			}

			Shield.push_back( Shield[0] );

			pShip->Shield.push_back(Shield);
		}
	}
	else
	{
											// alien ship
											// build the frame
		{
			pShip->Shape.clear();
			TVecPoints Ship;

			Ship.push_back(TVector2{0.5, 0});
			Ship.push_back(TVector2{0.25, 0.25});
			Ship.push_back(TVector2{-0.25, 0.25});
			Ship.push_back(TVector2{-0.5, 0});
			Ship.push_back(TVector2{-0.25, -0.25});
			Ship.push_back(TVector2{0.25, -0.25});
			Ship.push_back(TVector2{0.5, 0});
			Ship.push_back(TVector2{-0.5, 0});

			for (unsigned int i = 0; i < Ship.size(); ++i)
			{
				Ship[i].X *= Size.X;
				Ship[i].Y *= -Size.Y;
			}
												// windshield
			TVecPoints WShield;
			WShield.push_back(TVector2{0.2, 0.25});
			WShield.push_back(TVector2{0.15, 0.4});
			WShield.push_back(TVector2{-0.15, 0.4});
			WShield.push_back(TVector2{-0.2, 0.25});

			for (unsigned int i = 0; i < WShield.size(); ++i)
			{
				WShield[i].X *= Size.X;
				WShield[i].Y *= -Size.Y;
			}

			pShip->Shape.push_back(Ship);
			pShip->Shape.push_back(WShield);
		}
	}
}

/*!****************************************************************************
* @brief	Set the ship class (small, medium, big)
* @param	pShip	Pointer to the ship data structure
* @param	nClass The shiip class
******************************************************************************/
void SetClass(TShip* pShip, enShipClass nClass)
{
	assert(pShip);

	pShip->nClass = nClass;
}

/*!****************************************************************************
* @brief	Return the ship class
* @param	pShip Pointer to the ship data structure
* @return	The ship class
******************************************************************************/
enShipClass GetClass(TShip* pShip)
{
	assert(pShip);

	return pShip->nClass;
}

/*!****************************************************************************
* @brief	Reset the ship to the default properties
* @param	pShip Pointer to the ship data structure
******************************************************************************/
void Reset(TShip* pShip)
{
	assert(pShip);

	pShip->Pos = TVector2{0, 0};
	pShip->Vel = TVector2{0, 0};
	pShip->Rot = 0;
	pShip->nExplosionTicks = -1;
	pShip->nImpulseTicks = 0;

	pShip->bShield = false;
	pShip->nShieldTick = 0;

	SetAlive(pShip, true);
	SetVisible(pShip, false);
}

/*!****************************************************************************
* @brief	Return true if the ship is exploding
* @param	pShip Pointer to the ship data structure
* @return	True if the ship is exploding, false otherwise
******************************************************************************/
bool IsExploding(TShip* pShip)
{
	assert(pShip);

	return pShip->nExplosionTicks > 0;
}

/*!****************************************************************************
* @brief	Set the ship position
* @param	pShip Pointer to the ship data structure
* @param	Pos The position to be set for the ship
******************************************************************************/
void SetPos(TShip* pShip, TVector2 Pos)
{
	assert(pShip);

	pShip->Pos = Pos;
}

/*!****************************************************************************
* @brief	Return the ship position
* @param	pShip Pointer to the ship data structure
* @return	The ship position
******************************************************************************/
TVector2 GetPos(TShip* pShip)
{
	assert(pShip);

	return pShip->Pos;
}

/*!****************************************************************************
* @brief	Set the ship rotation
* @param	pShip Pointer to the ship data structure
* @param	Rot Rotation angle in radians
******************************************************************************/
void SetRot(TShip* pShip, double Rot)
{
	assert(pShip);

	pShip->Rot = Rot;
}

/*!****************************************************************************
* @brief	Return the ship rotation
* @param	pShip Pointer to the ship data structure
* @return	Return the rotation angle in radians
******************************************************************************/
double GetRot(TShip* pShip)
{
	assert(pShip);

	return pShip->Rot;
}

/*!****************************************************************************
* @brief	Set the ship velocity
* @param	pShip Pointer to the ship data structure
* @param	Vel The velocity value to be set for the ship
******************************************************************************/
void SetVel(TShip* pShip, TVector2 Vel)
{
	assert(pShip);

	pShip->Vel = Vel;
}

/*!****************************************************************************
* @brief	Return the ship velocity
* @param	pShip Pointer to the ship data structure
* @return	The velocity value of the ship
******************************************************************************/
TVector2 GetVel(TShip* pShip)
{
	assert(pShip);

	return pShip->Vel;
}

/*!****************************************************************************
* @brief	Set the color for the ship
* @param	pShip Pointer to the ship data structure
* @param	Color The color to be set for the ship
******************************************************************************/
void SetColor(TShip* pShip, COLORREF Color)
{
	assert(pShip);

	pShip->Color = Color;
}

/*!****************************************************************************
* @brief	Rotate the ship to the left
* @param	pShip Pointer to the ship data structure
* @param	DAngleDeg The rotation value in degrees
******************************************************************************/
void RotateLeft(TShip* pShip, double DAngleDeg)
{
	assert(pShip);

	pShip->Rot += DAngleDeg;
}

/*!****************************************************************************
* @brief	Rotate the ship to the right
* @param	pShip Pointer to the ship data structure
* @param	DAngleDeg The rotation value in degrees
******************************************************************************/
void RotateRight(TShip* pShip, double DAngleDeg)
{
	assert(pShip);

	pShip->Rot -= DAngleDeg;
}

/*!****************************************************************************
* @brief	Return the ship visibility
* @param	pShip Pointer to the ship data structure
* @return	True if the ship is visible, false otherwise
******************************************************************************/
bool IsVisible(TShip* pShip)
{
	assert(pShip);

	return pShip->bVisible;
}

/*!****************************************************************************
* @brief	Returns the ship status, alive or not
* @param	pShip Pointer to the ship data structure
* @return	True if the ship is alive, false otherwise
******************************************************************************/
bool IsAlive(TShip* pShip)
{
	assert(pShip);

	return pShip->bAlive;
}

/*!****************************************************************************
* @brief	Set the ship visiblity
* @param	pShip Pointer to the ship data structure
* @param	bVisible True for ship visiblitiy, false otherwise
******************************************************************************/
void SetVisible(TShip* pShip, bool bVisible)
{
	pShip->bVisible = bVisible;


	if( bVisible )
	{
		if( GetClass(pShip) == scAlienBig )
		{
			PlayTheSound(pShip->pSM, "saucer_big", true);
		}
		else if( GetClass(pShip) == scAlienSmall )
		{
			PlayTheSound(pShip->pSM, "saucer_small", true);
		}
	}
	else
	{
		if( GetClass(pShip) == scAlienBig )
		{
			StopTheSound(pShip->pSM, "saucer_big");
		}
		else if( GetClass(pShip) == scAlienSmall )
		{
			StopTheSound(pShip->pSM, "saucer_small");
		}
	}

}

/*!****************************************************************************
* @brief	Set the aliving status for the ship
* @param	pShip Pointer to the ship data structure
* @param	bAlive True for ship alivenes, false otherwise
******************************************************************************/
void SetAlive(TShip* pShip, bool bAlive)
{
	assert(pShip);

	pShip->bAlive = bAlive;
}

/*!****************************************************************************
* @brief	Give an impulse to the ship
* @param	pShip Pointer to the ship data structure
* @param	Impulse The value of the impulse to be given to the spaceship
******************************************************************************/
void Impulse(TShip* pShip, double Impulse)
{
	assert(pShip);
	assert(pShip->pSM);

	pShip->nImpulseTicks = SHIP_IMPULSETICKS;

											// gives the impulse in the same
											// direction of the ship heading
	pShip->Vel.X += cos( DEG2RAD(pShip->Rot - 90.0) ) * Impulse;
	pShip->Vel.Y += sin( DEG2RAD(pShip->Rot + 90.0) ) * Impulse;

											// limit the speed
	if (pShip->Vel.X > SHIP_MAXVEL) pShip->Vel.X = SHIP_MAXVEL;
	if (pShip->Vel.Y > SHIP_MAXVEL) pShip->Vel.Y = SHIP_MAXVEL;

											// plays the thrust sound
	unsigned nDelay = 250;
	static unsigned nCurTick = nDelay;

	if( (::GetTickCount() - nCurTick ) >= nDelay )
	{
		nCurTick = GetTickCount();

		PlayTheSound(pShip->pSM, "ship_thrust");
	}
}

/*!****************************************************************************
* @brief	Manages the explosion of the spaceship
* @param	pShip Pointer to the ship data structure
******************************************************************************/
void Explode(TShip* pShip)
{
	assert(pShip);
	assert(pShip->pSM);

	SetAlive(pShip, false);
	SetVisible(pShip, false);

	pShip->nDebris = SHIP_NDEBRIS / 2.0 + abs(Rand(SHIP_NDEBRIS)) / 2.0;

	double Scale = 8.0;
	double DAngle = (2.0 * M_PI) / pShip->nDebris;

	for(int i=0; i < pShip->nDebris; ++i)
	{
		double Radius = SHIP_SIZE / 4.0 + abs(Rand(SHIP_SIZE));

		pShip->Debris[i].X = pShip->Pos.X + cos(i*DAngle) * Radius;
		pShip->Debris[i].Y = pShip->Pos.Y + sin(i*DAngle) * Radius;

		pShip->DebrisScales[i] = fabs(Rand(Scale));
		pShip->DebrisScales[i] = fabs(Rand(Scale));
	}

	pShip->nExplosionTicks = SHIP_EXPLOSIONTICKS;

	PlayTheSound(pShip->pSM, "ship_explosion");

	DoExplosion(pShip);
}

/*!****************************************************************************
* @brief	Animate the explosion of the spaceship
* @param	pShip Pointer to the ship data structure
******************************************************************************/
void DoExplosion(TShip* pShip)
{
	assert(pShip);

	double Scale = 16;
	pShip->nExplosionTicks--;

	int nCurTick = SHIP_EXPLOSIONTICKS - pShip->nExplosionTicks;

	if( pShip->nExplosionTicks > 0 )
	{
		BYTE Brightness = 255.0/ double(SHIP_EXPLOSIONTICKS) * pShip->nExplosionTicks;

		//SDL_SetRenderDrawColor(pShip->pRenderer, Brightness, Brightness, Brightness, 255);

		for(int i=0; i<pShip->nDebris; i++)
		{
			double X = pShip->Pos.X + (pShip->Debris[i].X - pShip->Pos.X) * (Scale + pShip->DebrisScales[i]) / 100.0 * nCurTick;
			double Y = pShip->Pos.Y + (pShip->Debris[i].Y - pShip->Pos.Y) * (Scale + pShip->DebrisScales[i]) / 100.0 * nCurTick;

			//SDL_RenderDrawPoint(pShip->pRenderer, X, Y);

			TVector2 Pos { X, Y };
			DrawPoint(pShip->pVM, Pos, RGB(Brightness, Brightness, Brightness) );
		}
	}
}

/*!****************************************************************************
* @brief	Updates by time the spaceship status
* @param	pShip Pointer to the ship data structure
* @param	Dt The value for the delta time
******************************************************************************/
void Update(TShip* pShip, double Dt)
{
	assert(pShip);
	assert(pShip->pVM);
	assert(pShip->Shape.size() > 0);

	if ( IsAlive(pShip) )
	{
		if( GetClass(pShip) == scHuman )
		{
			pShip->nShieldTick++;
			if( pShip->nShieldTick > SHIELDTICKS )
			{
				pShip->bShield = false;
			}

			TVecVecPoints Shape = pShip->Shape;

			Rotate(Shape, pShip->Rot);
			TVector2 Vel = GetVel(pShip);
			pShip->Pos.X += Vel.X * Dt;
			pShip->Pos.Y += Vel.Y * Dt;
			Translate(Shape, pShip->Pos);

			DrawLines(pShip->pVM, Shape, 0, pShip->Color);

												// draw the engine
			pShip->nImpulseTicks--;

			if (pShip->nImpulseTicks > 0)
			{
				TVecVecPoints Engine = pShip->Engine;

				Rotate(Engine, pShip->Rot);
				Translate(Engine, pShip->Pos);

				DrawLines(pShip->pVM, Engine, 0, pShip->Color);
			}

											// draw the shield
			if( IsShieldActive(pShip) )
			{
				TVecVecPoints Shield = pShip->Shield;

				Translate(Shield, pShip->Pos);

											// some special effects ...
				{
					static int nCounter = 0, nMaxCount = 4;
					if( nCounter++ > nMaxCount ) nCounter = 0;
					double ShadeLevel = double(nCounter) / double(nMaxCount);

											// ... blink the shield when time is running out
					if( pShip->nShieldTick > SHIELDTICKS*3.0/4.0)
					{
						DrawLines(pShip->pVM, Shield, 0, pShip->Color * ShadeLevel);
					}
					else
					{
						DrawLines(pShip->pVM, Shield, 0, pShip->Color);
					}
				}
			}
		}
		else
		{
			static int nCounter = 0;
			nCounter++;

			TVecVecPoints Shape = pShip->Shape;

			TVector2 Vel = GetVel(pShip);

			double Module = 5.0;

			if( nCounter > 25 )
			{
				nCounter = 0;

				Vel.Y += Rand(2.0*Module);
				Vel.X += AbsRand(Module);
			}

			SetVel(pShip, Vel);

			pShip->Pos.X += Vel.X * Dt;
			pShip->Pos.Y += Vel.Y * Dt;
			Translate(Shape, pShip->Pos);

			DrawLines(pShip->pVM, Shape, 0, pShip->Color);

												// draw the engine
			pShip->nImpulseTicks--;

			if (pShip->nImpulseTicks > 0)
			{
				TVecVecPoints Engine = pShip->Engine;

				Rotate(Engine, pShip->Rot);
				Translate(Engine, pShip->Pos);

				DrawLines(pShip->pVM, Engine, 0, pShip->Color);
			}
		}
	}
	else if ( IsExploding(pShip) )
	{
		DoExplosion(pShip);
	}
}

/*!****************************************************************************
* @brief	Objects collision detection
* @param	pShip Pointer to the ship data structure
* @param	Pos Position to check for collision
******************************************************************************/
bool IsColliding(TShip* pShip, TVector2 Pos)
{
	assert(pShip);

	return bool( Distance(GetPos(pShip), Pos) <= pShip->Size.X);
}

/*!****************************************************************************
* @brief Activate the shield to protect the ship against alien missiles.
* @param pShip Pointer to the ship data structure
******************************************************************************/
void ActivateTheShield(TShip* pShip)
{
	assert(pShip);

	if( pShip->nShieldTick > SHIELDTICKS )
	{
		pShip->nShieldTick = 0;
		pShip->bShield = true;

		PlayTheSound(pShip->pSM, "shield");	
	}
}

/*!****************************************************************************
* @brief Return the status of the shield
* @param pShip Pointer to the ship
******************************************************************************/
bool IsShieldActive(TShip* pShip)
{
	assert(pShip);

	return pShip->bShield;
}

