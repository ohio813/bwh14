#include "tracking.h"
#include "teleport.h"

// defines
typedef enum _CHAR_RACE
{
	CHAR_RACE_HUMAN    = 1,
	CHAR_RACE_ORC      = 2,
	CHAR_RACE_DWARF    = 3,
	CHAR_RACE_NIGHTELF = 4,
	CHAR_RACE_UNDEAD   = 5,
	CHAR_RACE_TAUREN   = 6,
	CHAR_RACE_GNOME    = 7,
	CHAR_RACE_TROLL    = 8,
	CHAR_RACE_GOBLIN   = 9
} CHAR_RACE;
typedef enum _TRACK_RESULT
{
	TRACK_RESULT_DEFAULT = 0,
	TRACK_RESULT_SHOW    = 1,
	TRACK_RESULT_HIDE    = 2
} TRACK_RESULT;

//typedef struct _TRACK_DATA
//{
//	DWORD dwDontCare[816];
//	DWORD dwTrack;
//	DWORD dwResources;
//} TRACK_DATA, *PTRACK_DATA;
//
//typedef struct _CHARACTER_DATA2
//{
//	DWORD dwDontCare[30];
//	BYTE bCharType;
//} CHARACTER_DATA2, *PCHARACTER_DATA2;
//
//typedef struct _CHARACTER_DATA
//{
//	DWORD dwDontCare[68];
//	PCHARACTER_DATA2 pCharData2;
//	DWORD dwDontCare2[611];
//	PVOID pNpcData;
//	DWORD dwDontCare3[205];
//	PTRACK_DATA pTrackData;
//} CHARACTER_DATA, *PCHARACTER_DATA;

typedef PSZ(__fastcall *PNAMELOOKUP)(PBYTE pCharData);

// function prototypes
BOOL                   Tracking_MatchPattern(PSZ pszPattern, PSZ pszString, DWORD dwPatternLength);
TRACK_RESULT __stdcall Tracking_Hook(PBYTE pTargetData);
VOID                   Tracking_Stub();

// global variables
BOOL        g_bTrackAlliance = FALSE;
BOOL        g_bTrackHorde = FALSE;
DWORD       g_dwTrackData = 0;
DWORD       g_dwResourceData = 0;
DWORD       g_pOrigTrackingFunc = NULL;
char        g_szTrackString[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

PNAMELOOKUP g_NameLookup = NULL;
DWORD       g_JumpSize = NULL;
DWORD       g_TrackStructOffset = NULL;
DWORD       g_TrackDataOffset = NULL;
DWORD       g_CharStruct2Offset = NULL;
DWORD       g_CharTypeOffset = NULL;
DWORD       g_NpcDataOffset = NULL;

// code start
VOID Tracking_HookTrackFunction(PDWORD pTrackingFunc, PVOID pNameLookupFunc, DWORD dwJumpSize, DWORD dwTrackStructOffset, DWORD dwTrackDataOffset, DWORD dwCharStruct2Offset, DWORD dwCharTypeOffset, DWORD dwNpcDataOffset)
{
	DWORD dwOldProtect;
	
	g_NameLookup = (PNAMELOOKUP)pNameLookupFunc;
	g_JumpSize = dwJumpSize;
	g_TrackStructOffset = dwTrackStructOffset;
	g_TrackDataOffset = dwTrackDataOffset;
	g_CharStruct2Offset = dwCharStruct2Offset;
	g_CharTypeOffset = dwCharTypeOffset;
	g_NpcDataOffset = dwNpcDataOffset;
	
	if(pTrackingFunc && g_JumpSize && VirtualProtect(pTrackingFunc, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect))
	{
		g_pOrigTrackingFunc = *pTrackingFunc + (PtrToUlong(pTrackingFunc) + 4);
		*pTrackingFunc = PtrToUlong(Tracking_Stub) - (PtrToUlong(pTrackingFunc) + 4);
		VirtualProtect(pTrackingFunc, 4, dwOldProtect, &dwOldProtect);
	}
}
BOOL Tracking_MatchPattern(PSZ pszPattern, PSZ pszString, DWORD dwPatternLength)
{
	if(pszString)
	{
		DWORD dwStringLength = (DWORD)strlen(pszString);
		if(dwStringLength >= dwPatternLength)
		{
			for(DWORD x = 0; x <= dwStringLength - dwPatternLength; x++)
			{
				if(stricmp(pszPattern, &pszString[x]) == 0)
					return TRUE;
			}
		}
	}
	return FALSE;
}

VOID Tracking_SetTrackData(BOOL bAlliance, BOOL bHorde, BOOL bBeasts, BOOL bDemons, BOOL bDragons, BOOL bElementals, BOOL bGiants, BOOL bHumanoids, BOOL bUndead, BOOL bMachines, BOOL bSlimes, BOOL bCritters, BOOL bTreasure, BOOL bObjects, BOOL bHerbs, BOOL bMinerals)
{
	g_bTrackAlliance = bAlliance;
	g_bTrackHorde = bHorde;
	g_dwTrackData = 0;
	g_dwResourceData = 0;
	if(bBeasts) g_dwTrackData |= 1;
	if(bDemons) g_dwTrackData |= 4;
	if(bDragons) g_dwTrackData |= 2;
	if(bElementals) g_dwTrackData |= 8;
	if(bGiants) g_dwTrackData |= 16;
	if(bHumanoids) g_dwTrackData |= 64;
	if(bUndead) g_dwTrackData |= 32;
	if(bMachines) g_dwTrackData |= 256;
	if(bSlimes) g_dwTrackData |= 512;
	if(bCritters) g_dwTrackData |= 128;
	if(bTreasure) g_dwResourceData |= 32;
	if(bObjects) g_dwResourceData |= 4096;
	if(bHerbs) g_dwResourceData |= 2;
	if(bMinerals) g_dwResourceData |= 4;
}

VOID Tracking_SetTrackString(PSZ pszString)
{
	strncpy(g_szTrackString, pszString, sizeof(g_szTrackString) - 1);
	g_szTrackString[sizeof(g_szTrackString) - 1] = '\0';
}

TRACK_RESULT __stdcall Tracking_Hook(PBYTE pTargetData)
{
	DWORD lenTrackString = (DWORD)strlen(g_szTrackString);
	PSZ pszTargetName;
	PBYTE pTrackStruct;
	PBYTE pCharStruct2;

	PBYTE pCharData = (PBYTE)Teleport_LookupCharBlock();

	if(pCharData && g_TrackStructOffset && g_TrackDataOffset)
	{
		pTrackStruct = (PBYTE)UlongToPtr(*((PDWORD)&pCharData[g_TrackStructOffset]));
		if(pTrackStruct)
		{
			*((PDWORD)&pTrackStruct[g_TrackDataOffset]) = g_dwTrackData;
			*((PDWORD)&pTrackStruct[g_TrackDataOffset+4]) = g_dwResourceData;
		}
	}
	if(lenTrackString && g_NameLookup)
	{
		pszTargetName = g_NameLookup(pTargetData);
		if(Tracking_MatchPattern(g_szTrackString, pszTargetName, lenTrackString))
			return TRACK_RESULT_SHOW;
	}
	if(pTargetData && g_NpcDataOffset && g_CharTypeOffset && g_CharStruct2Offset)
	{
		pCharStruct2 = (PBYTE)UlongToPtr(*((PDWORD)&pTargetData[g_CharStruct2Offset]));
		if(*((PDWORD)&pTargetData[g_NpcDataOffset]) == NULL && pCharStruct2)
		{
			if(g_bTrackAlliance &&
				(pCharStruct2[g_CharTypeOffset] == CHAR_RACE_HUMAN ||
				 pCharStruct2[g_CharTypeOffset] == CHAR_RACE_DWARF ||
				 pCharStruct2[g_CharTypeOffset] == CHAR_RACE_NIGHTELF ||
				 pCharStruct2[g_CharTypeOffset] == CHAR_RACE_GNOME))
				 return TRACK_RESULT_SHOW;
			if(g_bTrackHorde &&
				(pCharStruct2[g_CharTypeOffset] == CHAR_RACE_ORC ||
				 pCharStruct2[g_CharTypeOffset] == CHAR_RACE_UNDEAD ||
				 pCharStruct2[g_CharTypeOffset] == CHAR_RACE_TAUREN ||
				 pCharStruct2[g_CharTypeOffset] == CHAR_RACE_TROLL))
				 return TRACK_RESULT_SHOW;
			return TRACK_RESULT_HIDE;
		}
	}
	return TRACK_RESULT_DEFAULT;
}

__declspec(naked) VOID Tracking_Stub()
{
	__asm
	{
		pushad
		push ecx
		call dword ptr[Tracking_Hook]
		mov [esp+1Ch], eax
		popad
		test eax, eax
		jz do_def
		dec eax
		jnz no_show
		mov eax, dword ptr[g_JumpSize]
		sub dword ptr[esp], eax
no_show:xor eax, eax
        ret
do_def: mov eax, dword ptr[g_pOrigTrackingFunc]
		jmp eax
	}
}

VOID Tracking_UnHookTrackFunction(PDWORD pTrackingFunc)
{
	DWORD dwOldProtect;
	if(pTrackingFunc && g_pOrigTrackingFunc)
	{
		VirtualProtect(pTrackingFunc, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);
		*pTrackingFunc = g_pOrigTrackingFunc - (PtrToUlong(pTrackingFunc) + 4);
		VirtualProtect(pTrackingFunc, 4, dwOldProtect, &dwOldProtect);
		g_pOrigTrackingFunc = NULL;
	}
}
