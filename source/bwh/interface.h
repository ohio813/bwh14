#ifndef _INTERFACE_H
#define _INTERFACE_H

#include <windows.h>
#include <d3d9.h>
#include "dxctrl\dxctrl.h"

VOID            Interface_DeleteControls();
VOID            Interface_FreeWindow();
PCDXCtrlManager Interface_InitControls(HWND hwndWindow, HMODULE hModule, PSZ pszFixedFont, INT iFixedSize, BOOL bFixedBold, PSZ pszProportionalFont, INT iProportionalSize, BOOL bProportionalBold);
VOID __stdcall  Interface_PresentHook(PDIRECT3DDEVICE9 pD3DDevice);
VOID __stdcall  Interface_ResetHook();
VOID            Interface_SetWindow(HWND hwndWindow);


#endif