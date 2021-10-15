/*!****************************************************************************

	@file	audio.h
	@file	audio.cpp

	@brief	Audio manager

	@noop	author:	Francesco Settembrini
	@noop	last update: 23/6/2021
	@noop	e-mail:	mailto:francesco.settembrini@poliba.it

******************************************************************************/

#include <windows.h>
#include <assert.h>

#include <string>
#include <fstream>
#include <sstream>

#include <al/al.h>
#include <al/alc.h>

//#include <sdl2/sdl.h>
//#include <sdl2/sdl_audio.h>

#include "audio.h"
#include "utils.h"


//-----------------------------------------------------------------------------
#define DELTAVOLUME	0.05


/*!****************************************************************************
* @brief	Initialize the sound manager
* @param	pALSystem Pointer to TALSystem data structure
* @return	Returns true for success, false otherwise
******************************************************************************/
bool SetupSoundManager(TALSystem *pALSystem)
{
	assert(pALSystem);

	bool bResult = true;
											// Get an OpenAL device
	pALSystem->pAlcDevice = alcOpenDevice(NULL);

	if (! (pALSystem->pAlcDevice == nullptr) )
	{
											// Create a context
		pALSystem->pAlcContext = alcCreateContext(pALSystem->pAlcDevice,NULL);
 
											// Set active context
		alcMakeContextCurrent(pALSystem->pAlcContext);
 
											// Clear Error Code
		alGetError();
	}
	else
	{
											// Handle Exception
	   ::MessageBox(0, TEXT("Error intializing OpenAL!"), TEXT("Error"), MB_OK | MB_ICONERROR);
	 
		bResult = false;
	}

	return bResult;
}

/*!****************************************************************************
* @brief	Cleanup the sound manager
* @param	pSM Pointer to the sound manager
******************************************************************************/
void CleanupSoundManager(TSoundManager* pSM)
{
	assert(pSM);
	assert(pSM->pALSystem);
	assert(pSM->pALSystem->pAlcContext);

	alcDestroyContext(pSM->pALSystem->pAlcContext);
	alcCloseDevice(pSM->pALSystem->pAlcDevice);
}

/*!****************************************************************************
* @brief	Set master volume
* @param	pSM Pointer to the sound manager
* @param	Volume Value for volume, between 0..1
******************************************************************************/
void SetMasterVolume(TSoundManager* pSM, double Volume)
{
	assert(pSM);

	if( Volume > 1.0 ) Volume = 1.0;
	if( Volume < 0 ) Volume = 0;

	alListenerf(AL_GAIN, Volume);
}

/*!****************************************************************************
* @brief	Get master volume
* @param	pSM Pointer to the sound manager
* @return	Value for the master volume, between 0..1
******************************************************************************/
double GetMasterVolume(TSoundManager* pSM)
{
	assert(pSM);

	ALfloat Volume = 0;

	alGetListenerf(AL_GAIN, &Volume);

	return double(Volume);
}

/*!****************************************************************************
* @brief	Increase master volume by a predefined step
* @param	pSM Pointer to the sound manager
******************************************************************************/
void IncreaseMasterVolume(TSoundManager* pSM)
{
	assert(pSM);

	double Volume = GetMasterVolume(pSM);

	Volume += DELTAVOLUME;

	if(Volume > 1.0) Volume = 1.0;
		
	SetMasterVolume(pSM, Volume);
}

/*!****************************************************************************
* @brief	Derease master volume by a predefined step
* @param	pSM Pointer to the sound manager
******************************************************************************/
void DecreaseMasterVolume(TSoundManager* pSM)
{
	assert(pSM);

	double Volume = GetMasterVolume(pSM);

	Volume -= DELTAVOLUME;

	if( Volume < 0 ) Volume = 0;

	SetMasterVolume(pSM, Volume);
}

/*!****************************************************************************
* @brief	Load a sound from file
* @param	pSM Pointer to the sound manager
* @param	strFileName Path to an audio file in WAV PCM format
* @return	Returns true for success, false otherwise
******************************************************************************/
bool LoadTheSound(TSoundManager* pSM, std::string strFileName)
{
	assert(pSM);

	bool bResult = false;

	int nChannels=0, nSampleRate=0, nBps=0, nSize = 0;
	char *pData = LoadWAV(strFileName.c_str(), nChannels, nSampleRate, nBps, nSize); 

/*
printf("\n\nFile Name: %s", strFileName.c_str());
printf("\nchannels: %d", nChannels);
printf("\nSampleRate: %d", nSampleRate);
printf("\nBps: %d", nBps);
printf("\nSamples per channel: %d", nSize);
*/

	if( pData )
	{
		ALuint nBufferId, nFormat;
		alGenBuffers(1, &nBufferId);

		if( nChannels == 1 )
		{
			if( nBps == 8 )
			{
				nFormat = AL_FORMAT_MONO8;
			}
			else
			{
				nFormat = AL_FORMAT_MONO16;
			}
		}
		else
		{
			if( nBps == 8 )
			{
				nFormat = AL_FORMAT_STEREO8;
			}
			else
			{
				nFormat = AL_FORMAT_STEREO16;
			}
		}

		alBufferData(nBufferId, nFormat, pData, nSize, nSampleRate);

		ALuint nSourceId;
		alGenSources(1, &nSourceId);

		alSourcei(nSourceId, AL_BUFFER, nBufferId);

		std::string strName = GetFileName(strFileName, true);

		TSoundTrack SoundTrack { strName, nBufferId, nSourceId };

		pSM->SoundTracks.insert(make_pair(strName, SoundTrack));

		bResult = true;
	}

	return bResult;
}

/*!****************************************************************************
* @brief	Load sounds from a list of specified files
* @param	pSM Pointer to the sound manager
* @param	strSounds A list of sound filenames to be open
* @return	Returns true for success, false otherwise
******************************************************************************/
bool LoadTheSounds(TSoundManager* pSM, std::vector<std::string> strSounds)
{
	assert(pSM);
	assert(strSounds.size());

	bool bResult = true;

	std::string strDataPath = GetDataPath();

	for(int i=0; i<strSounds.size(); ++i)
	{
		std::string strWavFile = strDataPath + strSounds[i] + ".wav";

		if( !LoadTheSound(pSM, strWavFile) )
		{
			bResult = false;
			break;
		}
	}

	return bResult;
}

/*!****************************************************************************
* @brief	Delete sound buffers and related resources
* @param	pSM Pointer to the sound manager
******************************************************************************/
void FreeTheSounds(TSoundManager* pSM)
{
	assert(pSM);

	for(auto Iter=pSM->SoundTracks.begin();Iter!=pSM->SoundTracks.end();++Iter)
	{
		alDeleteSources(1, &Iter->second.nSourceId);
	}
}

/*!****************************************************************************
* @brief	Play and audio track
* @param	pSM Pointer to the sound manager
* @param	bLoop Flag for looping: true if must be played repeatedly
******************************************************************************/
void PlayTheSound(TSoundManager* pSM, std::string strSound, bool bLoop)
{
	assert(pSM);

	if( pSM->SoundTracks.size() )
	{
		auto it = pSM->SoundTracks.find(strSound);

		//if( !it._Ptr->_Isnil )
		if( it._M_node != nullptr)
		{
			TSoundTrack ST = it->second;

			alSourcei(ST.nSourceId, AL_LOOPING, bLoop);
			alSourcePlay(ST.nSourceId);
		}
	}
}

/*!****************************************************************************
* @brief	Stops to playing a sound source
* @param	pSM Pointer to the sound manager
* @param	strSound Sound source to be stopped
******************************************************************************/
void StopTheSound(TSoundManager* pSM, std::string strSound)
{
	assert(pSM);

	if( pSM->SoundTracks.size() )
	{
		auto it = pSM->SoundTracks.find(strSound);

		//if( !it._Ptr->_Isnil )
		if( it._M_node != nullptr)
		{
			alSourceStop(it->second.nSourceId);
		}
	}
}

/*!****************************************************************************
* @brief	Stops to playing all sound sources
* @param	pSM Pointer to the sound manager
******************************************************************************/
void StopAllSounds(TSoundManager* pSM)
{
	assert(pSM);

	for( auto it = pSM->SoundTracks.begin(); it != pSM->SoundTracks.end(); ++it )
	{
		alSourceStop(it->second.nSourceId);
	}
}

/*!****************************************************************************
* @brief	Load raw data from an audio file in WAV PCM format
* @param	strFileName The path to the file name to be open
* @param[in,out] nChannels The number of channels: 1 for mono, 2 for stereo
* @param[in,out] nSampleRate The Sample rate for audio data
* @param[in,out] nBps The bits per sample
* @param[in,out] nSize The size of raw audio buffer
* @return	The pointer to the raw audio buffer data
******************************************************************************/
char* LoadWAV(std::string strFileName,
	int& nChannels, int& nSampleRate, int& nBps, int& nSize)
{
    char buffer[4];

    std::ifstream in(strFileName.c_str(), std::ios::binary);

    in.read(buffer, 4);

    if (strncmp(buffer, "RIFF", 4) != 0)
    {
        std::cout << "this is not a valid WAV file" << std::endl;
        return NULL;
    }

    in.read(buffer, 4);
    in.read(buffer, 4);      // WAVE
    in.read(buffer, 4);      // fmt
    in.read(buffer, 4);      // 16
    in.read(buffer, 2);      // 1

    in.read(buffer, 2);
    nChannels = ConvertToInt(buffer, 2);

    in.read(buffer, 4);
    nSampleRate = ConvertToInt(buffer, 4);

    in.read(buffer, 4);
    in.read(buffer, 2);

    in.read(buffer, 2);
    nBps = ConvertToInt(buffer, 2);

    in.read(buffer, 4);      // data
    in.read(buffer, 4);
    nSize = ConvertToInt(buffer, 4);

    char* pData = new char[nSize];
	assert(pData);
    in.read(pData, nSize);

    return pData;
}

