#ifndef _TRACKING_H
#define _TRACKING_H

#include <windows.h>

VOID Tracking_HookTrackFunction(PDWORD pTrackingFunc, PVOID pNameLookupFunc, DWORD dwJumpSize, DWORD dwTrackStructOffset, DWORD dwTrackDataOffset, DWORD dwCharStruct2Offset, DWORD dwCharTypeOffset, DWORD dwNpcDataOffset);
VOID Tracking_SetTrackData(BOOL bAlliance, BOOL bHorde, BOOL bBeasts, BOOL bDemons, BOOL bDragons, BOOL bElementals, BOOL bGiants, BOOL bHumanoids, BOOL bUndead, BOOL bMachines, BOOL bSlimes, BOOL bCritters, BOOL bTreasure, BOOL bObjects, BOOL bHerbs, BOOL bMinerals);
VOID Tracking_SetTrackString(PSZ pszString);
VOID Tracking_UnHookTrackFunction(PDWORD pTrackingFunc);

#endif