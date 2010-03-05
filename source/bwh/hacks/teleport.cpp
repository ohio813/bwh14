#include "teleport.h"
#include <math.h> 
#include "..\resource.h"

// defines
#define STATE_FALLING         8192
#define STATE_FORWARD         1
#define STATE_BACKWARD        2
#define STATE_LEFT            4
#define STATE_RIGHT           8
#define STATE_MOVEMENT        (STATE_FALLING | STATE_FORWARD | STATE_BACKWARD | STATE_LEFT | STATE_RIGHT)
#define SWIM_MODIFIER         2.2778
#define BACKWARD_MODIFIER     2.5
#define WALK_MODIFIER         4.5
#define BACKWARDSWIM_MODIFIER 4.5

typedef struct _CAMERAANGLE
{
    FLOAT y;
    FLOAT x;
} CAMERAANGLE, *PCAMERAANGLE;


// function prototypes
DWORD Teleport_GetMap();

// global variabls
PCHARBLOCK g_pLastCharBlock = NULL;
DWORD      g_dwDirection;
PSZ g_pszGetGuidDataString = "..\\Object/ObjectClient/Player_C.h";

PVOID  g_GetMap = NULL;
PVOID  g_GetGuidData = NULL;
DWORD  g_GetGuidDataConstant1 = NULL;
DWORD  g_GetGuidDataConstant2 = NULL;
PVOID  g_GetCharacterGuid = NULL;
PVOID  g_FallingTeleport = NULL;
PVOID  g_CameraAngle = NULL;
PFLOAT g_Corpse = NULL;

// code start
VOID Teleport_Init(PVOID pvGetMap, PVOID pvGetGuidData, DWORD dwGetGuidDataConstant1, DWORD dwGetGuidDataConstant2, PVOID pvGetCharacterGuid, PVOID pvFallingTeleport, PVOID pvCameraAngle, PFLOAT pfCorpse)
{
	g_GetMap = pvGetMap;
	g_GetGuidData = pvGetGuidData;
	g_GetGuidDataConstant1 = dwGetGuidDataConstant1;
	g_GetGuidDataConstant2 = dwGetGuidDataConstant2;
	g_GetCharacterGuid = pvGetCharacterGuid;
	g_FallingTeleport = pvFallingTeleport;
	g_CameraAngle = pvCameraAngle;
	g_Corpse = pfCorpse;
}

VOID Teleport_CameraSlew(PVOID pCameraAngle)
{
    float x=0, y=0, z=0;
	if(g_CameraAngle)
	{
		PCHARBLOCK pCharBlock = Teleport_LookupCharBlock();
		if(pCharBlock)
		{
			if(g_dwDirection & GO_UP)
				z += 1;
			if(g_dwDirection & GO_DOWN)
				z -= 1;
			if(g_dwDirection & GO_FORWARD)
			{
				x += sinf(((PCAMERAANGLE)g_CameraAngle)->x);
				y += cosf(((PCAMERAANGLE)g_CameraAngle)->x);
				z += ((PCAMERAANGLE)g_CameraAngle)->y;
			}
			if(g_dwDirection & GO_BACKWARD)
			{
				x -= sinf(((PCAMERAANGLE)g_CameraAngle)->x);
				y -= cosf(((PCAMERAANGLE)g_CameraAngle)->x);
				z -= ((PCAMERAANGLE)g_CameraAngle)->y;
			}
			if(g_dwDirection & GO_LEFT)
			{
				x += cosf(((PCAMERAANGLE)g_CameraAngle)->x);
				y -= sinf(((PCAMERAANGLE)g_CameraAngle)->x);
			}
			if(g_dwDirection & GO_RIGHT)
			{
				x -= cosf(((PCAMERAANGLE)g_CameraAngle)->x);
				y += sinf(((PCAMERAANGLE)g_CameraAngle)->x);
			}
			if(x || y || z)
			{
				Teleport_StopFalling(pCharBlock);
				pCharBlock->x += (x * (pCharBlock->runspeed / 32));
				pCharBlock->y += (y * (pCharBlock->runspeed / 32));
				pCharBlock->z += (z * (pCharBlock->runspeed / 32));
			}
		}
	}
}

VOID Teleport_ChangeSpeed(METHOD method, FLOAT f)
{
    PCHARBLOCK pCharBlock = Teleport_LookupCharBlock();
	if(pCharBlock)
	{
        if(method == METHOD_ADD)
        {
			pCharBlock->walkspeed += f;
            pCharBlock->runspeed += f;
			pCharBlock->backwardspeed += f;
            pCharBlock->swimspeed += f;
			pCharBlock->backwardswimspeed += f;
        }
        else
        {
			pCharBlock->walkspeed = f - (float)WALK_MODIFIER;
            pCharBlock->runspeed = f;
			pCharBlock->backwardspeed = f - (float)BACKWARD_MODIFIER;
            pCharBlock->swimspeed = f - (float)SWIM_MODIFIER;
			pCharBlock->backwardswimspeed = f - (float)BACKWARDSWIM_MODIFIER;
        }
	}
}

VOID Teleport_FreeWindow(HWND hwndWindow)
{
	KillTimer(hwndWindow, IDT_UPDATE);
}

BOOL Teleport_GetCurrentLocation(PLOCATION pLocation)
{
	PCHARBLOCK pCharBlock = Teleport_LookupCharBlock();
	if(pCharBlock)
	{
		pLocation->dwMap = Teleport_GetMap();
		pLocation->fX = pCharBlock->x;
		pLocation->fY = pCharBlock->y;
		pLocation->fZ = pCharBlock->z;
		pLocation->fSpeed = pCharBlock->runspeed;
	}
	else
	{
		pLocation->dwMap = -1;
		pLocation->fX = 0;
		pLocation->fY = 0;
		pLocation->fZ = 0;
		pLocation->fSpeed = 0;
	}
	if(pCharBlock != g_pLastCharBlock)
	{
		g_pLastCharBlock = pCharBlock;
		return TRUE;
	}
	else
		return FALSE;
}

VOID Teleport_GoToCorpse()
{
	if(g_Corpse && (g_Corpse[0] || g_Corpse[1] || g_Corpse[2]))
	{
		PCHARBLOCK pCharBlock = Teleport_LookupCharBlock();
		if(pCharBlock)
		{
			Teleport_StopFalling(pCharBlock);
			pCharBlock->x = g_Corpse[1];
			pCharBlock->y = g_Corpse[0];
			pCharBlock->z = g_Corpse[2];
		}
	}
}

PCHARBLOCK Teleport_LookupCharBlock()
{
	if(g_GetCharacterGuid && g_GetGuidData && g_GetGuidDataConstant1 && g_GetGuidDataConstant2)
	{
		__asm 
		{
			call dword ptr[g_GetCharacterGuid]
			test eax, eax
			jz notingame
			push dword ptr[g_GetGuidDataConstant2]
			push edx
			push eax
			mov edx, dword ptr[g_pszGetGuidDataString]
			mov ecx, dword ptr[g_GetGuidDataConstant1]
			call dword ptr[g_GetGuidData]
notingame:
		}
	}
	else
		return NULL;
}

VOID Teleport_SetWindow(HWND hwndWindow, DWORD dwUpdateSpeed)
{
	SetTimer(hwndWindow, IDT_UPDATE, dwUpdateSpeed, NULL);
}

VOID Teleport_ShiftAxis(AXIS axis, FLOAT f)
{
    PCHARBLOCK pCharBlock = Teleport_LookupCharBlock();
	if(pCharBlock)
	{
		Teleport_StopFalling(pCharBlock);
		f *= (pCharBlock->runspeed / 7);
		switch(axis)
		{
            case AXIS_X:
                pCharBlock->x += f;
                break;
            case AXIS_Y:
                pCharBlock->y += f;
                break;
            case AXIS_Z:
                pCharBlock->z += f;
                break;
		}
	}
}

BOOL Teleport_StopFalling(PCHARBLOCK pCharBlock)
{
	if(g_FallingTeleport)
	{
		__asm call dword ptr[g_FallingTeleport]
		pCharBlock->state &= ~STATE_MOVEMENT;
		return TRUE;
	}
	return FALSE;
}

DWORD Teleport_GetMap()
{
	if(g_GetMap)
	{
		__asm
		{
			call dword ptr[g_GetCharacterGuid]
			test eax, eax
			jz notingame
			call dword ptr[g_GetMap]
			jmp goodtogo
notingame:	not eax
goodtogo:
		}
	}
	else
		return -1;
}

VOID Teleport_Teleport(PLOCATION pLocation)
{
	PCHARBLOCK pCharBlock = Teleport_LookupCharBlock();
	if(pCharBlock)
	{
		if(Teleport_GetMap() == pLocation->dwMap)
		{
			Teleport_StopFalling(pCharBlock);
			pCharBlock->x = pLocation->fX;
			pCharBlock->y = pLocation->fY;
			pCharBlock->z = pLocation->fZ;
		}
	}
}