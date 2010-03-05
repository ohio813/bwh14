#ifndef _NOFALLDAMAGE_H
#define _NOFALLDAMAGE_H

#include <windows.h>

VOID NoFallDamage_Hook(PDWORD pNoFallDamageHook, DWORD dwNoFallDamageOffset);
VOID NoFallDamage_Enable(BOOL bEnable);
VOID NoFallDamage_UnHook(PDWORD pNoFallDamageHook);

#endif