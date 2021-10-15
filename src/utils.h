#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>
#include <vector>
#include <iostream>

void Debounce();

typedef std::vector<int> TVecIntegers;
typedef std::vector<std::string> TVecStrings;

std::string GetExePath();
std::string GetDataPath();

unsigned GetFileSize(FILE *fp);
bool IsWavFile(std::string strFileName);

std::string GetFileName(std::string strPath, bool bRemoveExt);

void Show(std::string strMsg);
void Show(TVecIntegers IntList);
void Show(TVecStrings strList);

bool IsBigEndian();
int ConvertToInt(char* buffer, int len);

#endif

