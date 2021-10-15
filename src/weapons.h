#ifndef _WEAPONS_H_
#define _WEAPONS_H_

#include <math.h>
#include <vector>

//#include <sdl2/sdl.h>

#include "maths.h"
#include "vectors.h"

#include "video.h"

struct TShip;

struct TMissile
{
	bool bArmed;
	TVector2 Pos, Vel;
	COLORREF Color;
	TVideoManager *pVM;

	TShip* pShip;
};

typedef std::vector<TMissile*> TVecPtrMissiles;

void Build(TMissile* pMissile, TVideoManager* pVM);
void Build(TMissile* pMissile, TVideoManager* pVM, TVector2 Pos, TVector2 Vel);

TVector2 GetPos(TMissile* pMissile);
bool IsArmed(TMissile* pMissile);
void Arm(TMissile* pMissile, TVector2 Pos, TVector2 Vel);
void Update(TMissile* pMissile, double Dt);

void Clear(TVecPtrMissiles& Missiles);

#endif

