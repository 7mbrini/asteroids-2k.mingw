/*!****************************************************************************

	@file	game.h	
	@file	game.cpp

	@brief	Game logic implementation

	@noop	author:	Francesco Settembrini
	@noop	last update: 23/6/2021
	@noop	e-mail:	mailto:francesco.settembrini@poliba.it

******************************************************************************/

#include <windows.h>
#include <winuser.h>
#include <mmsystem.h>

#include <locale>
#include <codecvt>
#include <string>

#include <algorithm>

									// openal audio engine
#include <al/al.h>
#include <al/alc.h>
									// sdl game engine
/*
#include <sdl2/sdl.h>
#include <sdl2/sdl_ttf.h>
#include <sdl2/sdl_audio.h>
#include <sdl2/sdl_syswm.h>
*/

#include "audio.h"
#include "video.h"

#include "game.h"
#include "utils.h"
#include "vectors.h"
#include "commdefs.h"

#include "resource.h"


#define DT	0.1

#define MAXLIVES		3
#define STARTLEVEL		1
#define STARTSCORE		0
#define BONUSCOUNTER	1
#define BONUSPOINTS		1000

#define SHOTDELAY		150
#define MAXASTEROIDS	5

#define ASTEROIDVEL			10
#define ASTEROIDVELRATIO	2
#define ASTEROIDBIGSIZE		30
#define ASTEROIDMIDSIZE 	20
#define ASTEROIDSMALLSIZE	10

#define BIGASTEROIDSCORE	5
#define MIDASTEROIDSCORE    10
#define SMALLASTEROIDSCORE	20

#define BIGALIENSHIPSCORE	100
#define SMALLALIENSHIPSCORE	500

#define SAFETYDISTANCE		(2.0*ASTEROIDBIGSIZE)

#define SPLASHDELAY			5000

#define ALIENSHOTDELAY		20
#define HUMANSHOTDELAY		100

#define MISSILESPEED		100.0


#define ALIENSHIPTICK		500

#define ALIENBIGINACCURACY		(M_PI/16.0)
//#define ALIENSMALLINACCURACY	(M_PI/32.0)
#define ALIENSMALLINACCURACY	(M_PI/64.0)



extern TGame* g_pGame;


/*!****************************************************************************
* @brief	Callback function for best scores input dialog
* @param	hDlg Handle to parent window
* @param	nMessage Message identifier
* @param	wParam WPARAM type message parameter
* @param	lParam LPARAM type message parameter
******************************************************************************/
BOOL CALLBACK BestScoresDlgProc(HWND hDlg, UINT nMessage, WPARAM wParam, LPARAM lParam)
{
	assert(g_pGame);

	switch( nMessage )
	{
		case WM_INITDIALOG:
			// SetDlgItemText(hDlg, IDC_EDIT_NAME, TEXT("YourName"));
		return TRUE;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case(IDOK):

					GetDlgItemTextW(hDlg, IDC_EDIT_NAME, g_pGame->pBestScoresName, 16);	// considera solo i primi 16 caratteri

					g_pGame->nDlgRetVal = IDOK;

					EndDialog(hDlg, 0);
					return TRUE;
				
				case(IDCANCEL):

					g_pGame->nDlgRetVal = IDCANCEL;
					EndDialog(hDlg, 0);
					return TRUE;
			}
		break;
	}

	return FALSE;
}


/*!****************************************************************************
* @brief	Setting up the game engine
* @param	pGame Pointer to the game engine
* @param	pVM Pointer to the VideoManager
* @param	pSM Pointer to the SoundManager
* @return	Returns true for success, false otherwise
******************************************************************************/
bool Setup(TGame* pGame, TVideoManager* pVM, TSoundManager* pSM)
{
	assert(pGame);
	assert(pVM);
	assert(pSM);

	bool bResult = true;

	pGame->pVM = pVM;
	pGame->pSM = pSM;

	pGame->nLevel = STARTLEVEL;
	pGame->bRun = true;
	pGame->bPause = false;
	pGame->nLives = MAXLIVES;
	pGame->nScore = STARTSCORE;
	pGame->nBonusCount = BONUSCOUNTER;
	pGame->bBestScoreDlgActive = false;


	if( !BuildTheFonts(pGame) )
	{
		::MessageBox(0,
			TEXT("Error: Cannot find FONTS data file!\n\nPress any key to exit ..."),
			TEXT("Fatal Error"),
			MB_OK | MB_ICONSTOP | MB_TASKMODAL);
		exit(-1);
	}


	if( !BuildTheShips(pGame) )
	{
		::MessageBox(0,
			TEXT("Error: Cannot build the ships!\n\nPress any key to exit ..."),
			TEXT("Fatal Error"),
			MB_OK | MB_ICONSTOP | MB_TASKMODAL);
		exit(-1);
	}


	if( !LoadTheSounds(pGame) )
	{
		::MessageBox(0,
			TEXT("Error: Cannot load the sounds!"),
			TEXT("Fatal Error"),
			MB_OK | MB_ICONERROR | MB_TASKMODAL);

		exit(-1);
	}


	if( !LoadTheHelp(pGame) )
	{
		::MessageBox(0,
			TEXT("Error: Cannot open the help file!"),
			TEXT("Fatal Error"),
			MB_OK | MB_ICONERROR | MB_TASKMODAL);

		exit(-1);
	}

	std::string strScoresFile = std::string(GetDataPath()) + std::string(SCORESFILE);
	if( !LoadTheBestScores(pGame, (char*) strScoresFile.c_str()) )
	{
		::MessageBox(0,
			TEXT("Error: Cannot open SCORES data file!\n\nPress any key to exit ..."),
			TEXT("Fatal Error"),
			MB_OK | MB_ICONSTOP | MB_TASKMODAL);
		exit(-1);
	}


#ifdef _DEVEL
	BuildTheAsteroids(pGame, 1);
#else
	BuildTheAsteroids(pGame, pGame->nLevel * MAXASTEROIDS );
#endif

	return bResult;
}

/*!****************************************************************************
* @brief	Loads the sounds
* @param	pGame Pointer to the game engine
* @return	Returns true for success, false otherwise
******************************************************************************/
bool LoadTheSounds(TGame* pGame)
{
	assert(pGame);
	assert(pGame->pSM);

	std::vector<std::string> strSounds = {
		"bonus",
		"shield",
		"ship_fire",
		"bang_large",
		"bang_medium",
		"bang_small",
		"saucer_big",
		"saucer_small",
		"ship_thrust",
		"ship_explosion",
		"starwars-trails"
	};

	return LoadTheSounds(pGame->pSM, strSounds);
}

/*!****************************************************************************
* @brief	Loads the help strings information
* @param	pGame Pointer to the game engine
* @return	Returns true for success, false otherwise
******************************************************************************/
bool LoadTheHelp(TGame* pGame)
{
	assert(pGame);

	bool bResult = false;

	std::string strHelpFile = GetDataPath() + std::string(HELPFILE);

	FILE* fp = fopen(strHelpFile.c_str(), "r");

	if( fp )
	{
		pGame->strHelp.clear();

		while( !feof(fp) )
		{
			char Buffer[1024];

			fgets(Buffer, sizeof(Buffer) - 1, fp);

			pGame->strHelp.push_back(Buffer);
		}

		fclose(fp);

		bResult = true;
	}

	return bResult;
}

/*!****************************************************************************
* @brief	Load the best scores from file
* @param	pFileName Pointer to the filename string
* @param	Records Vector of TRecord data structure
* @return	Returns true for success, false otherwise
******************************************************************************/
bool LoadTheBestScores(TGame* pGame, char* pFileName)
{
	assert(pGame);
	assert(pFileName);

	bool bResult = false;

	FILE *fp = fopen(pFileName, "r");

	if( fp )
	{
		pGame->BestScores.clear();

		while( !feof(fp) )
		{
			char Buffer[1024];

			if( fgets(Buffer, sizeof(Buffer)-1, fp) )
			{

				TRecordScores Score;
				memset(&Score, 0, sizeof(Score));
Score.strName = "best";
Score.nScore = 100;


				std::string strTemp(Buffer);
				int nPos = strTemp.find(",");

//Score.strName = strTemp;
		//pGame->BestScores.push_back(Score);

				//std::string strName = strTemp.substr(0, nPos);
				//strcpy(Score.pName, strName.c_str());
				Score.strName = strTemp.substr(0, nPos);

				std::string strScore = strTemp.substr(nPos+1, strTemp.length()-nPos);
				Score.nScore = atoi((char*) strScore.c_str());

				pGame->BestScores.push_back(Score);

			}
		}

		fclose(fp);

		Sort(pGame->BestScores, false);

		bResult = true;
	}

	return bResult;
}

/*!****************************************************************************
* @brief	Loads the fonts
* @param	pGame Pointer to the game engine
* @return	Returns true for success, false otherwise
******************************************************************************/
bool BuildTheFonts(TGame* pGame)
{
	assert(pGame);
	assert(pGame->pVM);

	std::wstring strFontName = L"Techno LCD";
	std::string strFontPath = GetDataPath() + "\\technolcd.ttf";

	return LoadFont(pGame->pVM, strFontPath, strFontName, FONTSIZE);
}

/*!****************************************************************************
* @brief	Set all parameteres to default values and restart the game
* @param	pGame Pointer to the game engine
******************************************************************************/
void Restart(TGame* pGame)
{
	assert(pGame);
	assert(pGame->pSM);
	assert(pGame->pShips.size());

	StopAllSounds(pGame->pSM);

	unsigned nWidth, nHeight;
	GetClientSize(pGame, nWidth, nHeight);

	for(int i=0; i<pGame->pShips.size(); ++i)
	{
		Reset(pGame->pShips[i]);

		if( GetClass(pGame->pShips[i]) == scHuman )
		{
			SetVel(pGame->pShips[i], TVector2{0,0});
			SetRot(pGame->pShips[i], 180.0);
			SetPos(pGame->pShips[i], TVector2{nWidth/2.0, nHeight/2.0} );

			SetAlive(pGame->pShips[i], true);
			SetVisible(pGame->pShips[i], true);
		}
		else
		{
			SetVel(pGame->pShips[i], TVector2{0,0});
			SetRot(pGame->pShips[i], 0);
			SetPos(pGame->pShips[i], TVector2{ -100, -100 } );

			SetAlive(pGame->pShips[i], false);
			SetVisible(pGame->pShips[i], false);
		}
	}

	pGame->nLives = MAXLIVES;
	pGame->nLevel = STARTLEVEL;
	pGame->nScore = STARTSCORE;
	pGame->nBonusCount = BONUSCOUNTER;
	pGame->bGameOver = false;


#ifdef _DEVEL
	BuildTheAsteroids(pGame, 1);
#else
	BuildTheAsteroids(pGame, pGame->nLevel * MAXASTEROIDS);
#endif
}

/*!****************************************************************************
* @brief	Terminates the game
* @param	pGame Pointer to the game engine
******************************************************************************/
void GameOver(TGame* pGame)
{
	pGame->bRun = true;
	pGame->bGameOver = true;

	for(int i=0; i<pGame->pShips.size(); ++i)
	{
		SetVisible(pGame->pShips[i], false);
		SetAlive(pGame->pShips[i], false);
	}

#ifndef _DEVEL
	PlayTheSound(pGame->pSM, "starwars-trails", true);
#endif
}

/*!****************************************************************************
* @brief	Advances the game to next level
* @param	pGame Pointer to the game engine
******************************************************************************/
void NextLevel(TGame* pGame)
{
	assert(pGame);
											// elimina i missili inesplosi
	Clear(pGame->pMissiles);

	pGame->nLevel++;

#ifdef _DEVEL
	unsigned nCount = pGame->nLevel;
#else
	unsigned nCount = pGame->nLevel * MAXASTEROIDS;
#endif

	BuildTheAsteroids(pGame, nCount);
}

/*!****************************************************************************
* @brief	Builds the ships
* @param	pGame Pointer to the game engine
* @return	Returns true for success, false otherwise
******************************************************************************/
bool BuildTheShips(TGame* pGame)
{
	assert(pGame);

	bool bResult = true;

											// build the human ship
	{
		TShip *pShip = new TShip;
		assert(pShip);

		Build(pShip,
			pGame->pVM,
			pGame->pSM,
			scHuman,
			TVector2 { SHIP_SIZE, SHIP_SIZE },
			TVector2 { FRAMEW/2, FRAMEH/2 },
			TVector2 { 0, 0 } );

		//SetClass(pShip, scHuman);

		SetRot(pShip, 180);
		SetAlive(pShip, true);
		SetVisible(pShip, true);
		SetColor(pShip, RGB(255,255,255));

		pGame->pShips.push_back(pShip);
	}
											// build the small alien ship
	{
		TShip *pShip = new TShip;
		assert(pShip);

		Build(pShip,
			pGame->pVM,
			pGame->pSM,
			scAlienSmall,
			//TVector2 { SHIP_SIZE/2.0, SHIP_SIZE/2.0 },
			//TVector2 { SHIP_SIZE * 3.0/4.0, SHIP_SIZE * 3.0/4.0 },
			TVector2 { SHIP_SIZE, SHIP_SIZE },
			TVector2 { -100, -100 },
			TVector2 { 0, 0 } );

		//SetClass(pShip, scAlienSmall);

		SetAlive(pShip, true);
		SetVisible(pShip, false);
		SetColor(pShip, RGB(255,255,255));

		pGame->pShips.push_back(pShip);
	}
											// build the big alien ship
	{
		TShip *pShip = new TShip;
		assert(pShip);

		Build(pShip,
			pGame->pVM,
			pGame->pSM,
			scAlienBig,
			//TVector2{1.25*SHIP_SIZE, 1.25*SHIP_SIZE},
			TVector2{1.5*SHIP_SIZE, 1.5*SHIP_SIZE},
			//TVector2{SHIP_SIZE, SHIP_SIZE},
			TVector2 { -100, -100 },
			TVector2 { 0, 0 } );

		//SetClass(pShip, scAlienBig);

		SetAlive(pShip, true);
		SetVisible(pShip, false);
		SetColor(pShip, RGB(255,255,255));

		pGame->pShips.push_back(pShip);
	}

	return bResult;
}

/*!****************************************************************************
* @brief	Checks status for pausing
* @param	pGame Pointer to the game engine
* @return	Returns true if game is in "pause" mode, false otherwise
******************************************************************************/
bool IsPausing(TGame* pGame)
{
	assert(pGame);

	return pGame->bPause;
}

/*!****************************************************************************
* @brief	Checks status for running
* @param	pGame Pointer to the game engine
* @return	Returns true if game is in "run" mode, false otherwise
******************************************************************************/
bool IsRunning(TGame* pGame)
{
	assert(pGame);

	return pGame->bRun;
}

/*!****************************************************************************
* @brief	Shots the missile
* @param	pGame Pointer to the game engine
* @param	pShip Pointer to the ship object
******************************************************************************/
void ShotTheMissile(TGame* pGame, TShip* pShip)
{
	assert(pGame);
	assert(pShip);
											// delay, in milliseconds,
											// between sequential shots
	unsigned nTickDelay = HUMANSHOTDELAY;				

	static unsigned nCurTick = nTickDelay;			

	if( (::GetTickCount() - nCurTick ) >= nTickDelay)
	{
		nCurTick = GetTickCount();

		PlayTheSound(pGame->pSM, "ship_fire");

		TMissile *pMissile = new TMissile;
		assert(pMissile);
		Build(pMissile, pGame->pVM);

		pMissile->pShip = pShip;

		pGame->pMissiles.push_back(pMissile);
													// nel caso dell'astronave "umana" spara
													// il missile lungo la direzione della prua
		if( GetClass(pShip) == scHuman )
		{
			double Rot = GetRot(pShip);
			double Mod = MISSILESPEED;
			TVector2 Vel{ Mod*cos((Rot-90)*M_PI/180.0f), Mod*sin((Rot+90)*M_PI/180.0f) };

			TVector2 Pos = GetPos(pShip);
			TVector2 ShipVel = GetVel(pShip);

			Arm(pMissile, Pos, Add( Vel, ShipVel) );
		}
	}
}

/*!****************************************************************************
* @brief	Inhibits game status for running
* @param	pGame Pointer to the game engine
******************************************************************************/
void EndTheGame(TGame* pGame)
{
	assert(pGame);

	pGame->bRun = false;
}

/*!****************************************************************************
* @brief	Cleanup resources
* @param	pGame Pointer to the game engine
******************************************************************************/
void Cleanup(TGame* pGame)
{
	assert(pGame);

	DeleteShips(pGame);
	DeleteAsteroids(pGame);
	FreeTheSounds(pGame->pSM);
}

/*!****************************************************************************
* @brief	Deletes the ships
* @param	pGame Pointer to the game engine
******************************************************************************/
void DeleteShips(TGame* pGame)
{
	assert(pGame);

	for(int i=0; i<pGame->pShips.size(); ++i)
	{
		assert(pGame->pShips[i]);
		delete pGame->pShips[i];
	}

	pGame->pShips.clear();
}

/*!****************************************************************************
* @brief	Pausing the game
* @param	pGame Pointer to the game engine
******************************************************************************/
void PauseTheGame(TGame* pGame)
{
	assert(pGame);

	pGame->bPause = !pGame->bPause;
}

/*!****************************************************************************
* @brief	Shows game information
* @param	pGame Pointer to the game engine
******************************************************************************/
void ShowInfo(TGame* pGame)
{
	assert(pGame);
	assert(pGame->pVM);

	unsigned nW, nH;
	GetClientSize(pGame, nW, nH);

	char Buffer[256];
	sprintf(Buffer, "Ships: %d", pGame->nLives < 0 ? 0 : pGame->nLives);
	DrawText(pGame->pVM, Buffer, 96, 16);

	sprintf(Buffer, "Level: %d", pGame->nLevel);
	DrawText(pGame->pVM, Buffer, nW/2.0, 16);

	sprintf(Buffer, "Score: %d", pGame->nScore);
	DrawText(pGame->pVM, Buffer, nW - 96, 16);
}

/*!****************************************************************************
* @brief	Deletes the asteroids
* @param	pGame Pointer to the game engine
******************************************************************************/
void DeleteAsteroids(TGame* pGame)
{
	assert(pGame);

												// cleaun-up the asteroid array
	for(unsigned i=0; i<pGame->pAsteroids.size(); i++)
	{
		if( pGame->pAsteroids[i] ) delete pGame->pAsteroids[i];
	}

	pGame->pAsteroids.clear();
}

/*!****************************************************************************
* @brief	Builds the asteroids
* @param	pGame Pointer to the game engine
* @param	nCount Number of asteroids to be created
******************************************************************************/
void BuildTheAsteroids(TGame* pGame, unsigned nCount)
{
	assert(pGame);
	assert(pGame->pVM);

												// firstly, clear the list
	DeleteAsteroids(pGame);
												// rebuild the asteroid's list
	for(unsigned int i=0; i<nCount; i++)
	{
		TVector2 Pos{ AbsRand(pGame->pVM->ClientArea.right), AbsRand(pGame->pVM->ClientArea.bottom) };
		TVector2 Vel { Rand(ASTEROIDVEL) + ASTEROIDVEL/5.0, Rand(ASTEROIDVEL) + ASTEROIDVEL/5.0 };

		TAsteroid *pAsteroid = new TAsteroid;
		assert(pAsteroid);

		Build(pAsteroid, pGame->pVM, acBig, Pos, Vel, ASTEROIDBIGSIZE + AbsRand(ASTEROIDBIGSIZE/10.0) );

		assert(pAsteroid);
		pGame->pAsteroids.push_back(pAsteroid);
	}
}

/*!****************************************************************************
* @brief	Checks if given position are out of the scenario boundaries
* @param	pGame Pointer to the game engine
* @param	Pos Position to be tested to for coordinates
* @return	Returns true if Pos is inside limits, false otherwise
******************************************************************************/
bool IsInsideGameArea(TGame* pGame, TVector2 Pos)
{
	assert(pGame);

	unsigned nWidth, nHeight;
	GetClientSize(pGame, nWidth, nHeight);

	return bool( (Pos.X>=0) && (Pos.X <= nWidth) && (Pos.Y >=0 ) && (Pos.Y <= nHeight) );
}

/*!****************************************************************************
* @brief	Force all actors inside of the scenario boundaries
* @param	pGame Pointer to the game engine
******************************************************************************/
void ForceInsideLimits(TGame* pGame)
{
	assert(pGame);
	assert(pGame->pVM);
											// force ships inside the scenery limits
	unsigned nWidth, nHeight;
	GetClientSize(pGame, nWidth, nHeight);

	for(int i=0; i<pGame->pShips.size(); ++i)
	{
		TVector2 Pos = GetPos(pGame->pShips[i]);

		if( !IsInsideGameArea(pGame, Pos) )
		{
			if( GetClass(pGame->pShips[i]) == scHuman )
			{

				if( Pos.X < 0 ) Pos.X = nWidth;
				if( Pos.X > nWidth ) Pos.X = 0;
				if( Pos.Y < 0 ) Pos.Y = nHeight;
				if( Pos.Y > nHeight ) Pos.Y = 0;

				SetPos(pGame->pShips[i], Pos);
			}
			else
			{
				if( Pos.X >=0 && Pos.X <= nWidth )
				{
					if( Pos.Y < 0 ) Pos.Y = nHeight;
					if( Pos.Y > nHeight ) Pos.Y = 0;

					SetPos(pGame->pShips[i], Pos);
				}
				else
				{
					SetVisible(pGame->pShips[i], false);
					SetPos(pGame->pShips[i], TVector2 { -100, -100 } );
				}
			}
		}
	}
											// force asteroids inside the scenery limits
	for(int i=0; i<pGame->pAsteroids.size(); ++i)
	{
		if( pGame->pAsteroids[i] && IsAlive(pGame->pAsteroids[i]) )
		{
			TVector2 Pos = GetPos(pGame->pAsteroids[i]);

			if( Pos.X < 0 ) Pos.X = nWidth;
			if( Pos.X > nWidth ) Pos.X = 0;
			if( Pos.Y < 0 ) Pos.Y = nHeight;
			if( Pos.Y > nHeight ) Pos.Y = 0;

			SetPos(pGame->pAsteroids[i], Pos);
		}
	}
}

/*!****************************************************************************
* @brief	Handles the transition to next game level
* @param	pGame Pointer to the game engine
******************************************************************************/
void LevelHandler(TGame* pGame)
{
	assert(pGame);

	bool bLevelCompleted = true;

	for( int i=0; i<pGame->pAsteroids.size(); i++)
	{
		if( pGame->pAsteroids[i] )
		{
			if( IsAlive(pGame->pAsteroids[i]) )
			{
				bLevelCompleted = false;
			}
		}
	}
											// se non ci sono piu` asteroidi ...
	if( bLevelCompleted )
	{
		pGame->pAsteroids.clear();

		NextLevel(pGame);
	}
}

/*!****************************************************************************
* @brief	Handles the "bonus" event
* @param	pGame Pointer to the game engine
******************************************************************************/
void BonusHandler(TGame* pGame)
{
	assert(pGame);

	if( pGame->nScore >= (BONUSPOINTS * pGame->nBonusCount) )
	{
		pGame->nLives++;
		pGame->nBonusCount++;

		PlayTheSound(pGame->pSM, "bonus");
	}
}

/*!****************************************************************************
* @brief	Collision detection between ships and asteroids
* @param	pAsteroid Pointer to the asteroid object
* @param	pShip Pointer to the ship object
* @return	Returns true if objects collides, false otherwise
******************************************************************************/
bool Collide(TAsteroid* pAsteroid, TShip* pShip)
{
//	return bool( Distance(pShip->Pos, pAsteroid->Pos) <= pAsteroid->Radius );
	return bool( Distance(pShip->Pos, pAsteroid->Pos) <= ( pAsteroid->Radius + pShip->Size.X / 2.0));
}

/*!****************************************************************************
* @brief	Previene di piazzare "a tradimento" l'astronave
*			ossia nel bel mezzo di una pioggia di meteoriti !
* @param	pGame Pointer to the game engine
* @param	Pos Position to want to check
* @return	Returns true if area around specified position is
			free from meteorites, false otherwise
******************************************************************************/
bool IsSafetyPos(TGame* pGame, TVector2 Pos)
{
	assert(pGame);

	bool bResult = true;

	for(int i=0; i<pGame->pAsteroids.size(); ++i)
	{
		if( pGame->pAsteroids[i] )
		{
			if( Distance( GetPos(pGame->pAsteroids[i]), Pos) <= SAFETYDISTANCE)
			{
				bResult = false;
				break;
			}
		}
	}

	return bResult;
}

/*!****************************************************************************
* @brief	Handles the "game-over" status
* @param	pGame Pointer to the game engine
******************************************************************************/
void GameOverHandler(TGame* pGame)
{
	assert(pGame);
	assert(pGame->pVM);

	static int nCounter = 0;
	unsigned nTickDelay = SPLASHDELAY;				

	static unsigned nCurTick = nTickDelay;			
	
	TVector2 ScreenCenter = GetScreenCenter(pGame->pVM);

	std::vector<std::string> strBestScores;
	int nItems = std::min(int(BESTSCORES), int(pGame->BestScores.size()));

	strBestScores.push_back("Best Scores:");
	strBestScores.push_back(" ");
	strBestScores.push_back(" ");

	for(int i=0; i<nItems; i++)
	{
		char Buffer[256];

		sprintf(Buffer, "%s   %d",
			(char*)pGame->BestScores[i].strName.c_str(),
			pGame->BestScores[i].nScore
		);

		strBestScores.push_back(Buffer);
	}

	if( (::GetTickCount() - nCurTick ) >= nTickDelay)
	{
		nCurTick = GetTickCount();

		nCounter++;
		if( nCounter > 2 ) nCounter = 0;
	}

	switch( nCounter )
	{
		case 0:
			DrawText(pGame->pVM, (char*)"Game Over", ScreenCenter.X, ScreenCenter.Y);
		break;

		case 1:
			DrawText(pGame->pVM, pGame->strHelp, ScreenCenter.X, 128, FONTSIZE);
		break;

		case 2:
			DrawText(pGame->pVM, strBestScores, ScreenCenter.X, 128, FONTSIZE);
		break;
	}
}

/*!****************************************************************************
* @brief	Handles collisions between all objects of the scenario
* @param	pGame Pointer to the game engine
******************************************************************************/
void CollisionHandler(TGame* pGame)
{
	assert(pGame);
											// check for collisions between ...

											// ... ships and asteroids
	for(int i=0; i<pGame->pAsteroids.size(); ++i)
	{
		for(int j=0; j<pGame->pShips.size(); ++j)
		{
			if( pGame->pAsteroids[i] && IsAlive(pGame->pShips[j]) )
			{
				if( Collide(pGame->pAsteroids[i], pGame->pShips[j]) )
				{
					delete pGame->pAsteroids[i];
					pGame->pAsteroids[i] = nullptr;

					Explode(pGame->pShips[j]);

					if( GetClass(pGame->pShips[j]) == scHuman )
					{
						pGame->nLives--;

						if( pGame->nLives == 0 )
						{
							GameOver(pGame);

							if( IsBestScore(pGame) )
							{
								RegisterBestScore(pGame);
								//pGame->bBestScore = true;
							}
						}
					}
				}
			}
		}
	}
											// ... missiles and ships

	for(int i=0; i<pGame->pMissiles.size(); ++i)
	{
		TMissile *pMissile = pGame->pMissiles[i];

		if( pMissile && IsArmed(pMissile) )
		{
			for(int j=0; j<pGame->pShips.size(); ++j)
			{
				TShip* pShip = pGame->pShips[j];
											
				if( IsAlive(pShip)
					&& pMissile
											// avoids that the missile destroy
											// the ship itself that has shooted it
					&& pMissile->pShip != pShip
				)
				{
					if( IsColliding(pShip, GetPos(pMissile)) && !IsShieldActive(pShip) )
					{
						Explode(pShip);

						delete pMissile;
						pMissile = nullptr;
						pGame->pMissiles[i] = nullptr;

						if( GetClass(pShip) == scHuman )
						{
							pGame->nLives--;

							if( pGame->nLives == 0 )
							{
								GameOver(pGame);
							}
						}
						else if( GetClass(pShip) == scAlienBig )
						{
							pGame->nScore += BIGALIENSHIPSCORE;
						}
						else if( GetClass(pShip) == scAlienSmall )
						{
							pGame->nScore += SMALLALIENSHIPSCORE;
						}
					}
				}
			}
		}
	}

											// ... missiles and asteroids
	for(int i=0; i<pGame->pMissiles.size(); i++)
	{
		TMissile* pMissile = pGame->pMissiles[i];

		if( pMissile && IsArmed(pMissile) )
		{
			for(int j=0; j<pGame->pAsteroids.size(); ++j)
			{
				TAsteroid *pRoid = pGame->pAsteroids[j];

				if( pRoid && pMissile && Collide(pRoid, GetPos(pMissile) ) )
				{
					if( GetClass(pRoid) == acBig )
					{
						pGame->nScore += BIGASTEROIDSCORE;
						PlayTheSound(pGame->pSM, "bang_large");

						TVector2 Pos, Vel;
						Pos = GetPos(pRoid);
						Vel = GetVel(pRoid);

						TVector2 RndVel1 {  Rand(Vel.X)/ASTEROIDVELRATIO, Rand(Vel.Y)/ASTEROIDVELRATIO };

						TVector2 Vel1 = Add(Vel, RndVel1);

						TAsteroid *pAsteroid = new TAsteroid;
						assert(pAsteroid);
						Build(pAsteroid, pGame->pVM, acMedium, Pos, Add(Vel, Vel1), ASTEROIDMIDSIZE + AbsRand(ASTEROIDMIDSIZE/4.0));

						pAsteroid->bAlive = true;

						pGame->pAsteroids.push_back(pAsteroid);


						TVector2 RndVel2 { Rand(Vel.X)/ASTEROIDVELRATIO, Rand(Vel.Y)/ASTEROIDVELRATIO };

						TVector2 Vel2 = Add(Vel, RndVel2);

						pAsteroid = new TAsteroid;
							
						Build(pAsteroid, pGame->pVM, acMedium, Pos, Add(Vel, Vel2), ASTEROIDMIDSIZE + AbsRand(ASTEROIDMIDSIZE/4.0));
						assert(pAsteroid);
							
						pAsteroid->bAlive = true;

						pGame->pAsteroids.push_back(pAsteroid);
					}
					else if( GetClass(pRoid) == acMedium )
					{
						pGame->nScore += MIDASTEROIDSCORE;
						PlayTheSound(pGame->pSM, "bang_medium");

						TVector2 Pos, Vel;
						Pos = GetPos(pRoid);
						Vel = GetVel(pRoid);

						TVector2 RndVel1 {  Rand(Vel.X)/ASTEROIDVELRATIO, Rand(Vel.Y)/ASTEROIDVELRATIO };
							
						TVector2 Vel1 = Add(Vel, RndVel1);

						TAsteroid *pAsteroid = new TAsteroid;
						assert(pAsteroid);

						Build(pAsteroid, pGame->pVM, acSmall, Pos, Add(Vel, Vel1), ASTEROIDSMALLSIZE + AbsRand(ASTEROIDSMALLSIZE/2.0) );

						pAsteroid->bAlive = true;

						pGame->pAsteroids.push_back(pAsteroid);

						TVector2 RndVel2 { Rand(Vel.X)/ASTEROIDVELRATIO, Rand(Vel.Y)/ASTEROIDVELRATIO };

						TVector2 Vel2 = Add(Vel, RndVel2);

						pAsteroid = new TAsteroid;
						assert(pAsteroid);

						Build(pAsteroid, pGame->pVM, acSmall, Pos, Add(Vel, Vel2), ASTEROIDSMALLSIZE + AbsRand(ASTEROIDSMALLSIZE/2.0) );

						pAsteroid->bAlive = true;

						pGame->pAsteroids.push_back(pAsteroid);
					}
					else
					{
						PlayTheSound(pGame->pSM, "bang_small");

						pGame->nScore += SMALLASTEROIDSCORE;
					}

					delete pGame->pAsteroids[j];
					pGame->pAsteroids[j] = nullptr;

					delete pMissile;
					pGame->pMissiles[i] = pMissile = nullptr;
				}
			}
		}
	}

											// deletes the missiles that have gone out of range (screen area)
	for(int i=0; i<pGame->pMissiles.size(); i++)
	{
		TMissile *pMissile = pGame->pMissiles[i];

		if( pMissile && !IsInsideGameArea(pGame, GetPos(pMissile)) )
		{
			delete pGame->pMissiles[i];
			pGame->pMissiles[i] = nullptr;
		}
	}
}

/*!****************************************************************************
* @brief	Checks for "game-over" status
* @param	pGame Pointer to the game engine
* @return	If the game is in "game-over" status return true, false otherwise
******************************************************************************/
bool IsGameOver(TGame* pGame)
{
	assert(pGame);

	return pGame->bGameOver;
}

/*!****************************************************************************
* @brief	Run the game
* @param	pGame Pointer to the game engine
******************************************************************************/
void Run(TGame* pGame)
{
	assert(pGame);
	assert(pGame->pVM);
	assert(pGame->pShips[scHuman]);
	assert(pGame->pShips[scAlienBig]);
	assert(pGame->pShips[scAlienSmall]);


											// update the ships
	for(int i=0; i<pGame->pShips.size(); ++i)
	{
		Update(pGame->pShips[i], DT);
	}
											// if alien ships are active (visibles)
											// then make many shoots against humans
	{
		static int nTickCount = 0;
		nTickCount++;

		if( nTickCount >= ALIENSHOTDELAY)
		{
			nTickCount = 0;

			if( IsVisible(pGame->pShips[scAlienBig]) )
			{
				TMissile *pMissile = new TMissile;
				assert(pMissile);
				Build(pMissile, pGame->pVM);

				pMissile->pShip = pGame->pShips[scAlienBig];

				pGame->pMissiles.push_back(pMissile);

				{
					TVector2 AlienPos = GetPos(pGame->pShips[scAlienBig]);
					TVector2 HumanPos = GetPos(pGame->pShips[scHuman]);

					double Mod = MISSILESPEED;

					double DX = HumanPos.X - AlienPos.X;
					double DY = HumanPos.Y - AlienPos.Y;

					//double Rot = atan2(DY, DX);	// too precise !
					double Rot = atan2(DY, DX) + ALIENBIGINACCURACY + Rand(ALIENBIGINACCURACY);

					TVector2 Vel{ Mod*cos(Rot), Mod*sin(Rot) };

					//TVector2 ShipVel = GetVel(pGame->pShips[scAlienBig]);

					Arm(pMissile, AlienPos, Vel );
				}
			}
			
			if( IsVisible(pGame->pShips[scAlienSmall]) )
			{
				TMissile *pMissile = new TMissile;
				assert(pMissile);
				Build(pMissile, pGame->pVM);

				pMissile->pShip = pGame->pShips[scAlienSmall];

				pGame->pMissiles.push_back(pMissile);

				{
					TVector2 AlienPos = GetPos(pGame->pShips[scAlienSmall]);
					TVector2 HumanPos = GetPos(pGame->pShips[scHuman]);

					double Mod = MISSILESPEED;

					double DX = HumanPos.X - AlienPos.X;
					double DY = HumanPos.Y - AlienPos.Y;

					//double Rot = atan2(DY, DX);	// too precise !
					double Rot = atan2(DY, DX) + ALIENSMALLINACCURACY + Rand(ALIENSMALLINACCURACY);

					TVector2 Vel{ Mod*cos(Rot), Mod*sin(Rot) };

					//TVector2 ShipVel = GetVel(pGame->pShips[scAlienSmall]);

					Arm(pMissile, AlienPos, Vel );
				}
			}
		}
	}
											// update the missiles
	for(int i=0; i<pGame->pMissiles.size(); i++)
	{
		TMissile* pMissile = pGame->pMissiles[i];

		if ( pMissile && IsArmed(pMissile) )
		{
			Update(pMissile, DT);
		}
	}
											// update the asteroids
	unsigned nWidth, nHeight;
	GetClientSize(pGame, nWidth, nHeight);

	for(int i=0; i<pGame->pAsteroids.size(); ++i)
	{
		TAsteroid *pAsteroid = pGame->pAsteroids[i];

		if( pAsteroid && IsAlive(pAsteroid) )
		{
			Update(pAsteroid, DT);

			TVector2 Pos = GetPos(pAsteroid);

			if( Pos.X < 0 ) Pos.X = nWidth;
			if( Pos.X > nWidth ) Pos.X = 0;
			if( Pos.Y < 0 ) Pos.Y = nHeight;
			if( Pos.Y > nHeight ) Pos.Y = 0;

			SetPos(pAsteroid, Pos);
		}
	}
											// forces actors inside of scenery limits
	ForceInsideLimits(pGame);

	if( !IsGameOver(pGame) )
	{
		TVector2 ScreenCenter = GetScreenCenter(pGame->pVM);

		if ( IsSafetyPos(pGame, ScreenCenter)
			&& !IsAlive(pGame->pShips[scHuman])
			&& !IsExploding(pGame->pShips[scHuman]) )
		{
			Reset(pGame->pShips[scHuman]);
			SetPos(pGame->pShips[scHuman], ScreenCenter );
			SetRot(pGame->pShips[scHuman], 180.0);
			SetVel(pGame->pShips[scHuman], TVector2{0,0} );
			SetAlive(pGame->pShips[scHuman], true);
			SetVisible(pGame->pShips[scHuman], true);
		}

		AlienShipsHandler(pGame);

		CollisionHandler(pGame);

		BonusHandler(pGame);

		LevelHandler(pGame);
	}
	else
	{
#ifndef _DEVEL
		GameOverHandler(pGame);
#endif
	}
											// show info (help, ships, score, etc...)
	ShowInfo(pGame);
}

/*!****************************************************************************
* @brief	Handles the aliens ships
* @param	pGame Pointer to the game engine
******************************************************************************/
void AlienShipsHandler(TGame* pGame)
{
	assert(pGame);

	static int nAlienShipTick = ALIENSHIPTICK + Rand(ALIENSHIPTICK/2);

	nAlienShipTick--;

	TShip* pShip = RandSign() >= 0 ? pGame->pShips[scAlienBig] : pGame->pShips[scAlienSmall];
	assert(pShip);

	if( nAlienShipTick == 0 )
	{
		nAlienShipTick = ALIENSHIPTICK + Rand(ALIENSHIPTICK/2);

		if( !IsVisible(pShip) )
		{
			TVector2 ScreenCenter = GetScreenCenter(pGame->pVM);

			SetPos(pShip, TVector2 { 0, ScreenCenter.Y + Rand(double(ScreenCenter.Y - 50)) } );

			SetVel(pShip, TVector2 { 25 + AbsRand(25), 0 } );
			SetAlive(pShip, true);
			SetVisible(pShip, true);
		}
	}
}

/*!****************************************************************************
* @brief	Return the handle to the main game window
* @param	pGame Pointer to the game engine
* @return	Window handle
******************************************************************************/
HWND GetMainWnd(TGame* pGame)
{
	assert(pGame);
	assert(pGame->pVM);

	return pGame->pVM->hWnd;
}

/*!****************************************************************************
* @brief	Checks for best score
* @param	pGame Pointer to the game engine
* @return	Returns true if the game score is in the best scores
******************************************************************************/
bool IsBestScore(TGame* pGame)
{
	assert(pGame);
	
	bool bResult = false;

	int nCount = std::min(int(BESTSCORES), int(pGame->BestScores.size()));

											// linear search
	for(int i=0; i<nCount; i++)
	{
		if( pGame->nScore > pGame->BestScores[i].nScore )
		{
			bResult = true;
			break;
		}
	}

	return bResult;
}

/*!****************************************************************************
* @brief	Shows a dialog-box to gets best score data
* @param	pGame Pointer to the game engine
******************************************************************************/
void RegisterBestScore(TGame* pGame)
{
	assert(pGame);

	pGame->bBestScoreDlgActive = true;
		::DialogBox(NULL, MAKEINTRESOURCE(IDD_RECORDS), GetMainWnd(pGame), (DLGPROC) BestScoresDlgProc);
	pGame->bBestScoreDlgActive = false;

	if( pGame->nDlgRetVal == IDOK )
	{
		SaveBestScores(pGame);
	}
}

/*!****************************************************************************
* @brief	Checks if is active the "Best Scores" input dialog
* @param	pGame Pointer to the game engine
* @return	True if the input dialog is active, false otherwise
******************************************************************************/
bool IsInputDialog(TGame* pGame)
{
	assert(pGame);

	return pGame->bBestScoreDlgActive;
}

/*!****************************************************************************
* @brief	Save best scores to file
* @param	pGame Pointer to the game engine
******************************************************************************/
void SaveBestScores(TGame* pGame)
{
	assert(pGame);

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::string strName = converter.to_bytes(pGame->pBestScoresName);

	TRecordScores BestScore;
	BestScore.strName = strName;
	BestScore.nScore = pGame->nScore;

	pGame->BestScores.push_back(BestScore);

											// false -> sort from bigger to lower
	Sort(pGame->BestScores, false);	

											// save (append) the score to file
	std::string strScoresFile = std::string(GetDataPath()) + std::string(SCORESFILE);

											// "a" -> open in "append" mode
	FILE *fp = fopen(strScoresFile.c_str(), "a");

	if( fp )
	{
		fprintf(fp, "%s,%d\n", BestScore.strName.c_str(), BestScore.nScore);

		fclose(fp);
	}
}

/*!****************************************************************************
* @brief	Reorder best scores
* @param	Scores	Reference to a vector of best scores data structures
* @param	bAscending Sets the reordering mode, true for ascending order
******************************************************************************/
void Sort(TVecRecordScores& Scores, bool bAscending)
{

	for(int i=0; i<Scores.size(); i++)
	{
		for(int j=i+1; j<Scores.size(); j++)
		{
			if( bAscending )
			{
				if( Scores[i].nScore> Scores[j].nScore)
				{
#ifdef _MSC_VER
					std::swap(Scores[i], Scores[j]);
#else
					Swap(Scores[i], Scores[j]);
#endif
				}
			}
			else
			{
				if( Scores[i].nScore< Scores[j].nScore)
				{
#ifdef _MSC_VER
					std::swap(Scores[i], Scores[j]);
#else
					Swap(Scores[i], Scores[j]);
#endif
				}
			}
		}
	}

}

#ifndef _MSC_VER
	void Swap(TRecordScores& T1, TRecordScores& T2)
	{
		TRecordScores Temp = T1;

		T1 = T2;
		T2 = Temp;
	}
#endif

/*!****************************************************************************
* @brief	Gets the size of the client area
* @param	pGame Pointer to the game engine
* @param[in,out] nW Width of clienr area
* @param[in,out] nH Height of clienr area
******************************************************************************/
void GetClientSize(TGame* pGame, unsigned& nW, unsigned& nH)
{
	assert(pGame);
	assert(pGame->pVM);

	nW = pGame->pVM->ClientArea.right;
	nH = pGame->pVM->ClientArea.bottom;
}

