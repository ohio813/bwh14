#include "settings.h"
#include "common.h"

char szINI[MAX_PATH];

// code start
VOID Settings_LoadGPSWindowPosition(PDWORD pdwPos)
{
	*pdwPos = GetPrivateProfileInt("SAVE", "GPSWindowPos", 0, szINI);
}

VOID Settings_LoadHacks(PDWORD pdwHacks)
{
	*pdwHacks = GetPrivateProfileInt("SAVE", "Hacks", 0, szINI);
}

BOOL Settings_LoadINI(HMODULE hModule, PSETTINGS pSettings)
{
	GetMyDirectory(hModule, szINI);
    strcat(szINI, "bwh.ini");

	pSettings->Present = GetPrivateProfileInt("D3D", "Present", NULL, szINI);
	pSettings->Reset = GetPrivateProfileInt("D3D", "Reset", NULL, szINI);

	pSettings->GetMap = UlongToPtr(GetPrivateProfileInt("BWH", "GetMap", NULL, szINI));
	pSettings->GetGuidData = UlongToPtr(GetPrivateProfileInt("BWH", "GetGuidData", NULL, szINI));
	pSettings->GetGuidDataConstant1 = GetPrivateProfileInt("BWH", "GetGuidDataConstant1", NULL, szINI);
	pSettings->GetGuidDataConstant2 = GetPrivateProfileInt("BWH", "GetGuidDataConstant2", NULL, szINI);
	pSettings->GetCharacterGuid = UlongToPtr(GetPrivateProfileInt("BWH", "GetCharacterGuid", NULL, szINI));
	pSettings->FallingTeleport = UlongToPtr(GetPrivateProfileInt("BWH", "FallingTeleport", NULL, szINI));
	pSettings->CameraAngle = UlongToPtr(GetPrivateProfileInt("BWH", "CameraAngle", NULL, szINI));
	pSettings->SendChatMessage = UlongToPtr(GetPrivateProfileInt("BWH", "SendChatMessage", NULL, szINI));
	pSettings->TrackingHook = (PDWORD)UlongToPtr(GetPrivateProfileInt("BWH", "TrackingHook", NULL, szINI));
	pSettings->TrackingJumpSize = GetPrivateProfileInt("BWH", "TrackingJumpSize", NULL, szINI);
	pSettings->TrackingTrackStructOffset = GetPrivateProfileInt("BWH", "TrackingTrackStructOffset", NULL, szINI);
	pSettings->TrackingTrackDataOffset = GetPrivateProfileInt("BWH", "TrackingTrackDataOffset", NULL, szINI);
	pSettings->TrackingCharStruct2Offset = GetPrivateProfileInt("BWH", "TrackingCharStruct2Offset", NULL, szINI);
	pSettings->TrackingCharTypeOffset = GetPrivateProfileInt("BWH", "TrackingCharTypeOffset", NULL, szINI);
	pSettings->TrackingNpcDataOffset = GetPrivateProfileInt("BWH", "TrackingNpcDataOffset", NULL, szINI);
	pSettings->NameLookup = UlongToPtr(GetPrivateProfileInt("BWH", "NameLookup", NULL, szINI));
	pSettings->Corpse = (PFLOAT)UlongToPtr(GetPrivateProfileInt("BWH", "Corpse", NULL, szINI));
	pSettings->NoFallDamageHook = (PDWORD)UlongToPtr(GetPrivateProfileInt("BWH", "NoFallDamageHook", NULL, szINI));
	pSettings->NoFallDamageOffset = GetPrivateProfileInt("BWH", "NoFallDamageOffset", NULL, szINI);
	pSettings->ZeroGravity = (PDWORD)UlongToPtr(GetPrivateProfileInt("BWH", "ZeroGravity", NULL, szINI));
	pSettings->MountainClimb = (PDWORD)UlongToPtr(GetPrivateProfileInt("BWH", "MountainClimb", NULL, szINI));
	pSettings->LockSpeed = (PBYTE)UlongToPtr(GetPrivateProfileInt("BWH", "LockSpeed", NULL, szINI));
	pSettings->TeleportToPlane = (PBYTE)UlongToPtr(GetPrivateProfileInt("BWH", "TeleportToPlane", NULL, szINI));
	pSettings->FollowNpc1 = (PBYTE)UlongToPtr(GetPrivateProfileInt("BWH", "FollowNpc1", NULL, szINI));
	pSettings->FollowNpc2 = (PBYTE)UlongToPtr(GetPrivateProfileInt("BWH", "FollowNpc2", NULL, szINI));
	pSettings->FollowFaction = (PBYTE)UlongToPtr(GetPrivateProfileInt("BWH", "FollowFaction", NULL, szINI));
	pSettings->FollowDistance = (PFLOAT)UlongToPtr(GetPrivateProfileInt("BWH", "FollowDistance", NULL, szINI));

	pSettings->kShowHide = GetPrivateProfileInt("HOTKEYS", "ShowHide", NULL, szINI);
	pSettings->kXPlus = GetPrivateProfileInt("HOTKEYS", "X-Plus", NULL, szINI);
	pSettings->kXMinus = GetPrivateProfileInt("HOTKEYS", "X-Minus", NULL, szINI);
	pSettings->kYPlus = GetPrivateProfileInt("HOTKEYS", "Y-Plus", NULL, szINI);
	pSettings->kYMinus = GetPrivateProfileInt("HOTKEYS", "Y-Minus", NULL, szINI);
	pSettings->kZPlus = GetPrivateProfileInt("HOTKEYS", "Z-Plus", NULL, szINI);
	pSettings->kZMinus = GetPrivateProfileInt("HOTKEYS", "Z-Minus", NULL, szINI);
	pSettings->kSpeedPlus = GetPrivateProfileInt("HOTKEYS", "Speed-Plus", NULL, szINI);
	pSettings->kSpeedMinus = GetPrivateProfileInt("HOTKEYS", "Speed-Minus", NULL, szINI);
	pSettings->kNormalSpeed = GetPrivateProfileInt("HOTKEYS", "NormalSpeed", NULL, szINI);
	pSettings->kTeleport = GetPrivateProfileInt("HOTKEYS", "Teleport", NULL, szINI);
	pSettings->kQuickSave = GetPrivateProfileInt("HOTKEYS", "QuickSave", NULL, szINI);
	pSettings->kQuickLoad = GetPrivateProfileInt("HOTKEYS", "QuickLoad", NULL, szINI);
	pSettings->kTrackAlliance = GetPrivateProfileInt("HOTKEYS", "TrackAlliance", NULL, szINI);
	pSettings->kTrackHorde = GetPrivateProfileInt("HOTKEYS", "TrackHorde", NULL, szINI);
	pSettings->kTrackBeasts = GetPrivateProfileInt("HOTKEYS", "TrackBeasts", NULL, szINI);
	pSettings->kTrackDemons = GetPrivateProfileInt("HOTKEYS", "TrackDemons", NULL, szINI);
	pSettings->kTrackDragons = GetPrivateProfileInt("HOTKEYS", "TrackDragons", NULL, szINI);
	pSettings->kTrackElementals = GetPrivateProfileInt("HOTKEYS", "TrackElementals", NULL, szINI);
	pSettings->kTrackGiants = GetPrivateProfileInt("HOTKEYS", "TrackGiants", NULL, szINI);
	pSettings->kTrackHumanoids = GetPrivateProfileInt("HOTKEYS", "TrackHumanoids", NULL, szINI);
	pSettings->kTrackUndead = GetPrivateProfileInt("HOTKEYS", "TrackUndead", NULL, szINI);
	pSettings->kTrackMachines = GetPrivateProfileInt("HOTKEYS", "TrackMachines", NULL, szINI);
	pSettings->kTrackSlimes = GetPrivateProfileInt("HOTKEYS", "TrackSlimes", NULL, szINI);
	pSettings->kTrackCritters = GetPrivateProfileInt("HOTKEYS", "TrackCritters", NULL, szINI);
	pSettings->kTrackTreasure = GetPrivateProfileInt("HOTKEYS", "TrackTreasure", NULL, szINI);
	pSettings->kTrackObjects = GetPrivateProfileInt("HOTKEYS", "TrackObjects", NULL, szINI);
	pSettings->kTrackHerbs = GetPrivateProfileInt("HOTKEYS", "TrackHerbs", NULL, szINI);
	pSettings->kTrackMinerals = GetPrivateProfileInt("HOTKEYS", "TrackMinerals", NULL, szINI);
	pSettings->kNoFallDamage = GetPrivateProfileInt("HOTKEYS", "NoFallDamage", NULL, szINI);
	pSettings->kMountainClimb = GetPrivateProfileInt("HOTKEYS", "MountainClimb", NULL, szINI);
	pSettings->kLockSpeed = GetPrivateProfileInt("HOTKEYS", "LockSpeed", NULL, szINI);
	pSettings->kZeroGravity = GetPrivateProfileInt("HOTKEYS", "ZeroGravity", NULL, szINI);
	pSettings->kTeleportToPlane = GetPrivateProfileInt("HOTKEYS", "TeleportToPlane", NULL, szINI);
	pSettings->kFollowNpc = GetPrivateProfileInt("HOTKEYS", "FollowNpc", NULL, szINI);
	pSettings->kGoToCorpse = GetPrivateProfileInt("HOTKEYS", "GoToCorpse", NULL, szINI);
	pSettings->kGPS = GetPrivateProfileInt("HOTKEYS", "GPS", NULL, szINI);

	GetPrivateProfileString("FONTS", "FixedFont", "Terminal", pSettings->FixedFont, 100, szINI);
	pSettings->FixedSize = (INT)GetPrivateProfileInt("FONTS", "FixedSize", 9, szINI);
	pSettings->FixedBold = (BOOL)GetPrivateProfileInt("FONTS", "FixedBold", FALSE, szINI);
	GetPrivateProfileString("FONTS", "ProportionalFont", "Arial", pSettings->ProportionalFont, 100, szINI);
	pSettings->ProportionalSize = (INT)GetPrivateProfileInt("FONTS", "ProportionalSize", 9, szINI);
	pSettings->ProportionalBold = (BOOL)GetPrivateProfileInt("FONTS", "ProportionalBold", FALSE, szINI);

	if( pSettings->Present &&
	    pSettings->Reset)
	    return TRUE;
	else
		return FALSE;
}

VOID Settings_LoadLocationFile(PSZ pszLocationFile)
{
	GetPrivateProfileString("SAVE", "LocationFile", "My Locations", pszLocationFile, 21, szINI);
}

VOID Settings_LoadMainWindowPosition(PDWORD pdwPos)
{
	*pdwPos = GetPrivateProfileInt("SAVE", "MainWindowPos", 0, szINI);
}

VOID Settings_LoadTrackPattern(PSZ pszTrackPattern)
{
	GetPrivateProfileString("SAVE", "TrackPattern", "", pszTrackPattern, 27, szINI);
}

VOID Settings_LoadTracks(PDWORD pdwTracks)
{
	*pdwTracks = GetPrivateProfileInt("SAVE", "Tracks", 0, szINI);
}

VOID Settings_SaveGPSWindowPosition(DWORD dwPos)
{
	char szBuff[11];
	WritePrivateProfileString("SAVE", "GPSWindowPos", itoa(dwPos, szBuff, 10), szINI);
}

VOID Settings_SaveHacks()
{
	char szBuff[11];
	DWORD dwHacks = 0;
	if(g_pDisableHotkeys->GetCheckState())
		dwHacks |= 1;
	if(g_pNoFallDamage->GetCheckState())
		dwHacks |= 2;
	if(g_pMountainClimb->GetCheckState())
		dwHacks |= 4;
	if(g_pLockSpeed->GetCheckState())
		dwHacks |= 8;
	if(g_pZeroGravity->GetCheckState())
		dwHacks |= 16;
	if(g_pTeleportToPlane->GetCheckState())
		dwHacks |= 32;
	if(g_pFollowNPC->GetCheckState())
		dwHacks |= 64;
	WritePrivateProfileString("SAVE", "Hacks", itoa(dwHacks, szBuff, 10), szINI);
}

VOID Settings_SaveLocationFile()
{
	char szLocationFile[21];
	g_pFileEdit->GetText(szLocationFile, sizeof(szLocationFile));
	WritePrivateProfileString("SAVE", "LocationFile", szLocationFile, szINI);
}

VOID Settings_SaveMainWindowPosition(DWORD dwPos)
{
	char szBuff[11];
	WritePrivateProfileString("SAVE", "MainWindowPos", itoa(dwPos, szBuff, 10), szINI);
}

VOID Settings_SaveTrackPattern()
{
	char szTrackPattern[29];
	g_pTrackEdit->GetText(szTrackPattern, sizeof(szTrackPattern));
	WritePrivateProfileString("SAVE", "TrackPattern", szTrackPattern, szINI);
}

VOID Settings_SaveTracks()
{
	char szBuff[11];
	DWORD dwTracks = 0;
	if(g_pAlliance->GetCheckState())
		dwTracks |= 1;
	if(g_pHorde->GetCheckState())
		dwTracks |= 2;
	if(g_pBeasts->GetCheckState())
		dwTracks |= 4;
	if(g_pDemons->GetCheckState())
		dwTracks |= 8;
	if(g_pDragons->GetCheckState())
		dwTracks |= 16;
	if(g_pElementals->GetCheckState())
		dwTracks |= 32;
	if(g_pGiants->GetCheckState())
		dwTracks |= 64;
	if(g_pHumanoids->GetCheckState())
		dwTracks |= 128;
	if(g_pUndead->GetCheckState())
		dwTracks |= 256;
	if(g_pMachines->GetCheckState())
		dwTracks |= 512;
	if(g_pSlimes->GetCheckState())
		dwTracks |= 1024;
	if(g_pCritters->GetCheckState())
		dwTracks |= 2048;
	if(g_pTreasure->GetCheckState())
		dwTracks |= 4096;
	if(g_pObjects->GetCheckState())
		dwTracks |= 8192;
	if(g_pHerbs->GetCheckState())
		dwTracks |= 16384;
	if(g_pMinerals->GetCheckState())
		dwTracks |= 32768;
	WritePrivateProfileString("SAVE", "Tracks", itoa(dwTracks, szBuff, 10), szINI);
}
