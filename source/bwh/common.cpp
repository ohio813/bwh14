#include "common.h"

// global variables
BYTE g_bNop[] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};

HMODULE          g_hinstDLL;
PCDXCtrlDialog   g_pMainWindow;
PCDXCtrlButton   g_pClose;
PCDXCtrlButton   g_pXLeft;
PCDXCtrlButton   g_pXRight;
PCDXCtrlTextbox  g_pXEdit;
PCDXCtrlButton   g_pYLeft;
PCDXCtrlButton   g_pYRight;
PCDXCtrlTextbox  g_pYEdit;
PCDXCtrlButton   g_pZLeft;
PCDXCtrlButton   g_pZRight;
PCDXCtrlTextbox  g_pZEdit;
PCDXCtrlButton   g_pSLeft;
PCDXCtrlButton   g_pSRight;
PCDXCtrlTextbox  g_pSEdit;
PCDXCtrlButton   g_pDisableHotkeys;
PCDXCtrlButton   g_pNoFallDamage;
PCDXCtrlButton   g_pMountainClimb;
PCDXCtrlButton   g_pLockSpeed;
PCDXCtrlButton   g_pZeroGravity;
PCDXCtrlButton   g_pTeleportToPlane;
PCDXCtrlButton   g_pFollowNPC;
PCDXCtrlTextbox  g_pTrackEdit;
PCDXCtrlButton   g_pAlliance;
PCDXCtrlButton   g_pHorde;
PCDXCtrlButton   g_pBeasts;
PCDXCtrlButton   g_pDemons;
PCDXCtrlButton   g_pDragons;
PCDXCtrlButton   g_pElementals;
PCDXCtrlButton   g_pGiants;
PCDXCtrlButton   g_pHumanoids;
PCDXCtrlButton   g_pUndead;
PCDXCtrlButton   g_pMachines;
PCDXCtrlButton   g_pSlimes;
PCDXCtrlButton   g_pCritters;
PCDXCtrlButton   g_pTreasure;
PCDXCtrlButton   g_pObjects;
PCDXCtrlButton   g_pHerbs;
PCDXCtrlButton   g_pMinerals;
PCDXCtrlButton   g_pNewGroup;
PCDXCtrlButton   g_pAddLocation;
PCDXCtrlButton   g_pEditEntry;
PCDXCtrlButton   g_pDeleteEntry;
PCDXCtrlButton   g_pMoveUp;
PCDXCtrlButton   g_pMoveDown;
PCDXCtrlButton   g_pTeleport;
PCDXCtrlButton   g_pGoToCorpse;
PCDXCtrlButton   g_pGPS;
PCDXCtrlTextbox  g_pFileEdit;
PCDXCtrlButton   g_pLoad;
PCDXCtrlTreeview g_pTreeview;
PCDXCtrlWindow   g_pAddLocationWindow;
PCDXCtrlTextbox  g_pAddLocationNEdit;
PCDXCtrlTextbox  g_pAddLocationXEdit;
PCDXCtrlTextbox  g_pAddLocationYEdit;
PCDXCtrlTextbox  g_pAddLocationZEdit;
PCDXCtrlTextbox  g_pAddLocationMEdit;
PCDXCtrlButton   g_pAddLocationOk;
PCDXCtrlButton   g_pAddLocationCancel;
PCDXCtrlWindow   g_pNewGroupWindow;
PCDXCtrlTextbox  g_pNewGroupNEdit;
PCDXCtrlButton   g_pNewGroupOk;
PCDXCtrlButton   g_pNewGroupCancel;
PCDXCtrlDialog   g_pGPSWindow;
PCDXCtrlButton   g_pGPSClose;
PCDXCtrlStatic   g_pGPSXEdit;
PCDXCtrlStatic   g_pGPSYEdit;
PCDXCtrlStatic   g_pGPSZEdit;
PCDXCtrlStatic   g_pGPSSEdit;
PCDXCtrlStatic   g_pGPSMEdit;

// code start
__declspec(dllexport) char szModuleFilename[MAX_PATH];

DWORD GetModuleFileNameU(HMODULE hModule, PSZ pszFilename)
{
	DWORD dwSize = GetModuleFileName(hModule, pszFilename, MAX_PATH);
	if(dwSize == 0)
	{
		strcpy(pszFilename, szModuleFilename);
		dwSize = (DWORD)strlen(szModuleFilename);
	}
	return dwSize;
}

VOID GetMyDirectory(HMODULE hModule, PSZ pszDir)
{
   DWORD dwLength = GetModuleFileNameU(hModule, pszDir);
   for(; pszDir[dwLength] != '\\'; dwLength--)
       pszDir[dwLength] = '\0';
}

PVOID ManualLoadDataResource(HMODULE hModule, INT iResourceId, PDWORD pdwAtlasSize)
{
	if(pdwAtlasSize) *pdwAtlasSize = NULL;
	if(hModule)
	{
		PIMAGE_RESOURCE_DIRECTORY pMainDir = PIMAGE_RESOURCE_DIRECTORY(UlongToPtr(PIMAGE_NT_HEADERS(UlongToPtr(PtrToUlong(hModule) + PIMAGE_DOS_HEADER(hModule)->e_lfanew))->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress + PtrToUlong(hModule)));
		if(PtrToUlong(pMainDir) != PtrToUlong(hModule))
		{
			PIMAGE_RESOURCE_DIRECTORY_ENTRY pMainEntry = PIMAGE_RESOURCE_DIRECTORY_ENTRY(pMainDir + 1);
			for(WORD x = 0; x < pMainDir->NumberOfIdEntries + pMainDir->NumberOfIdEntries; x++)
			{
				if(pMainEntry[x].NameIsString == FALSE && pMainEntry[x].Id == PtrToUlong(RT_RCDATA))
				{
					PIMAGE_RESOURCE_DIRECTORY pDataDir = PIMAGE_RESOURCE_DIRECTORY(UlongToPtr(PtrToUlong(pMainDir) + pMainEntry[x].OffsetToDirectory));
					PIMAGE_RESOURCE_DIRECTORY_ENTRY pDataEntry = PIMAGE_RESOURCE_DIRECTORY_ENTRY(pDataDir + 1);
					for(WORD y = 0; y < pDataDir->NumberOfIdEntries + pMainDir->NumberOfIdEntries; y++)
					{
						if(pDataEntry[y].NameIsString == FALSE && pDataEntry[y].Id == iResourceId)
						{
							PIMAGE_RESOURCE_DIRECTORY pLangDir = PIMAGE_RESOURCE_DIRECTORY(UlongToPtr(PtrToUlong(pMainDir) + pDataEntry[y].OffsetToDirectory));
							PIMAGE_RESOURCE_DIRECTORY_ENTRY pLangEntry = PIMAGE_RESOURCE_DIRECTORY_ENTRY(pLangDir + 1);
							PIMAGE_RESOURCE_DATA_ENTRY pData = PIMAGE_RESOURCE_DATA_ENTRY(UlongToPtr(PtrToUlong(pMainDir) + pLangEntry->OffsetToData));
							*pdwAtlasSize = pData->Size;
							return UlongToPtr(pData->OffsetToData + PtrToUlong(hModule));
						}
					}
					break;
				}
			}
		}
	}
	return NULL;
}
