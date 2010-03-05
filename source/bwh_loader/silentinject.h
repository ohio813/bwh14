#ifndef _SILENTINJECT_H
#define _SILENTINJECT_H

#include <windows.h>

VOID SilentInjectDll(HWND hwndWoW, DWORD dwLoadRVA, DWORD dwUnloadRVA, PSZ pszDll, BOOL bInject);

#endif