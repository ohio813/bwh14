#ifndef _LISTMANAGER_H
#define _LISTMANAGER_H

#include <windows.h>
#include "dxctrl\dxctrl.h"

VOID ListManager_AddLocation(HWND hwndWindow, FLOAT fX, FLOAT fY, FLOAT fZ, DWORD dwMap);
VOID ListManager_AddLocationCancel(HWND hwndWindow);
VOID ListManager_AddLocationOk(HWND hwndWindow);
VOID ListManager_DeleteEntry();
VOID ListManager_EditEntry(HWND hwndWindow);
VOID ListManager_FileEditChanged();
BOOL ListManager_GetLocation(INT iIndex, PDWORD pdwMap, PFLOAT pfX, PFLOAT pfY, PFLOAT pfZ);
BOOL ListManager_GetSelectedLocation(PDWORD pdwMap, PFLOAT pfX, PFLOAT pfY, PFLOAT pfZ);
VOID ListManager_NewGroup(HWND hwndWindow);
VOID ListManager_NewGroupCancel(HWND hwndWindow);
VOID ListManager_NewGroupOk(HWND hwndWindow);
VOID ListManager_ListClick(HWND hwndWindow);
VOID ListManager_Load(HWND hwndWindow);
VOID ListManager_MoveItem(BOOL bUp);

#endif