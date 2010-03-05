#ifndef _COMMON_H
#define _COMMON_H

#include <windows.h>
#include "dxctrl\dxctrl.h"

PVOID ManualLoadDataResource(HMODULE hModule, INT iResourceId, PDWORD pdwAtlasSize);
VOID GetMyDirectory(HMODULE hModule, PSZ pszDir);
VOID LoadSavedState();

extern HMODULE          g_hinstDLL;
extern BYTE             g_bNop[];
extern PCDXCtrlDialog   g_pMainWindow;
extern PCDXCtrlButton   g_pClose;
extern PCDXCtrlButton   g_pXLeft;
extern PCDXCtrlButton   g_pXRight;
extern PCDXCtrlTextbox  g_pXEdit;
extern PCDXCtrlButton   g_pYLeft;
extern PCDXCtrlButton   g_pYRight;
extern PCDXCtrlTextbox  g_pYEdit;
extern PCDXCtrlButton   g_pZLeft;
extern PCDXCtrlButton   g_pZRight;
extern PCDXCtrlTextbox  g_pZEdit;
extern PCDXCtrlButton   g_pSLeft;
extern PCDXCtrlButton   g_pSRight;
extern PCDXCtrlTextbox  g_pSEdit;
extern PCDXCtrlButton   g_pDisableHotkeys;
extern PCDXCtrlButton   g_pNoFallDamage;
extern PCDXCtrlButton   g_pMountainClimb;
extern PCDXCtrlButton   g_pLockSpeed;
extern PCDXCtrlButton   g_pZeroGravity;
extern PCDXCtrlButton   g_pTeleportToPlane;
extern PCDXCtrlButton   g_pFollowNPC;
extern PCDXCtrlTextbox  g_pTrackEdit;
extern PCDXCtrlButton   g_pAlliance;
extern PCDXCtrlButton   g_pHorde;
extern PCDXCtrlButton   g_pBeasts;
extern PCDXCtrlButton   g_pDemons;
extern PCDXCtrlButton   g_pDragons;
extern PCDXCtrlButton   g_pElementals;
extern PCDXCtrlButton   g_pGiants;
extern PCDXCtrlButton   g_pHumanoids;
extern PCDXCtrlButton   g_pUndead;
extern PCDXCtrlButton   g_pMachines;
extern PCDXCtrlButton   g_pSlimes;
extern PCDXCtrlButton   g_pCritters;
extern PCDXCtrlButton   g_pTreasure;
extern PCDXCtrlButton   g_pObjects;
extern PCDXCtrlButton   g_pHerbs;
extern PCDXCtrlButton   g_pMinerals;
extern PCDXCtrlButton   g_pNewGroup;
extern PCDXCtrlButton   g_pAddLocation;
extern PCDXCtrlButton   g_pEditEntry;
extern PCDXCtrlButton   g_pDeleteEntry;
extern PCDXCtrlButton   g_pMoveUp;
extern PCDXCtrlButton   g_pMoveDown;
extern PCDXCtrlButton   g_pTeleport;
extern PCDXCtrlButton   g_pGoToCorpse;
extern PCDXCtrlButton   g_pGPS;
extern PCDXCtrlTextbox  g_pFileEdit;
extern PCDXCtrlButton   g_pLoad;
extern PCDXCtrlTreeview g_pTreeview;
extern PCDXCtrlWindow   g_pAddLocationWindow;
extern PCDXCtrlTextbox  g_pAddLocationNEdit;
extern PCDXCtrlTextbox  g_pAddLocationXEdit;
extern PCDXCtrlTextbox  g_pAddLocationYEdit;
extern PCDXCtrlTextbox  g_pAddLocationZEdit;
extern PCDXCtrlTextbox  g_pAddLocationMEdit;
extern PCDXCtrlButton   g_pAddLocationOk;
extern PCDXCtrlButton   g_pAddLocationCancel;
extern PCDXCtrlWindow   g_pNewGroupWindow;
extern PCDXCtrlTextbox  g_pNewGroupNEdit;
extern PCDXCtrlButton   g_pNewGroupOk;
extern PCDXCtrlButton   g_pNewGroupCancel;
extern PCDXCtrlDialog   g_pGPSWindow;
extern PCDXCtrlButton   g_pGPSClose;
extern PCDXCtrlStatic   g_pGPSXEdit;
extern PCDXCtrlStatic   g_pGPSYEdit;
extern PCDXCtrlStatic   g_pGPSZEdit;
extern PCDXCtrlStatic   g_pGPSSEdit;
extern PCDXCtrlStatic   g_pGPSMEdit;

#endif