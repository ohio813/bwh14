#include "lockspeed.h"
#include "..\common.h"

BYTE g_bLockSpeedBackup[6];

VOID LockSpeed_Enable(PBYTE pLockSpeed, BOOL bEnable)
{
	DWORD dwOldProtect;
	if(pLockSpeed && VirtualProtect(pLockSpeed, sizeof(g_bLockSpeedBackup), PAGE_READWRITE, &dwOldProtect))
	{
		if(bEnable)
		{
			if(pLockSpeed[0] != 0x90)
				memcpy(g_bLockSpeedBackup, pLockSpeed, sizeof(g_bLockSpeedBackup));
			memcpy(pLockSpeed, g_bNop, sizeof(g_bLockSpeedBackup));
		}
		else
		{
			if(pLockSpeed[0] == 0x90)
				memcpy(pLockSpeed, g_bLockSpeedBackup, sizeof(g_bLockSpeedBackup));
		}
		VirtualProtect(pLockSpeed, sizeof(g_bLockSpeedBackup), dwOldProtect, &dwOldProtect);
	}
}