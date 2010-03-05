#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <windows.h>

typedef struct _SETTINGS
{
	DWORD  Present;
	DWORD  Reset;

	PVOID  GetMap;
	PVOID  GetGuidData;
	DWORD  GetGuidDataConstant1;
	DWORD  GetGuidDataConstant2;
	PVOID  GetCharacterGuid;
	PVOID  FallingTeleport;
	PVOID  CameraAngle;
	PVOID  SendChatMessage;
	PDWORD TrackingHook;
	DWORD  TrackingJumpSize;
	DWORD  TrackingTrackStructOffset;
	DWORD  TrackingTrackDataOffset;
	DWORD  TrackingCharStruct2Offset;
	DWORD  TrackingCharTypeOffset;
	DWORD  TrackingNpcDataOffset;
	PVOID  NameLookup;
	PFLOAT Corpse;
	PDWORD NoFallDamageHook;
	DWORD  NoFallDamageOffset;
	PDWORD ZeroGravity;
	PDWORD MountainClimb;
	PBYTE  LockSpeed;
	PBYTE  TeleportToPlane;
	PBYTE  FollowNpc1;
	PBYTE  FollowNpc2;
	PBYTE  FollowFaction;
	PFLOAT FollowDistance;

	DWORD kShowHide;
	DWORD kXPlus;
	DWORD kXMinus;
	DWORD kYPlus;
	DWORD kYMinus;
	DWORD kZPlus;
	DWORD kZMinus;
	DWORD kSpeedPlus;
	DWORD kSpeedMinus;
	DWORD kNormalSpeed;
	DWORD kTeleport;
	DWORD kQuickSave;
	DWORD kQuickLoad;
	DWORD kTrackAlliance;
	DWORD kTrackHorde;
	DWORD kTrackBeasts;
	DWORD kTrackDemons;
	DWORD kTrackDragons;
	DWORD kTrackElementals;
	DWORD kTrackGiants;
	DWORD kTrackHumanoids;
	DWORD kTrackUndead;
	DWORD kTrackMachines;
	DWORD kTrackSlimes;
	DWORD kTrackCritters;
	DWORD kTrackTreasure;
	DWORD kTrackObjects;
	DWORD kTrackHerbs;
	DWORD kTrackMinerals;
	DWORD kNoFallDamage;
	DWORD kMountainClimb;
	DWORD kLockSpeed;
	DWORD kZeroGravity;
	DWORD kTeleportToPlane;
	DWORD kFollowNpc;
	DWORD kGoToCorpse;
	DWORD kGPS;

	CHAR  FixedFont[100];
	INT   FixedSize;
	BOOL  FixedBold;
	CHAR  ProportionalFont[100];
	INT   ProportionalSize;
	BOOL  ProportionalBold;
} SETTINGS, *PSETTINGS;

VOID Settings_LoadGPSWindowPosition(PDWORD pdwPos);
VOID Settings_LoadHacks(PDWORD pdwHacks);
BOOL Settings_LoadINI(HMODULE hModule, PSETTINGS pSettings);
VOID Settings_LoadLocationFile(PSZ pszLocationFile);
VOID Settings_LoadMainWindowPosition(PDWORD pdwPos);
VOID Settings_LoadTrackPattern(PSZ pszTrackPattern);
VOID Settings_LoadTracks(PDWORD pdwTracks);
VOID Settings_SaveGPSWindowPosition(DWORD dwPos);
VOID Settings_SaveHacks();
VOID Settings_SaveLocationFile();
VOID Settings_SaveMainWindowPosition(DWORD dwPos);
VOID Settings_SaveTrackPattern();
VOID Settings_SaveTracks();

#endif