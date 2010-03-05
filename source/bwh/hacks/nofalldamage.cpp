#include "nofalldamage.h"
#include "..\common.h"

VOID NoFallDamage_Stub();

DWORD g_NoFallDamageOffset = NULL;
DWORD g_pOrigNoFallDamageFunc = NULL;
BOOL g_bNoFallDamageEnabled = FALSE;
DWORD g_FallTimeOffset;
DWORD g_FallTimeValue;
DWORD g_WowReturn;

VOID NoFallDamage_Hook(PDWORD pNoFallDamageHook, DWORD dwNoFallDamageOffset)
{
	DWORD dwOldProtect;
	
	g_NoFallDamageOffset = dwNoFallDamageOffset;
	
	if(pNoFallDamageHook && g_NoFallDamageOffset && VirtualProtect(pNoFallDamageHook, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect))
	{
		g_pOrigNoFallDamageFunc = *pNoFallDamageHook + (PtrToUlong(pNoFallDamageHook) + 4);
		*pNoFallDamageHook = PtrToUlong(NoFallDamage_Stub) - (PtrToUlong(pNoFallDamageHook) + 4);
		VirtualProtect(pNoFallDamageHook, 4, dwOldProtect, &dwOldProtect);
	}
}

VOID NoFallDamage_Enable(BOOL bEnable)
{
	g_bNoFallDamageEnabled = bEnable;
}

__declspec(naked) VOID NoFallDamage_Stub()
{
	__asm
	{
		// is no fall damage enabled?
		cmp dword ptr[g_bNoFallDamageEnabled], 0
		je returntowow

		// make sure pointer is not null
		test ecx, ecx
		je returntowow

		// calculate offset to fall time and compare with max value
		mov eax, ecx
		add eax, dword ptr[g_NoFallDamageOffset]
		cmp dword ptr[eax], 0x480
		jle returntowow

		// modify procedure return address
		pop dword ptr[g_WowReturn]
		push returntobwh
		
		// backup offset to fall time and it's current value
		mov dword ptr[g_FallTimeOffset], eax
		push dword ptr[eax]
		pop dword ptr[g_FallTimeValue]

		// set fall time to max value
		mov dword ptr[eax], 0x480

returntowow:
		mov eax, dword ptr[g_pOrigNoFallDamageFunc]
		jmp eax

returntobwh:
		push eax
		
		// restore original fall time
		mov eax, dword ptr[g_FallTimeOffset]
		push dword ptr[g_FallTimeValue]
		pop dword ptr[eax]
		
		pop eax
		
		// return to wow
		push dword ptr[g_WowReturn]
		ret
	}
}

VOID NoFallDamage_UnHook(PDWORD pNoFallDamageHook)
{
	DWORD dwOldProtect;
	if(pNoFallDamageHook && g_pOrigNoFallDamageFunc)
	{
		VirtualProtect(pNoFallDamageHook, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);
		*pNoFallDamageHook = g_pOrigNoFallDamageFunc - (PtrToUlong(pNoFallDamageHook) + 4);
		VirtualProtect(pNoFallDamageHook, 4, dwOldProtect, &dwOldProtect);
		g_pOrigNoFallDamageFunc = NULL;
	}
}