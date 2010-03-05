#include "zerogravity.h"
#include "teleport.h"

// defines
#define STATE_ONGROUND        2048
#define STATE_JUMPING         1073741824

// code start
VOID ZeroGravity_Enable(PDWORD pZeroGravity, BOOL bEnable)
{
	DWORD dwOldProtect;
	PCHARBLOCK pCharBlock = Teleport_LookupCharBlock();

	if(pCharBlock && pZeroGravity && VirtualProtect(pZeroGravity, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &dwOldProtect))
	{
		if(bEnable)
		{
			Teleport_StopFalling(pCharBlock);
			*pZeroGravity &= ~STATE_ONGROUND;
			pCharBlock->state |= (STATE_JUMPING | STATE_ONGROUND);
		}
		else
		{
			*pZeroGravity |= STATE_ONGROUND;
			pCharBlock->state &= ~(STATE_JUMPING | STATE_ONGROUND);
		}
		VirtualProtect(pZeroGravity, sizeof(DWORD), dwOldProtect, &dwOldProtect);
	}

}