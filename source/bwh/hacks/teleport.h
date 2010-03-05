#ifndef _TELEPORT_H
#define _TELEPORT_H

#include <windows.h>

typedef struct _LOCATION
{
	DWORD dwMap;
	FLOAT fX;
	FLOAT fY;
	FLOAT fZ;
	FLOAT fSpeed;
} LOCATION, *PLOCATION;

typedef enum _AXIS
{
	AXIS_X,
	AXIS_Y,
	AXIS_Z
} AXIS;

typedef enum _METHOD
{
	METHOD_ADD,
	METHOD_SET
} METHOD;

typedef struct _CHARBLOCK
{
    DWORD dwUnknown1[586];
    FLOAT y;
    FLOAT x;
    FLOAT z;
    DWORD dwUnknown2[9];
    DWORD state;
    DWORD dwUnknown3[17];
	FLOAT walkspeed;
    FLOAT runspeed;
    FLOAT backwardspeed;
    FLOAT swimspeed;
	FLOAT backwardswimspeed;
} CHARBLOCK, *PCHARBLOCK;

typedef struct _CHARDATA
{
    DWORD dwUnknown1[52];
    DWORD dwD0;
    DWORD dwD4;
    DWORD dwUnknown2;
    DWORD dwMap;
} CHARDATA, *PCHARDATA;

#define GO_NOWHERE  0
#define GO_FORWARD  1
#define GO_BACKWARD 2
#define GO_LEFT     4
#define GO_RIGHT    8
#define GO_UP       16
#define GO_DOWN     32

typedef PCHARBLOCK(__fastcall *PFINDCHARBLOCK)(DWORD dw16, PSZ psz, DWORD dwD0, DWORD dwD4, DWORD dw148);

extern DWORD g_dwDirection;

VOID       Teleport_Init(PVOID pvGetMap, PVOID pvGetGuidData, DWORD dwGetGuidDataConstant1, DWORD dwGetGuidDataConstant2, PVOID pvGetCharacterGuid, PVOID pvFallingTeleport, PVOID pvCameraAngle, PFLOAT pfCorpse);
VOID       Teleport_CameraSlew(PVOID pCameraAngle);
VOID       Teleport_ChangeSpeed(METHOD method, FLOAT f);
VOID       Teleport_FreeWindow(HWND hwndWindow);
BOOL       Teleport_GetCurrentLocation(PLOCATION pLocation);
VOID       Teleport_GoToCorpse();
PCHARBLOCK Teleport_LookupCharBlock();
VOID       Teleport_SetWindow(HWND hwndWindow, DWORD dwUpdateSpeed);
VOID       Teleport_ShiftAxis(AXIS axis, FLOAT f);
BOOL       Teleport_StopFalling(PCHARBLOCK pCharBlock);
VOID       Teleport_Teleport(PLOCATION pLocation);

#endif