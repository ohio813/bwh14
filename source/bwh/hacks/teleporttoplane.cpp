#include "teleporttoplane.h"
#include "..\common.h"

BYTE g_bPlaneTeleportBackup[3];

VOID TeleportToPlane_Enable(PBYTE pPlaneTeleport, BOOL bEnable)
{
	DWORD dwOldProtect;
	if(pPlaneTeleport && VirtualProtect(pPlaneTeleport, sizeof(g_bPlaneTeleportBackup), PAGE_READWRITE, &dwOldProtect))
	{
		if(bEnable)
		{
			if(pPlaneTeleport[0] != 0x90)
				memcpy(g_bPlaneTeleportBackup, pPlaneTeleport, sizeof(g_bPlaneTeleportBackup));
			memcpy(pPlaneTeleport, g_bNop, sizeof(g_bPlaneTeleportBackup));
		}
		else
		{
			if(pPlaneTeleport[0] == 0x90)
				memcpy(pPlaneTeleport, g_bPlaneTeleportBackup, sizeof(g_bPlaneTeleportBackup));
		}
		VirtualProtect(pPlaneTeleport, sizeof(g_bPlaneTeleportBackup), dwOldProtect, &dwOldProtect);
	}
}