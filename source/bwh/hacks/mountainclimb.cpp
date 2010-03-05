#include "mountainclimb.h"

VOID MountainClimb_Enable(PDWORD pMountainClimb, BOOL bEnable)
{
	DWORD dwOldProtect;
	if(pMountainClimb && VirtualProtect(pMountainClimb, sizeof(DWORD), PAGE_READWRITE, &dwOldProtect))
	{
		if(bEnable)
		{
            *pMountainClimb = 0x3DCCCCCD;
		}
		else
		{
            *pMountainClimb = 0x3F248DBB;
		}
		VirtualProtect(pMountainClimb, sizeof(DWORD), dwOldProtect, &dwOldProtect);
	}

}