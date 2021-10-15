#ifndef _ASTEROIDS_H_
#define _ASTEROIDS_H_

#include <windows.h>
#include <map>
#include <string>
#include <vector>
#include <stdio.h>

//#include <sdl2/sdl.h>

#include "video.h"
#include "vectors.h"


enum enAsteroidClass { acBig, acMedium, acSmall };

struct TAsteroid
{
	bool bAlive;
	COLORREF Color;
	double Radius, Rot, DRot;
	TVecPoints Shape;
	TVector2 Pos, Vel, Acc;
	TVideoManager* pVM;
	enAsteroidClass nClass;
};

typedef std::vector<TAsteroid> TVecAsteroids;
typedef std::vector<TAsteroid*> TVecPtrAsteroids;

bool Collide(TAsteroid* pAsteroid, TVector2 Pt);
bool IsAlive(TAsteroid* pAsteroid);
void Crash(TAsteroid* pAsteroid);

void SetPos(TAsteroid* pAsteroid, TVector2 Pos);
TVector2 GetPos(TAsteroid* pAsteroid);
TVector2 GetVel(TAsteroid* pAsteroid);
enAsteroidClass GetClass(TAsteroid* pAsteroid);

void Update(TAsteroid* pAsteroid, double DeltaT);
TVecPoints RandShape(double Size);

void Build(TAsteroid* pAsteroid, TVideoManager* VideoManager,
	enAsteroidClass nClass, TVector2 Pos, TVector2 Vel, double Radius);

#endif

