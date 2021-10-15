/**
* @file ships.h
* @author Francesco Settembrini
* @date 23/6/2021
* @version 1.0.0
*/

#ifndef _SHIPS_H_
#define _SHIPS_H_

#include <windows.h>
#include <vector>
//#include <sdl2/sdl.h>

#include "vectors.h"
#include "audio.h"
#include "video.h"


#define SHIP_SIZE				16
#define SHIP_ROTSTEP			10.0
#define SHIP_IMPULSE			4.0
#define SHIP_VELSTEP			0.5
#define SHIP_MAXVEL				250.0
#define SHIP_IMPULSETICKS		20
#define SHIP_EXPLOSIONTICKS		64
#define SHIP_NDEBRIS			16


enum enShipClass { scHuman, scAlienSmall, scAlienBig };

struct TShip;
typedef std::vector<TShip*> TVecPtrShips;


struct TShip
{
	TVideoManager *pVM;
	TSoundManager *pSM;

	enShipClass nClass;

	COLORREF Color;
	double Rot, Impulse;
	bool bAlive, bVisible;
	TVector2 Size, Pos, Vel;
	TVecVecPoints Shape, Engine, Shield;
	int nImpulseTicks, nExplosionTicks;

	TVector2 Debris[SHIP_NDEBRIS];
	double DebrisScales[SHIP_NDEBRIS];
	int nDebris;

	bool bShield;
	unsigned nShieldTick;
};


void Build(TShip* pShip, TVideoManager *pVM, TSoundManager* pSM,
	enShipClass nClass, TVector2 Size, TVector2 Pos, TVector2 Vel);

void Reset(TShip* pShip);

void SetClass(TShip* pShip, enShipClass nClass);
enShipClass GetClass(TShip* pShip);

void ActivateTheShield(TShip* pShip);
bool IsShieldActive(TShip* pShip);
bool IsExploding(TShip* pShip); 

void Update(TShip* pShip, double Dt);
void SetRot(TShip* pShip, double Rot);
void SetPos(TShip* pShip, TVector2 Pos);
TVector2 GetPos(TShip* pShip);
double GetRot(TShip* pShip);
void SetVel(TShip* pShip, TVector2 Vel);
TVector2 GetVel(TShip* pShip);
void SetColor(TShip* pShip, COLORREF Color);

void RotateLeft(TShip* pShip, double DAngleDeg);
void RotateRight(TShip* pShip, double DAngleDeg);
bool IsVisible(TShip* pShip);
bool IsAlive(TShip* pShip);
void SetVisible(TShip* pShip, bool bVisible);
void SetAlive(TShip* pShip, bool bAlive);

void Impulse(TShip* pShip, double Impulse);

void Explode(TShip* pShip);
void DoExplosion(TShip* pShip);

bool IsColliding(TShip* pShip, TVector2 Pos);

#endif



