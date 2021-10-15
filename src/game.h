#ifndef _GAME_H_
#define _GAME_H_

#include <windows.h>
#include <map>
#include <string>
#include <vector>
#include <stdio.h>

//#include <sdl2/sdl.h>
//#include <sdl2/sdl_audio.h>
//#include <sdl2/sdl_ttf.h>

#include "audio.h"
#include "video.h"

#include "ships.h"
#include "weapons.h"
#include "asteroids.h"


struct TRecordScores
{
	unsigned nScore;
	std::string strName;
};

typedef std::vector<std::string> TVecStrings;
typedef std::vector<TRecordScores> TVecRecordScores;

struct TGame
{
	TVideoManager* pVM;
	TSoundManager* pSM;

	bool bRun, bPause, bGameOver;
	TVecPtrShips pShips;

	TVecPtrMissiles pMissiles;
	TVecPtrAsteroids pAsteroids;
	int nScore, nLevel, nDifficulty, nLives, nBonusCount;

	TVecStrings strHelp;
								// best score dialog
	WCHAR pBestScoresName[256];
	TVecRecordScores BestScores;
	unsigned nDlgRetVal;
	bool bBestScoreDlgActive;
};


bool Setup(TGame* pGame, TVideoManager* pVM, TSoundManager* pSM);

void Cleanup(TGame* pGame);
void Restart(TGame* pGame);

void GameOver(TGame* pGame);

void Run(TGame* pGame);
void ShowInfo(TGame* pGame);
void NextLevel(TGame* pGame);

bool IsSafetyPos(TGame* pGame, TVector2 Pos);

void ShotTheMissile(TGame* pGame, TShip* pShip);

void GetClientSize(TGame* pGame, unsigned& nW, unsigned& nH);

void EndTheGame(TGame* pGame);
void PauseTheGame(TGame* pGame);
bool IsPausing(TGame* pGame);

bool IsRunning(TGame* pGame);
bool IsGameOver(TGame* pGame);

bool LoadTheHelp(TGame* pGame);
bool LoadTheSounds(TGame* pGame);

bool IsBestScore(TGame* pGame);
void RegisterBestScore(TGame* pGame);
void SaveBestScores(TGame* pGame);
bool LoadTheBestScores(TGame* pGame, char* pFileName);

bool BuildTheFonts(TGame* pGame);
void BuildTheAsteroids(TGame* pGame, unsigned nCount);
void DeleteAsteroids(TGame* pGame);

bool BuildTheShips(TGame* pGame);

void ForceInsideLimits(TGame* pGame);
bool IsInsideGameArea(TGame* pGame, TVector2 Pos);

void DeleteShips(TGame* pGame);

void LevelHandler(TGame* pGame);
void BonusHandler(TGame* pGame);
void CollisionHandler(TGame* pGame);

bool Collide(TMissile* pMissile, TShip* pShip);
bool Collide(TAsteroid* pAsteroid, TShip* pShip);

bool IsInputDialog(TGame* pGame);

void GameOverHandler(TGame* pGame);

void AlienShipsHandler(TGame* pGame);

HWND GetMainWnd(TGame* pGame);

#ifndef _MSC_VER
	void Swap(TRecordScores& T1, TRecordScores& T2);
#endif

void Sort(TVecRecordScores& Scores, bool bAscending=true);

#endif

