#include <windows.h>
#include <stdio.h>
#include "settings.h"
#include "hooker.h"
#include "hacks\tracking.h"
#include "hacks\teleport.h"
#include "hacks\nofalldamage.h"
#include "hacks\mountainclimb.h"
#include "hacks\lockspeed.h"
#include "hacks\zerogravity.h"
#include "hacks\teleporttoplane.h"
#include "hacks\follownpc.h"
#include "dxctrl\dxctrl.h"
#include "listmanager.h"
#include "common.h"
#include "resource.h"

// function prototypes
BOOL WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID LoadSavedState();

// global variables
SETTINGS        g_Settings;
PCDXCtrlManager g_pD3DManager;
BOOL            g_bLeftButtonDown = FALSE;
LOCATION        g_locQuickSave = {-1, 0, 0, 0, 0};
BOOL            g_bDisableHotkeys = FALSE;
BOOL            g_bAltDown = FALSE;
BOOL            g_bCtrlDown = FALSE;

// code start
BOOL __stdcall ChatMessageRecv(PSZ pszMsg)
{
	LOCATION loc;
	char szBwhTele[] = "bwhtele ";
	if(memcmp(pszMsg, szBwhTele, sizeof(szBwhTele) - 1) == 0)
	{
		if(ListManager_GetLocation(atoi(&pszMsg[8]), &loc.dwMap, &loc.fX, &loc.fY, &loc.fZ))
		{
			Teleport_Teleport(&loc);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch(fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        g_hinstDLL = hinstDLL;
		DisableThreadLibraryCalls(hinstDLL);
        if( Settings_LoadINI(hinstDLL, &g_Settings) &&
		    HookWindowProcedure(WindowProc))
		{
			g_pD3DManager = HookD3D(hinstDLL, g_Settings.Present, g_Settings.Reset, g_Settings.FixedFont, g_Settings.FixedSize, g_Settings.FixedBold, g_Settings.ProportionalFont, g_Settings.ProportionalSize, g_Settings.ProportionalBold);
			if(g_pD3DManager)
			{
				Teleport_Init(g_Settings.GetMap, g_Settings.GetGuidData, g_Settings.GetGuidDataConstant1, g_Settings.GetGuidDataConstant2, g_Settings.GetCharacterGuid, g_Settings.FallingTeleport, g_Settings.CameraAngle, g_Settings.Corpse);
				HookChatFunction(g_Settings.SendChatMessage, ChatMessageRecv);
				Tracking_HookTrackFunction(g_Settings.TrackingHook, g_Settings.NameLookup, g_Settings.TrackingJumpSize, g_Settings.TrackingTrackStructOffset, g_Settings.TrackingTrackDataOffset, g_Settings.TrackingCharStruct2Offset, g_Settings.TrackingCharTypeOffset, g_Settings.TrackingNpcDataOffset);
				NoFallDamage_Hook(g_Settings.NoFallDamageHook, g_Settings.NoFallDamageOffset);
				return TRUE;
			}
		}
		break;
    case DLL_PROCESS_DETACH:
		NoFallDamage_UnHook(g_Settings.NoFallDamageHook);
		Tracking_UnHookTrackFunction(g_Settings.TrackingHook);
		UnHookD3D(g_Settings.Present, g_Settings.Reset);
		UnHookWindowProcedure();
        UnHookChatFunction(g_Settings.SendChatMessage);
		break;
    }
    return FALSE;	
}

VOID LoadSavedState()
{
	DWORD dwHacks;
	DWORD dwTracks;
	char szLocationFile[21];
	char szTrackPattern[27];

	Settings_LoadHacks(&dwHacks);
	if(dwHacks & 1)
	{
		g_pDisableHotkeys->SetCheckState(TRUE);
		g_bDisableHotkeys = TRUE;
	}
	if(dwHacks & 2)
	{
		g_pNoFallDamage->SetCheckState(TRUE);
		NoFallDamage_Enable(TRUE);
	}
	if(dwHacks & 4)
	{
		g_pMountainClimb->SetCheckState(TRUE);
		MountainClimb_Enable(g_Settings.MountainClimb, TRUE);
	}
	if(dwHacks & 8)
	{
		g_pLockSpeed->SetCheckState(TRUE);
		LockSpeed_Enable(g_Settings.LockSpeed, TRUE);
	}
	if(dwHacks & 16)
	{
		g_pZeroGravity->SetCheckState(TRUE);
		ZeroGravity_Enable(g_Settings.ZeroGravity, TRUE);
	}
	if(dwHacks & 32)
	{
		g_pTeleportToPlane->SetCheckState(TRUE);
		TeleportToPlane_Enable(g_Settings.TeleportToPlane, TRUE);
	}
	if(dwHacks & 64)
	{
		g_pFollowNPC->SetCheckState(TRUE);
		FollowNPC_Enable(g_Settings.FollowNpc1, g_Settings.FollowNpc2, g_Settings.FollowFaction, g_Settings.FollowDistance, TRUE);
	}

	Settings_LoadLocationFile(szLocationFile);
	g_pFileEdit->SetText(szLocationFile);
	ListManager_FileEditChanged();

	Settings_LoadTrackPattern(szTrackPattern);
	g_pTrackEdit->SetText(szTrackPattern);
	Tracking_SetTrackString(szTrackPattern);

	Settings_LoadTracks(&dwTracks);
	if(dwTracks & 1)
		g_pAlliance->SetCheckState(TRUE);
	if(dwTracks & 2)
		g_pHorde->SetCheckState(TRUE);
	if(dwTracks & 4)
		g_pBeasts->SetCheckState(TRUE);
	if(dwTracks & 8)
		g_pDemons->SetCheckState(TRUE);
	if(dwTracks & 16)
		g_pDragons->SetCheckState(TRUE);
	if(dwTracks & 32)
		g_pElementals->SetCheckState(TRUE);
	if(dwTracks & 64)
		g_pGiants->SetCheckState(TRUE);
	if(dwTracks & 128)
		g_pHumanoids->SetCheckState(TRUE);
	if(dwTracks & 256)
		g_pUndead->SetCheckState(TRUE);
	if(dwTracks & 512)
		g_pMachines->SetCheckState(TRUE);
	if(dwTracks & 1024)
		g_pSlimes->SetCheckState(TRUE);
	if(dwTracks & 2048)
		g_pCritters->SetCheckState(TRUE);
	if(dwTracks & 4096)
		g_pTreasure->SetCheckState(TRUE);
	if(dwTracks & 8192)
		g_pObjects->SetCheckState(TRUE);
	if(dwTracks & 16384)
		g_pHerbs->SetCheckState(TRUE);
	if(dwTracks & 32768)
		g_pMinerals->SetCheckState(TRUE);
	Tracking_SetTrackData(g_pAlliance->GetCheckState(), g_pHorde->GetCheckState(), g_pBeasts->GetCheckState(), g_pDemons->GetCheckState(), g_pDragons->GetCheckState(), g_pElementals->GetCheckState(), g_pGiants->GetCheckState(), g_pHumanoids->GetCheckState(), g_pUndead->GetCheckState(), g_pMachines->GetCheckState(), g_pSlimes->GetCheckState(), g_pCritters->GetCheckState(), g_pTreasure->GetCheckState(), g_pObjects->GetCheckState(), g_pHerbs->GetCheckState(), g_pMinerals->GetCheckState());
}

BOOL WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char szBuff[27];
	LOCATION loc;
	PVOID pFocusedControl;
	WORD wCode, wId;
	BOOL bRet = FALSE;
	switch(uMsg)
	{

	///////////////////
	// WM_CANCELMODE //
	///////////////////
	case WM_CANCELMODE:
        g_bLeftButtonDown = FALSE;
		g_bCtrlDown = FALSE;
		g_bAltDown = FALSE;
        KillTimer(hwnd, IDT_CAMERASLEW);
        g_dwDirection = GO_NOWHERE;
        break;

	////////////////
	// WM_COMMAND //
	////////////////
	case WM_COMMAND:
		wCode = HIWORD(wParam);
		wId = LOWORD(wParam);
		switch(wId)
		{

		////////////////
		// GPS Window //
		////////////////
		case IDD_GPS:
			if(wCode == EN_CHANGE)
			{
				Settings_SaveGPSWindowPosition((DWORD)lParam);
				return TRUE;
			}
			break;
		case IDC_GPSCLOSE:
			if(wCode == BN_CLICKED)
			{
				g_pGPSWindow->Hide();
				return TRUE;
			}
			break;
		case IDC_GPS:
			if(wCode == BN_CLICKED)
			{
				if(g_pGPSWindow->IsVisible())
					g_pGPSWindow->Hide();
				else
					g_pGPSWindow->Show();
				return TRUE;
			}
			break;

		///////////////////
		// Hack Controls //
		///////////////////
		case IDC_DISABLEHOTKEYS:
			if(wCode == BN_CLICKED)
			{
				g_bDisableHotkeys = g_pDisableHotkeys->GetCheckState();
				Settings_SaveHacks();
				return TRUE;
			}
			break;
		case IDC_NOFALLDAMAGE:
			if(wCode == BN_CLICKED)
			{
				NoFallDamage_Enable(g_pNoFallDamage->GetCheckState());
				Settings_SaveHacks();
				return TRUE;
			}
			break;
		case IDC_MOUNTAINCLIMB:
			if(wCode == BN_CLICKED)
			{
				MountainClimb_Enable(g_Settings.MountainClimb, g_pMountainClimb->GetCheckState());
				Settings_SaveHacks();
				return TRUE;
			}
			break;
		case IDC_LOCKSPEED:
			if(wCode == BN_CLICKED)
			{
				LockSpeed_Enable(g_Settings.LockSpeed, g_pLockSpeed->GetCheckState());
				Settings_SaveHacks();
				return TRUE;
			}
			break;
		case IDC_ZEROGRAVITY:
			if(wCode == BN_CLICKED)
			{
				ZeroGravity_Enable(g_Settings.ZeroGravity, g_pZeroGravity->GetCheckState());
				Settings_SaveHacks();
				return TRUE;
			}
			break;
		case IDC_TELEPORTTOPLANE:
			if(wCode == BN_CLICKED)
			{
				TeleportToPlane_Enable(g_Settings.TeleportToPlane, g_pTeleportToPlane->GetCheckState());
				Settings_SaveHacks();
				return TRUE;
			}
			break;
		case IDC_FOLLOWNPC:
			if(wCode == BN_CLICKED)
			{
				FollowNPC_Enable(g_Settings.FollowNpc1, g_Settings.FollowNpc2, g_Settings.FollowFaction, g_Settings.FollowDistance, g_pFollowNPC->GetCheckState());
				Settings_SaveHacks();
				return TRUE;
			}

		///////////////////
		// List Controls //
		///////////////////
		case IDD_MAIN:
			if(wCode == EN_CHANGE)
			{
				Settings_SaveMainWindowPosition((DWORD)lParam);
				return TRUE;
			}
			break;
		case IDC_CLOSE:
			if(wCode == BN_CLICKED)
			{
				g_pMainWindow->Hide();
				g_pD3DManager->SetFocusedControl(NULL);
				return TRUE;
			}
			break;
		case IDC_FILEEDIT:
			if(wCode == EN_CHANGE)
			{
				ListManager_FileEditChanged();
				return TRUE;
			}
			if(wCode == EN_UPDATE)
				return TRUE;
			break;
		case IDC_LOAD:
			if(wCode == BN_CLICKED)
			{
				ListManager_Load(hwnd);
				return TRUE;
			}
			break;
		case IDC_TREEVIEW:
			if(wCode == LBN_SELCHANGE)
			{
				ListManager_ListClick(hwnd);
				return TRUE;
			}
			if(wCode == LBN_DBLCLK)
			{
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_TELEPORT, BN_CLICKED), (LPARAM)g_pTeleport);
				return TRUE;
			}
			break;
		case IDC_NEWGROUP:
			if(wCode == BN_CLICKED)
			{
				ListManager_NewGroup(hwnd);
				return TRUE;
			}
			break;
		case IDC_NEWGROUPOK:
			if(wCode == BN_CLICKED)
			{
				ListManager_NewGroupOk(hwnd);
				return TRUE;
			}
			break;
		case IDC_NEWGROUPCANCEL:
			if(wCode == BN_CLICKED)
			{
				ListManager_NewGroupCancel(hwnd);
				return TRUE;
			}
			break;
		case IDC_NEWGROUPNEDIT:
			if(wCode == EN_UPDATE)
			{
				ListManager_NewGroupOk(hwnd);
				return TRUE;
			}
			if(wCode == EN_CHANGE)
				return TRUE;
			break;
		case IDC_ADDLOCATION:
			if(wCode == BN_CLICKED)
			{
				Teleport_GetCurrentLocation(&loc);
				ListManager_AddLocation(hwnd, loc.fX, loc.fY, loc.fZ, loc.dwMap);
				return TRUE;
			}
			break;
		case IDC_ADDLOCATIONCANCEL:
			if(wCode == BN_CLICKED)
			{
				ListManager_AddLocationCancel(hwnd);
				return TRUE;
			}
			break;
		case IDC_ADDLOCATIONOK:
			if(wCode == BN_CLICKED)
			{
				ListManager_AddLocationOk(hwnd);
				return TRUE;
			}
		case IDC_ADDLOCATIONNEDIT:
			if(wCode == EN_UPDATE)
			{
				ListManager_AddLocationOk(hwnd);
				return TRUE;
			}
			if(wCode == EN_CHANGE)
				return TRUE;
			break;
		case IDC_ADDLOCATIONXEDIT:
			if(wCode == EN_UPDATE)
			{
				ListManager_AddLocationOk(hwnd);
				return TRUE;
			}
			if(wCode == EN_CHANGE)
				return TRUE;
			break;
		case IDC_ADDLOCATIONYEDIT:
			if(wCode == EN_UPDATE)
			{
				ListManager_AddLocationOk(hwnd);
				return TRUE;
			}
			if(wCode == EN_CHANGE)
				return TRUE;
			break;
		case IDC_ADDLOCATIONZEDIT:
			if(wCode == EN_UPDATE)
			{
				ListManager_AddLocationOk(hwnd);
				return TRUE;
			}
			if(wCode == EN_CHANGE)
				return TRUE;
			break;
		case IDC_ADDLOCATIONMEDIT:
			if(wCode == EN_UPDATE)
			{
				ListManager_AddLocationOk(hwnd);
				return TRUE;
			}
			if(wCode == EN_CHANGE)
				return TRUE;
			break;
		case IDC_EDITENTRY:
			if(wCode == BN_CLICKED)
			{
				ListManager_EditEntry(hwnd);
				return TRUE;
			}
			break;
		case IDC_DELETEENTRY:
			if(wCode == BN_CLICKED)
			{
				ListManager_DeleteEntry();
				return TRUE;
			}
			break;
		case IDC_MOVEUP:
			if(wCode == BN_CLICKED)
			{
				ListManager_MoveItem(TRUE);
				return TRUE;
			}
			break;
		case IDC_MOVEDOWN:
			if(wCode == BN_CLICKED)
			{
				ListManager_MoveItem(FALSE);
				return TRUE;
			}
			break;

		///////////////////////
		// Teleport Controls //
		///////////////////////
		case IDC_TELEPORT:
			if(wCode == BN_CLICKED)
			{
				if(ListManager_GetSelectedLocation(&loc.dwMap, &loc.fX, &loc.fY, &loc.fZ))
				{
					Teleport_Teleport(&loc);
				}
				return TRUE;
			}
			break;
		case IDC_GOTOCORPSE:
			if(wCode == BN_CLICKED)
			{
				Teleport_GoToCorpse();
				return TRUE;
			}
			break;
		case IDC_XLEFT:
			if(wCode == BN_CLICKED)
			{
				g_pXEdit->KillFocus(hwnd);
				Teleport_ShiftAxis(AXIS_X, -1);
				return TRUE;
			}
			break;
		case IDC_XRIGHT:
			if(wCode == BN_CLICKED)
			{
				g_pXEdit->KillFocus(hwnd);
				Teleport_ShiftAxis(AXIS_X, 1);
				return TRUE;
			}
			break;
		case IDC_XEDIT:
			if(wCode == EN_UPDATE)
			{
				Teleport_GetCurrentLocation(&loc);
				g_pXEdit->GetText(szBuff, sizeof(szBuff));
				loc.fX = (FLOAT)atof(szBuff);
				Teleport_Teleport(&loc);
				return TRUE;
			}
			if(wCode == EN_CHANGE)
				return TRUE;
			break;
		case IDC_YLEFT:
			if(wCode == BN_CLICKED)
			{
				g_pYEdit->KillFocus(hwnd);
				Teleport_ShiftAxis(AXIS_Y, -1);
				return TRUE;
			}
			break;
		case IDC_YRIGHT:
			if(wCode == BN_CLICKED)
			{
				g_pYEdit->KillFocus(hwnd);
				Teleport_ShiftAxis(AXIS_Y, 1);
				return TRUE;
			}
			break;
		case IDC_YEDIT:
			if(wCode == EN_UPDATE)
			{
				Teleport_GetCurrentLocation(&loc);
				g_pYEdit->GetText(szBuff, sizeof(szBuff));
				loc.fY = (FLOAT)atof(szBuff);
				Teleport_Teleport(&loc);
				return TRUE;
			}
			if(wCode == EN_CHANGE)
				return TRUE;
			break;
		case IDC_ZLEFT:
			if(wCode == BN_CLICKED)
			{
				g_pZEdit->KillFocus(hwnd);
				Teleport_ShiftAxis(AXIS_Z, -1);
				return TRUE;
			}
			break;
		case IDC_ZRIGHT:
			if(wCode == BN_CLICKED)
			{
				g_pZEdit->KillFocus(hwnd);
				Teleport_ShiftAxis(AXIS_Z, 1);
				return TRUE;
			}
			break;
		case IDC_ZEDIT:
			if(wCode == EN_UPDATE)
			{
				Teleport_GetCurrentLocation(&loc);
				g_pZEdit->GetText(szBuff, sizeof(szBuff));
				loc.fZ = (FLOAT)atof(szBuff);
				Teleport_Teleport(&loc);
				return TRUE;
			}
			if(wCode == EN_CHANGE)
				return TRUE;
			break;
		case IDC_SLEFT:
			if(wCode == BN_CLICKED)
			{
				g_pSEdit->KillFocus(hwnd);
				Teleport_ChangeSpeed(METHOD_ADD, -1);
				return TRUE;
			}
			break;
		case IDC_SRIGHT:
			if(wCode == BN_CLICKED)
			{
				g_pSEdit->KillFocus(hwnd);
				Teleport_ChangeSpeed(METHOD_ADD, 1);
				return TRUE;
			}
			break;
		case IDC_SEDIT:
			if(wCode == EN_UPDATE)
			{
				g_pSEdit->GetText(szBuff, sizeof(szBuff));
				Teleport_ChangeSpeed(METHOD_SET, (FLOAT)atof(szBuff));
				return TRUE;
			}
			if(wCode == EN_CHANGE)
				return TRUE;
			break;

		////////////////////
		// Track Controls //
		////////////////////
		case IDC_TRACKEDIT:
			if(wCode == EN_CHANGE)
			{
				g_pTrackEdit->GetText(szBuff, sizeof(szBuff));
				Tracking_SetTrackString(strupr(szBuff));
				Settings_SaveTrackPattern();
				return TRUE;
			}
			if(wCode == EN_UPDATE)
				return TRUE;
			break;
		case IDC_ALLIANCE:
			if(wCode == BN_CLICKED)
			{
				Tracking_SetTrackData(g_pAlliance->GetCheckState(), g_pHorde->GetCheckState(), g_pBeasts->GetCheckState(), g_pDemons->GetCheckState(), g_pDragons->GetCheckState(), g_pElementals->GetCheckState(), g_pGiants->GetCheckState(), g_pHumanoids->GetCheckState(), g_pUndead->GetCheckState(), g_pMachines->GetCheckState(), g_pSlimes->GetCheckState(), g_pCritters->GetCheckState(), g_pTreasure->GetCheckState(), g_pObjects->GetCheckState(), g_pHerbs->GetCheckState(), g_pMinerals->GetCheckState());
				Settings_SaveTracks();
				return TRUE;
			}
			break;
		case IDC_HORDE:
			if(wCode == BN_CLICKED)
			{
				Tracking_SetTrackData(g_pAlliance->GetCheckState(), g_pHorde->GetCheckState(), g_pBeasts->GetCheckState(), g_pDemons->GetCheckState(), g_pDragons->GetCheckState(), g_pElementals->GetCheckState(), g_pGiants->GetCheckState(), g_pHumanoids->GetCheckState(), g_pUndead->GetCheckState(), g_pMachines->GetCheckState(), g_pSlimes->GetCheckState(), g_pCritters->GetCheckState(), g_pTreasure->GetCheckState(), g_pObjects->GetCheckState(), g_pHerbs->GetCheckState(), g_pMinerals->GetCheckState());
				Settings_SaveTracks();
				return TRUE;
			}
			break;
		case IDC_BEASTS:
			if(wCode == BN_CLICKED)
			{
				Tracking_SetTrackData(g_pAlliance->GetCheckState(), g_pHorde->GetCheckState(), g_pBeasts->GetCheckState(), g_pDemons->GetCheckState(), g_pDragons->GetCheckState(), g_pElementals->GetCheckState(), g_pGiants->GetCheckState(), g_pHumanoids->GetCheckState(), g_pUndead->GetCheckState(), g_pMachines->GetCheckState(), g_pSlimes->GetCheckState(), g_pCritters->GetCheckState(), g_pTreasure->GetCheckState(), g_pObjects->GetCheckState(), g_pHerbs->GetCheckState(), g_pMinerals->GetCheckState());
				Settings_SaveTracks();
				return TRUE;
			}
			break;
		case IDC_DEMONS:
			if(wCode == BN_CLICKED)
			{
				Tracking_SetTrackData(g_pAlliance->GetCheckState(), g_pHorde->GetCheckState(), g_pBeasts->GetCheckState(), g_pDemons->GetCheckState(), g_pDragons->GetCheckState(), g_pElementals->GetCheckState(), g_pGiants->GetCheckState(), g_pHumanoids->GetCheckState(), g_pUndead->GetCheckState(), g_pMachines->GetCheckState(), g_pSlimes->GetCheckState(), g_pCritters->GetCheckState(), g_pTreasure->GetCheckState(), g_pObjects->GetCheckState(), g_pHerbs->GetCheckState(), g_pMinerals->GetCheckState());
				Settings_SaveTracks();
				return TRUE;
			}
			break;
		case IDC_DRAGONS:
			if(wCode == BN_CLICKED)
			{
				Tracking_SetTrackData(g_pAlliance->GetCheckState(), g_pHorde->GetCheckState(), g_pBeasts->GetCheckState(), g_pDemons->GetCheckState(), g_pDragons->GetCheckState(), g_pElementals->GetCheckState(), g_pGiants->GetCheckState(), g_pHumanoids->GetCheckState(), g_pUndead->GetCheckState(), g_pMachines->GetCheckState(), g_pSlimes->GetCheckState(), g_pCritters->GetCheckState(), g_pTreasure->GetCheckState(), g_pObjects->GetCheckState(), g_pHerbs->GetCheckState(), g_pMinerals->GetCheckState());
				Settings_SaveTracks();
				return TRUE;
			}
			break;
		case IDC_ELEMENTALS:
			if(wCode == BN_CLICKED)
			{
				Tracking_SetTrackData(g_pAlliance->GetCheckState(), g_pHorde->GetCheckState(), g_pBeasts->GetCheckState(), g_pDemons->GetCheckState(), g_pDragons->GetCheckState(), g_pElementals->GetCheckState(), g_pGiants->GetCheckState(), g_pHumanoids->GetCheckState(), g_pUndead->GetCheckState(), g_pMachines->GetCheckState(), g_pSlimes->GetCheckState(), g_pCritters->GetCheckState(), g_pTreasure->GetCheckState(), g_pObjects->GetCheckState(), g_pHerbs->GetCheckState(), g_pMinerals->GetCheckState());
				Settings_SaveTracks();
				return TRUE;
			}
			break;
		case IDC_GIANTS:
			if(wCode == BN_CLICKED)
			{
				Tracking_SetTrackData(g_pAlliance->GetCheckState(), g_pHorde->GetCheckState(), g_pBeasts->GetCheckState(), g_pDemons->GetCheckState(), g_pDragons->GetCheckState(), g_pElementals->GetCheckState(), g_pGiants->GetCheckState(), g_pHumanoids->GetCheckState(), g_pUndead->GetCheckState(), g_pMachines->GetCheckState(), g_pSlimes->GetCheckState(), g_pCritters->GetCheckState(), g_pTreasure->GetCheckState(), g_pObjects->GetCheckState(), g_pHerbs->GetCheckState(), g_pMinerals->GetCheckState());
				Settings_SaveTracks();
				return TRUE;
			}
			break;
		case IDC_HUMANOIDS:
			if(wCode == BN_CLICKED)
			{
				Tracking_SetTrackData(g_pAlliance->GetCheckState(), g_pHorde->GetCheckState(), g_pBeasts->GetCheckState(), g_pDemons->GetCheckState(), g_pDragons->GetCheckState(), g_pElementals->GetCheckState(), g_pGiants->GetCheckState(), g_pHumanoids->GetCheckState(), g_pUndead->GetCheckState(), g_pMachines->GetCheckState(), g_pSlimes->GetCheckState(), g_pCritters->GetCheckState(), g_pTreasure->GetCheckState(), g_pObjects->GetCheckState(), g_pHerbs->GetCheckState(), g_pMinerals->GetCheckState());
				Settings_SaveTracks();
				return TRUE;
			}
			break;
		case IDC_UNDEAD:
			if(wCode == BN_CLICKED)
			{
				Tracking_SetTrackData(g_pAlliance->GetCheckState(), g_pHorde->GetCheckState(), g_pBeasts->GetCheckState(), g_pDemons->GetCheckState(), g_pDragons->GetCheckState(), g_pElementals->GetCheckState(), g_pGiants->GetCheckState(), g_pHumanoids->GetCheckState(), g_pUndead->GetCheckState(), g_pMachines->GetCheckState(), g_pSlimes->GetCheckState(), g_pCritters->GetCheckState(), g_pTreasure->GetCheckState(), g_pObjects->GetCheckState(), g_pHerbs->GetCheckState(), g_pMinerals->GetCheckState());
				Settings_SaveTracks();
				return TRUE;
			}
			break;
		case IDC_MACHINES:
			if(wCode == BN_CLICKED)
			{
				Tracking_SetTrackData(g_pAlliance->GetCheckState(), g_pHorde->GetCheckState(), g_pBeasts->GetCheckState(), g_pDemons->GetCheckState(), g_pDragons->GetCheckState(), g_pElementals->GetCheckState(), g_pGiants->GetCheckState(), g_pHumanoids->GetCheckState(), g_pUndead->GetCheckState(), g_pMachines->GetCheckState(), g_pSlimes->GetCheckState(), g_pCritters->GetCheckState(), g_pTreasure->GetCheckState(), g_pObjects->GetCheckState(), g_pHerbs->GetCheckState(), g_pMinerals->GetCheckState());
				Settings_SaveTracks();
				return TRUE;
			}
			break;
		case IDC_SLIMES:
			if(wCode == BN_CLICKED)
			{
				Tracking_SetTrackData(g_pAlliance->GetCheckState(), g_pHorde->GetCheckState(), g_pBeasts->GetCheckState(), g_pDemons->GetCheckState(), g_pDragons->GetCheckState(), g_pElementals->GetCheckState(), g_pGiants->GetCheckState(), g_pHumanoids->GetCheckState(), g_pUndead->GetCheckState(), g_pMachines->GetCheckState(), g_pSlimes->GetCheckState(), g_pCritters->GetCheckState(), g_pTreasure->GetCheckState(), g_pObjects->GetCheckState(), g_pHerbs->GetCheckState(), g_pMinerals->GetCheckState());
				Settings_SaveTracks();
				return TRUE;
			}
			break;
		case IDC_CRITTERS:
			if(wCode == BN_CLICKED)
			{
				Tracking_SetTrackData(g_pAlliance->GetCheckState(), g_pHorde->GetCheckState(), g_pBeasts->GetCheckState(), g_pDemons->GetCheckState(), g_pDragons->GetCheckState(), g_pElementals->GetCheckState(), g_pGiants->GetCheckState(), g_pHumanoids->GetCheckState(), g_pUndead->GetCheckState(), g_pMachines->GetCheckState(), g_pSlimes->GetCheckState(), g_pCritters->GetCheckState(), g_pTreasure->GetCheckState(), g_pObjects->GetCheckState(), g_pHerbs->GetCheckState(), g_pMinerals->GetCheckState());
				Settings_SaveTracks();
				return TRUE;
			}
			break;
		case IDC_TREASURE:
			if(wCode == BN_CLICKED)
			{
				Tracking_SetTrackData(g_pAlliance->GetCheckState(), g_pHorde->GetCheckState(), g_pBeasts->GetCheckState(), g_pDemons->GetCheckState(), g_pDragons->GetCheckState(), g_pElementals->GetCheckState(), g_pGiants->GetCheckState(), g_pHumanoids->GetCheckState(), g_pUndead->GetCheckState(), g_pMachines->GetCheckState(), g_pSlimes->GetCheckState(), g_pCritters->GetCheckState(), g_pTreasure->GetCheckState(), g_pObjects->GetCheckState(), g_pHerbs->GetCheckState(), g_pMinerals->GetCheckState());
				Settings_SaveTracks();
				return TRUE;
			}
			break;
		case IDC_OBJECTS:
			if(wCode == BN_CLICKED)
			{
				Tracking_SetTrackData(g_pAlliance->GetCheckState(), g_pHorde->GetCheckState(), g_pBeasts->GetCheckState(), g_pDemons->GetCheckState(), g_pDragons->GetCheckState(), g_pElementals->GetCheckState(), g_pGiants->GetCheckState(), g_pHumanoids->GetCheckState(), g_pUndead->GetCheckState(), g_pMachines->GetCheckState(), g_pSlimes->GetCheckState(), g_pCritters->GetCheckState(), g_pTreasure->GetCheckState(), g_pObjects->GetCheckState(), g_pHerbs->GetCheckState(), g_pMinerals->GetCheckState());
				Settings_SaveTracks();
				return TRUE;
			}
			break;
		case IDC_HERBS:
			if(wCode == BN_CLICKED)
			{
				Tracking_SetTrackData(g_pAlliance->GetCheckState(), g_pHorde->GetCheckState(), g_pBeasts->GetCheckState(), g_pDemons->GetCheckState(), g_pDragons->GetCheckState(), g_pElementals->GetCheckState(), g_pGiants->GetCheckState(), g_pHumanoids->GetCheckState(), g_pUndead->GetCheckState(), g_pMachines->GetCheckState(), g_pSlimes->GetCheckState(), g_pCritters->GetCheckState(), g_pTreasure->GetCheckState(), g_pObjects->GetCheckState(), g_pHerbs->GetCheckState(), g_pMinerals->GetCheckState());
				Settings_SaveTracks();
				return TRUE;
			}
			break;
		case IDC_MINERALS:
			if(wCode == BN_CLICKED)
			{
				Tracking_SetTrackData(g_pAlliance->GetCheckState(), g_pHorde->GetCheckState(), g_pBeasts->GetCheckState(), g_pDemons->GetCheckState(), g_pDragons->GetCheckState(), g_pElementals->GetCheckState(), g_pGiants->GetCheckState(), g_pHumanoids->GetCheckState(), g_pUndead->GetCheckState(), g_pMachines->GetCheckState(), g_pSlimes->GetCheckState(), g_pCritters->GetCheckState(), g_pTreasure->GetCheckState(), g_pObjects->GetCheckState(), g_pHerbs->GetCheckState(), g_pMinerals->GetCheckState());
				Settings_SaveTracks();
				return TRUE;
			}
			break;
		}
		break;

	////////////////
	// WM_KEYDOWN //
	////////////////
	case WM_KEYDOWN:
		if(wParam == VK_CONTROL)
		{
			g_bCtrlDown = TRUE;
		}
		if(wParam == VK_MENU)
		{
			g_bAltDown = TRUE;
		}
		wParam = wParam | (g_bCtrlDown ? 0x80000000 : 0) | (g_bAltDown ? 0x40000000 : 0);
		if(wParam == g_Settings.kShowHide)
		{
			if(g_pMainWindow->IsVisible())
			{
				g_pMainWindow->Hide();
				g_pD3DManager->SetFocusedControl(NULL);
			}
			else
				g_pMainWindow->Show();
			bRet = TRUE;
		}
		if(g_bDisableHotkeys == FALSE)
		{
			if(wParam == g_Settings.kXMinus)
			{
				if(g_bLeftButtonDown == FALSE)
					Teleport_ShiftAxis(AXIS_X, -1); 
				g_dwDirection |= GO_LEFT;
				bRet = TRUE;
			}
			if(wParam == g_Settings.kXPlus)
			{
				if(g_bLeftButtonDown == FALSE)
					Teleport_ShiftAxis(AXIS_X, 1); 
				g_dwDirection |= GO_RIGHT;
				bRet = TRUE;
			}
			if(wParam == g_Settings.kYMinus)
			{
				if(g_bLeftButtonDown == FALSE)
					Teleport_ShiftAxis(AXIS_Y, -1); 
				g_dwDirection |= GO_BACKWARD;
				bRet = TRUE;
			}
			if(wParam == g_Settings.kYPlus)
			{
				if(g_bLeftButtonDown == FALSE)
					Teleport_ShiftAxis(AXIS_Y, 1); 
				g_dwDirection |= GO_FORWARD;
				bRet = TRUE;
			}
			if(wParam == g_Settings.kZMinus)
			{
				if(g_bLeftButtonDown == FALSE)
					Teleport_ShiftAxis(AXIS_Z, -1); 
				g_dwDirection |= GO_DOWN;
				bRet = TRUE;
			}
			if(wParam == g_Settings.kZPlus)
			{
				if(g_bLeftButtonDown == FALSE)
					Teleport_ShiftAxis(AXIS_Z, 1); 
				g_dwDirection |= GO_UP;
				bRet = TRUE;
			}
			if(wParam == g_Settings.kSpeedMinus)
			{
				g_pSEdit->KillFocus(hwnd);
				Teleport_ChangeSpeed(METHOD_ADD, -1);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kSpeedPlus)
			{
				g_pSEdit->KillFocus(hwnd);
				Teleport_ChangeSpeed(METHOD_ADD, 1);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kNormalSpeed)
			{
				g_pSEdit->KillFocus(hwnd);
				Teleport_ChangeSpeed(METHOD_SET, 7);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kTeleport)
			{
				if(ListManager_GetSelectedLocation(&loc.dwMap, &loc.fX, &loc.fY, &loc.fZ))
				{
					Teleport_Teleport(&loc);
				}
				bRet = TRUE;
			}
			if(wParam == g_Settings.kQuickSave)
			{
				Teleport_GetCurrentLocation(&g_locQuickSave);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kQuickLoad)
			{
				Teleport_Teleport(&g_locQuickSave);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kTrackAlliance)
			{
				if(g_pAlliance->GetCheckState())
					g_pAlliance->SetCheckState(FALSE);
				else
					g_pAlliance->SetCheckState(TRUE);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_ALLIANCE, BN_CLICKED), (LPARAM)g_pAlliance);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kTrackHorde)
			{
				if(g_pHorde->GetCheckState())
					g_pHorde->SetCheckState(FALSE);
				else
					g_pHorde->SetCheckState(TRUE);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_HORDE, BN_CLICKED), (LPARAM)g_pHorde);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kTrackBeasts)
			{
				if(g_pBeasts->GetCheckState())
					g_pBeasts->SetCheckState(FALSE);
				else
					g_pBeasts->SetCheckState(TRUE);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_BEASTS, BN_CLICKED), (LPARAM)g_pBeasts);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kTrackDemons)
			{
				if(g_pDemons->GetCheckState())
					g_pDemons->SetCheckState(FALSE);
				else
					g_pDemons->SetCheckState(TRUE);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_DEMONS, BN_CLICKED), (LPARAM)g_pDemons);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kTrackDragons)
			{
				if(g_pDragons->GetCheckState())
					g_pDragons->SetCheckState(FALSE);
				else
					g_pDragons->SetCheckState(TRUE);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_DRAGONS, BN_CLICKED), (LPARAM)g_pDragons);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kTrackElementals)
			{
				if(g_pElementals->GetCheckState())
					g_pElementals->SetCheckState(FALSE);
				else
					g_pElementals->SetCheckState(TRUE);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_ELEMENTALS, BN_CLICKED), (LPARAM)g_pElementals);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kTrackGiants)
			{
				if(g_pGiants->GetCheckState())
					g_pGiants->SetCheckState(FALSE);
				else
					g_pGiants->SetCheckState(TRUE);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_GIANTS, BN_CLICKED), (LPARAM)g_pGiants);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kTrackHumanoids)
			{
				if(g_pHumanoids->GetCheckState())
					g_pHumanoids->SetCheckState(FALSE);
				else
					g_pHumanoids->SetCheckState(TRUE);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_HUMANOIDS, BN_CLICKED), (LPARAM)g_pHumanoids);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kTrackUndead)
			{
				if(g_pUndead->GetCheckState())
					g_pUndead->SetCheckState(FALSE);
				else
					g_pUndead->SetCheckState(TRUE);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_UNDEAD, BN_CLICKED), (LPARAM)g_pUndead);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kTrackMachines)
			{
				if(g_pMachines->GetCheckState())
					g_pMachines->SetCheckState(FALSE);
				else
					g_pMachines->SetCheckState(TRUE);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_MACHINES, BN_CLICKED), (LPARAM)g_pMachines);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kTrackSlimes)
			{
				if(g_pSlimes->GetCheckState())
					g_pSlimes->SetCheckState(FALSE);
				else
					g_pSlimes->SetCheckState(TRUE);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_SLIMES, BN_CLICKED), (LPARAM)g_pSlimes);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kTrackCritters)
			{
				if(g_pCritters->GetCheckState())
					g_pCritters->SetCheckState(FALSE);
				else
					g_pCritters->SetCheckState(TRUE);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_CRITTERS, BN_CLICKED), (LPARAM)g_pCritters);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kTrackTreasure)
			{
				if(g_pTreasure->GetCheckState())
					g_pTreasure->SetCheckState(FALSE);
				else
					g_pTreasure->SetCheckState(TRUE);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_TREASURE, BN_CLICKED), (LPARAM)g_pTreasure);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kTrackObjects)
			{
				if(g_pObjects->GetCheckState())
					g_pObjects->SetCheckState(FALSE);
				else
					g_pObjects->SetCheckState(TRUE);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_OBJECTS, BN_CLICKED), (LPARAM)g_pObjects);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kTrackHerbs)
			{
				if(g_pHerbs->GetCheckState())
					g_pHerbs->SetCheckState(FALSE);
				else
					g_pHerbs->SetCheckState(TRUE);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_HERBS, BN_CLICKED), (LPARAM)g_pHerbs);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kTrackMinerals)
			{
				if(g_pMinerals->GetCheckState())
					g_pMinerals->SetCheckState(FALSE);
				else
					g_pMinerals->SetCheckState(TRUE);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_MINERALS, BN_CLICKED), (LPARAM)g_pMinerals);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kNoFallDamage)
			{
				if(g_pNoFallDamage->GetCheckState())
					g_pNoFallDamage->SetCheckState(FALSE);
				else
					g_pNoFallDamage->SetCheckState(TRUE);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_NOFALLDAMAGE, BN_CLICKED), (LPARAM)g_pNoFallDamage);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kMountainClimb)
			{
				if(g_pMountainClimb->GetCheckState())
					g_pMountainClimb->SetCheckState(FALSE);
				else
					g_pMountainClimb->SetCheckState(TRUE);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_MOUNTAINCLIMB, BN_CLICKED), (LPARAM)g_pMountainClimb);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kLockSpeed)
			{
				if(g_pLockSpeed->GetCheckState())
					g_pLockSpeed->SetCheckState(FALSE);
				else
					g_pLockSpeed->SetCheckState(TRUE);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_LOCKSPEED, BN_CLICKED), (LPARAM)g_pLockSpeed);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kZeroGravity)
			{
				if(g_pZeroGravity->GetCheckState())
					g_pZeroGravity->SetCheckState(FALSE);
				else
					g_pZeroGravity->SetCheckState(TRUE);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_ZEROGRAVITY, BN_CLICKED), (LPARAM)g_pZeroGravity);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kTeleportToPlane)
			{
				if(g_pTeleportToPlane->GetCheckState())
					g_pTeleportToPlane->SetCheckState(FALSE);
				else
					g_pTeleportToPlane->SetCheckState(TRUE);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_TELEPORTTOPLANE, BN_CLICKED), (LPARAM)g_pTeleportToPlane);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kFollowNpc)
			{
				if(g_pFollowNPC->GetCheckState())
					g_pFollowNPC->SetCheckState(FALSE);
				else
					g_pFollowNPC->SetCheckState(TRUE);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_FOLLOWNPC, BN_CLICKED), (LPARAM)g_pFollowNPC);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kGoToCorpse)
			{
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_GOTOCORPSE, BN_CLICKED), (LPARAM)g_pGoToCorpse);
				bRet = TRUE;
			}
			if(wParam == g_Settings.kGPS)
			{
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_GPS, BN_CLICKED), (LPARAM)g_pGPS);
				bRet = TRUE;
			}
		}
		break;

	//////////////
	// WM_KEYUP //
	//////////////
	case WM_KEYUP:
		if(g_bDisableHotkeys == FALSE)
		{
			if(wParam == VK_CONTROL)
			{
				if(GetAsyncKeyState(VK_MENU) >= 0)
					g_bAltDown = FALSE;
				g_bCtrlDown = FALSE;
			}
			if(wParam == VK_MENU)
			{
				if(GetAsyncKeyState(VK_CONTROL) >= 0)
					g_bCtrlDown = FALSE;
				g_bAltDown = FALSE;
			}
			wParam = wParam | (g_bCtrlDown ? 0x80000000 : 0) | (g_bAltDown ? 0x40000000 : 0);
			if(wParam == g_Settings.kXMinus)
			{
				g_dwDirection &= ~GO_LEFT;
				return TRUE;
			}
			if(wParam == g_Settings.kXPlus)
			{
				g_dwDirection &= ~GO_RIGHT;
				return TRUE;
			}
			if(wParam == g_Settings.kYMinus)
			{
				g_dwDirection &= ~GO_BACKWARD;
				return TRUE;
			}
			if(wParam == g_Settings.kYPlus)
			{
				g_dwDirection &= ~GO_FORWARD;
				return TRUE;
			}
			if(wParam == g_Settings.kZMinus)
			{
				g_dwDirection &= ~GO_DOWN;
				return TRUE;
			}
			if(wParam == g_Settings.kZPlus)
			{
				g_dwDirection &= ~GO_UP;
				return TRUE;
			}
		}
		break;

	////////////////////
	// WM_LBUTTONDOWN //
	////////////////////
    case WM_LBUTTONDOWN:
        g_bLeftButtonDown = TRUE;
        if(g_Settings.CameraAngle)
            SetTimer(hwnd, IDT_CAMERASLEW, 10, NULL);
        break;

	//////////////////
	// WM_LBUTTONUP //
	//////////////////
    case WM_LBUTTONUP:
        g_bLeftButtonDown = FALSE;
        KillTimer(hwnd, IDT_CAMERASLEW);
        break;

	//////////////
	// WM_TIMER //
	//////////////
	case WM_TIMER:
		if(wParam == IDT_UPDATE)
		{
			if(Teleport_GetCurrentLocation(&loc))
			{
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_ZEROGRAVITY, BN_CLICKED), (LPARAM)g_pZeroGravity);
			}
			pFocusedControl = g_pD3DManager->GetFocusedControl();

			sprintf(szBuff, "%.3f", loc.fX);
			g_pGPSXEdit->SetText(szBuff);
			if(pFocusedControl != g_pXEdit)
				g_pXEdit->SetText(szBuff);
			
			sprintf(szBuff, "%.3f", loc.fY);
			g_pGPSYEdit->SetText(szBuff);
			if(pFocusedControl != g_pYEdit)
				g_pYEdit->SetText(szBuff);

			sprintf(szBuff, "%.3f", loc.fZ);
			g_pGPSZEdit->SetText(szBuff);
			if(pFocusedControl != g_pZEdit)
				g_pZEdit->SetText(szBuff);

			sprintf(szBuff, "%.3f", loc.fSpeed);
			g_pGPSSEdit->SetText(szBuff);
			if(pFocusedControl != g_pSEdit)
				g_pSEdit->SetText(szBuff);

			sprintf(szBuff, "%d", loc.dwMap);
			g_pGPSMEdit->SetText(szBuff);
			return TRUE;
		}
		if(wParam == IDT_CAMERASLEW)
		{
			Teleport_CameraSlew(g_Settings.CameraAngle);
			return TRUE;
		}
		break;
	}
	return bRet;
}