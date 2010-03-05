#ifndef _HOOKER_H
#define _HOOKER_H

#include <windows.h>
#include "dxctrl\dxctrl.h"

typedef BOOL(__stdcall *PFNRECVCHATMESSAGE)(PSZ pszMsg);
typedef BOOL(*PFNWINDOWPROC)(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

VOID            HookChatFunction(PVOID pChatAddress, PFNRECVCHATMESSAGE pRecvFunc);
PCDXCtrlManager HookD3D(HMODULE hModule, DWORD dwPresentRVA, DWORD dwResetRVA, PSZ pszFixedFont, INT iFixedSize, BOOL bFixedBold, PSZ pszProportionalFont, INT iProportionalSize, BOOL bProportionalBold);
BOOL            HookWindowProcedure(PFNWINDOWPROC pfnWindowProc);
VOID            UnHookChatFunction(PVOID pChatAddress);
VOID            UnHookD3D(DWORD dwPresentRVA, DWORD dwResetRVA);
VOID            UnHookWindowProcedure();

#endif