/*!****************************************************************************

	@file	utils.h
	@file	utils.cpp

	@brief	Utility routines

	@noop	author:	Francesco Settembrini
	@noop	last update: 23/6/2021
	@noop	e-mail:	mailto:francesco.settembrini@poliba.it

******************************************************************************/

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <locale>
#include <codecvt>
#include <string>

#include "commdefs.h"
#include "utils.h"


//-----------------------------------------------------------------------------

#define DEBOUNCEDELAY		100



/*!****************************************************************************
* @brief	Insert a delay to prevent "bouncing" effect when repeatedly
*			pressing keys on keyboard	
******************************************************************************/
void Debounce()
{
	::Sleep(DEBOUNCEDELAY);
}

/*!****************************************************************************
* @brief	Gets the full path for the executable file
* @return	Returns the full path to the exe file name
******************************************************************************/
std::string GetExePath()
{
	std::string strResult;
	char ownPath[MAX_PATH];

											// When NULL is passed to GetModuleHandle,
											// the handle of the exe itself is returned
    HMODULE hModule = GetModuleHandle(NULL);
    if (hModule != NULL)
    {
											// Use GetModuleFileName() with module handle to get the path
		//GetModuleFileName(hModule, ownPath, (sizeof(ownPath))); 
		GetModuleFileNameA(hModule, ownPath, (sizeof(ownPath))); 

		{
			const int BUFSIZE = 260;

			DWORD  retval=0;
			//BOOL   success;

			char buffer[BUFSIZE];
			//char buf[BUFSIZE];
			char* lppPart;
												// Retrieve the full path name for a file. 
												// The file does not need to exist.

			retval = GetFullPathNameA( ownPath, BUFSIZE, (LPSTR) buffer, &lppPart);

			if( retval )
			{
				int nPos = strstr((char*)buffer, (char*)lppPart) - buffer;

				char DirName[BUFSIZE];
				memset(DirName, 0, BUFSIZE);
				strncpy(DirName, buffer, nPos-1);

				strResult = std::string(DirName);
			}
		}
	}

	return strResult;
}

/*!****************************************************************************
* @brief	Gets the application data folder
* @return	Returns the path to data folder
******************************************************************************/
std::string GetDataPath()
{
	std::string strDataPath = GetExePath() + std::string(DATAFOLDER);

	return strDataPath;
}

/*!****************************************************************************
* @brief	Gets the file size
* @param	fp Pointer to a FILE struct
* @return	Returns the size for a specified file
******************************************************************************/
unsigned GetFileSize(FILE *fp)
{
    int prev = ftell(fp);

    fseek(fp, 0L, SEEK_END);
    
	unsigned nSize = ftell(fp);
											//go back to where we were
    fseek(fp,prev,SEEK_SET);

    return nSize;
}

/*!****************************************************************************
* @brief	Checks for WAV audo file
* @param	strFileName The PCM WAV file name
* @return	Returns true if the file is of PCM WAV type, false otherwise
******************************************************************************/
bool IsWavFile(std::string strFileName)
{
	bool bResult = false;

	FILE* fp = fopen(strFileName.c_str(), "rb");

	if( fp )
	{
		BYTE MagicWord[4+1];
		memset(MagicWord, 0, sizeof(MagicWord));

		fseek(fp, 0, SEEK_SET);
		fread(MagicWord, 4, 1, fp);

#ifdef _DEBUG
	char strBuffer[256];

	sprintf(strBuffer, "sizeof magic word = %d\n", sizeof(MagicWord));
	OutputDebugStringA(strBuffer);

	sprintf(strBuffer, "WAV magic word = %s\n", MagicWord);
	OutputDebugStringA(strBuffer);
#endif

		if( !_strcmpi((LPCSTR) MagicWord, "RIFF" ) )
		{
			bResult = true;
		}

		fclose(fp);
	}

	return bResult;
}

/*!****************************************************************************
* @brief	Extracts the "name" from a file, without path
* @param	strPath The full path of a file
* @param	bRemoveExt Specifies if wants to remove extension
* @return	Returns a string containing the name of a file
*			without the path and (optionally) extension
******************************************************************************/
std::string GetFileName(std::string strPath, bool bRemoveExt)
{
	const size_t last_slash_idx = strPath.find_last_of("\\/");

	if (std::string::npos != last_slash_idx)
	{
		strPath.erase(0, last_slash_idx + 1);
	}

	if ( bRemoveExt )
	{
		const size_t period_idx = strPath.rfind('.');
		if (std::string::npos != period_idx)
		{
			strPath.erase(period_idx);
		}
	}

	return strPath;
}

/*!****************************************************************************
* @brief	Shows a MessageBox containing a specified string
* @param	strMsg The message to show
******************************************************************************/
void Show(std::string strMsg)
{
	::MessageBoxA(0, strMsg.c_str(), "Info:",
		MB_OK | MB_ICONINFORMATION | MB_TASKMODAL);
}

/*!****************************************************************************
* @brief	Shows a MessageBox containing a list of integer values
* @param	IntList A vector of integers to show
******************************************************************************/
void Show(TVecIntegers IntList)
{
	char strList[4096];
	memset(strList, 0, sizeof(strList));

	for(int i=0; i<IntList.size(); i++)
	{
		char Buffer[256];
		sprintf(Buffer, "%d\n", IntList[i]);

		strcat(strList, Buffer);
	}

	::MessageBoxA(0, strList, "Info",
		MB_OK | MB_ICONINFORMATION | MB_TASKMODAL);
}

/*!****************************************************************************
* @brief	Shows a MessageBox containing a list of strings
* @param	strList A vector of strings to show
******************************************************************************/
void Show(TVecStrings strList)
{
/*
	WCHAR strBuffer[4096];

	for(int i=0; i<strList.size(); i++)
	{
		WCHAR strTemp[256];
		wsprintf(strTemp, TEXT("%s\n"), strList[i]);

												// prevents the buffer overrun
		if( wcslen(strBuffer) < sizeof(strBuffer)-1)
		{
			wcscat(strBuffer, strTemp);
		}
	}

	::MessageBox(0, strBuffer, TEXT("Info"),
		MB_OK | MB_ICONINFORMATION | MB_TASKMODAL);
*/
}

/*!****************************************************************************
* @brief	Checks for platform endianness
* @return	Returns true if the hardware platform is "big endian"
******************************************************************************/
bool IsBigEndian()
{
    int nInt = 1;

    return !( (char*) & nInt)[0];
}

/*!****************************************************************************
* @brief	Converts a sequence of bytes in integer, basing on endianness
* @param	pBuffer Pointer to a sequence of bytes
* @param	nLen Length of sequence of bytes to be converted
* @return	The integer corresponding to a sequence of nLen bytes
******************************************************************************/
int ConvertToInt(char* pBuffer, int nLen)
{
    int a = 0;

    if (!IsBigEndian())
	{
        for (int i = 0; i<nLen; i++)
		{
            ((char*)&a)[i] = pBuffer[i];
		}
	}
    else
	{
        for (int i = 0; i<nLen; i++)
		{
            ((char*)&a)[3 - i] = pBuffer[i];
		}
	}

    return a;
}


