#include "filemap.h"

#undef RtlZeroMemory
extern "C" WINBASEAPI void WINAPI RtlZeroMemory(PVOID Destination, SIZE_T Length);

VOID CloseFileMap(PFILEMAP pfm)
{
	UnmapViewOfFile(pfm->pbFile);
	CloseHandle(pfm->hMap);
	CloseHandle(pfm->hFile);
}

BOOL IsPE(PBYTE pbImage)
{
	PIMAGE_DOS_HEADER pDosHdr = PIMAGE_DOS_HEADER(pbImage);
	PIMAGE_NT_HEADERS pNtHdr;
	BOOL bRet = FALSE;
	if(pbImage && pDosHdr->e_magic == IMAGE_DOS_SIGNATURE)
	{
		pNtHdr = PIMAGE_NT_HEADERS(pbImage + pDosHdr->e_lfanew);
		if(pNtHdr->Signature == IMAGE_NT_SIGNATURE)
			bRet = TRUE;
	}
	return bRet;
}

BOOL OpenFileMap(PSZ szFilename, DWORD dwMode, PFILEMAP pfm)
{
	DWORD dwCreateAccess;
	DWORD dwMapAccess;
	DWORD dwViewAccess;
	
	switch(dwMode)
	{
	case FILEMAP_READ:
		dwCreateAccess = GENERIC_READ;
		dwMapAccess = PAGE_READONLY;
		dwViewAccess = FILE_MAP_READ;
		break;
	case FILEMAP_WRITE:
		dwCreateAccess = GENERIC_READ | GENERIC_WRITE;
		dwMapAccess = PAGE_READWRITE;
		dwViewAccess = FILE_MAP_WRITE;
		break;
	case FILEMAP_COPY:
		dwCreateAccess = GENERIC_READ | GENERIC_WRITE;
		dwMapAccess = PAGE_WRITECOPY;
		dwViewAccess = FILE_MAP_COPY;
		break;
	default:
		return FALSE;
		break;
	}
	
	RtlZeroMemory(pfm, sizeof(FILEMAP));

	pfm->hFile = CreateFile(szFilename, dwCreateAccess, NULL, NULL, OPEN_EXISTING, NULL, NULL);
	if(pfm->hFile != INVALID_HANDLE_VALUE)
	{
		pfm->dwSize = GetFileSize(pfm->hFile, NULL);
		pfm->hMap = CreateFileMapping(pfm->hFile, NULL, dwMapAccess, 0, 0, NULL);
		if(pfm->hMap)
		{
			pfm->pbFile = (PBYTE)MapViewOfFile(pfm->hMap, dwViewAccess, 0, 0, 0);
			if(pfm->pbFile)
				return TRUE;
			CloseHandle(pfm->hMap);
			pfm->hMap = NULL;
		}
		CloseHandle(pfm->hFile);
		pfm->hFile = NULL;
		pfm->dwSize = 0;
	}
	return FALSE;
}
