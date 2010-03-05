#include "follownpc.h"
#include "..\common.h"

BYTE g_bFollowNpcBackup1[2];
BYTE g_bFollowNpcBackup2[2];
BYTE g_bFollowNpcBackup3[2];

VOID FollowNPC_Enable(PBYTE pFollowNpc1, PBYTE pFollowNpc2, PBYTE pFollowFaction, PFLOAT pFollowNPCDistance, BOOL bEnable)
{
	DWORD dwOldProtect;
	if(pFollowNpc1 && pFollowNpc2 && VirtualProtect(pFollowNpc1, PtrToUlong(pFollowNpc2) - PtrToUlong(pFollowNpc1) + 1, PAGE_EXECUTE_READWRITE, &dwOldProtect))
	{
		if(bEnable)
		{
			if(pFollowNpc1[0] != 0x90)
			{
				memcpy(g_bFollowNpcBackup1, pFollowNpc1, sizeof(g_bFollowNpcBackup1));
				memcpy(g_bFollowNpcBackup2, pFollowNpc2, sizeof(g_bFollowNpcBackup2));
			}
			memcpy(pFollowNpc1, g_bNop, sizeof(g_bFollowNpcBackup1));
			memcpy(pFollowNpc2, g_bNop, sizeof(g_bFollowNpcBackup2));
		}
		else
		{
			if(pFollowNpc1[0] == 0x90)
			{
				memcpy(pFollowNpc1, g_bFollowNpcBackup1, sizeof(g_bFollowNpcBackup1));
				memcpy(pFollowNpc2, g_bFollowNpcBackup2, sizeof(g_bFollowNpcBackup2));
			}
		}
		VirtualProtect(pFollowNpc1, PtrToUlong(pFollowNpc2) - PtrToUlong(pFollowNpc1) + 1, dwOldProtect, &dwOldProtect);
	}
	if(pFollowFaction && VirtualProtect(pFollowFaction, sizeof(g_bFollowNpcBackup3), PAGE_EXECUTE_READWRITE, &dwOldProtect))
	{
		if(bEnable)
		{
			if(pFollowFaction[0] != 0x90)
			{
				memcpy(g_bFollowNpcBackup3, pFollowFaction, sizeof(g_bFollowNpcBackup3));
			}
			memcpy(pFollowFaction, g_bNop, sizeof(g_bFollowNpcBackup3));
		}
		else
		{
			if(pFollowFaction[0] == 0x90)
			{
				memcpy(pFollowFaction, g_bFollowNpcBackup3, sizeof(g_bFollowNpcBackup3));
			}
		}
		VirtualProtect(pFollowFaction, sizeof(g_bFollowNpcBackup3), dwOldProtect, &dwOldProtect);
	}
	if(pFollowNPCDistance && VirtualProtect(pFollowNPCDistance, sizeof(FLOAT), PAGE_READWRITE, &dwOldProtect))
	{
		if(bEnable)
		{
			*(PDWORD)pFollowNPCDistance = 0x7F800000;
		}
		else
		{
			*pFollowNPCDistance = 900;
		}
		VirtualProtect(pFollowNPCDistance, sizeof(FLOAT), dwOldProtect, &dwOldProtect);
	}

}