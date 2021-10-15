#ifndef _SOUNDMANAGER_H_
#define _SOUNDMANAGER_H_

#include <al/al.h>
#include <al/alc.h>

#include <string>
#include <vector>
#include <map>


struct TALSystem
{
	ALCcontext *pAlcContext;
	ALCdevice *pAlcDevice;
};

struct TSoundTrack
{
	std::string strName;
	ALuint nBufferId, nSourceId;
};

typedef std::vector<TSoundTrack> TSoundTracks;
typedef std::map< std::string, TSoundTrack > TMapSoundTracks;

struct TSoundManager
{
	TALSystem *pALSystem;
	TMapSoundTracks SoundTracks;
};

bool SetupSoundManager(TALSystem *pALSystem);
void CleanupSoundManager(TSoundManager* pSM);

double GetMasterVolume(TSoundManager* pSM);
void SetMasterVolume(TSoundManager* pSM, double Volume);

void IncreaseMasterVolume(TSoundManager* pSM);
void DecreaseMasterVolume(TSoundManager* pSM);

bool LoadTheSounds(TSoundManager* pSM, std::vector<std::string> strSounds);
void FreeTheSounds(TSoundManager* pSM);
bool LoadTheSound(TSoundManager* pSM, std::string strFileName);
void PlayTheSound(TSoundManager* pSM, std::string strSound, bool bLoop = false);
void StopTheSound(TSoundManager* pSM, std::string strSound);
void StopAllSounds(TSoundManager* pSM);

char* LoadWAV(std::string strFileName, int& nChannels, int& nSampleRate, int& nBps, int& nSize);

#endif

