#ifndef _FILEMAP_H
#define _FILEMAP_H

#include <windows.h>

#define FILEMAP_READ  1
#define FILEMAP_WRITE 2
#define FILEMAP_COPY  3

typedef struct _FILEMAP
{
	HANDLE hFile;
	HANDLE hMap;
	PBYTE  pbFile;
	DWORD  dwSize;
} FILEMAP, *PFILEMAP;

void CloseFileMap(PFILEMAP pfm);
BOOL IsPE(PBYTE pbImage);
BOOL OpenFileMap(char* szFilename, DWORD dwMode, PFILEMAP pfm);

#endif