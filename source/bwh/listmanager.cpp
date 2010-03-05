#include "listmanager.h"
#include <stdio.h>
#include "common.h"
#include "settings.h"

// definitions
typedef struct _LOCATIONLIST
{
	BOOL bGroup;
	CHAR szName[21];
	FLOAT fX;
	FLOAT fY;
	FLOAT fZ;
	DWORD dwMap;
	struct _LOCATIONLIST* link;

} LOCATIONLIST, *PLOCATIONLIST;

// function prototypes
VOID  ListManager_AddItem(BOOL bGroup, PSZ pszName, PSZ pszX, PSZ pszY, PSZ pszZ, PSZ pszM, DWORD dwPos);
PBYTE ListManager_OpenFile(PSZ pszFilename, PDWORD pdwSize);
BOOL  ListManager_ParseLine(PBYTE* pbFile, PDWORD pdwSize, PBOOL pbGroup, PSZ szName, PSZ szMap, PSZ szX, PSZ szY, PSZ szZ);
VOID  ListManager_SaveTreeview();
VOID  ListManager_SetEnabledButtons(BOOL bNewGroup, BOOL bAddLocation, BOOL bEditEntry, BOOL bDeleteEntry, BOOL bMoveUp, BOOL bMoveDown, BOOL bTeleport);
VOID  ListManager_SetupButtons();

// global variables
PLOCATIONLIST    g_pLocationList = NULL;
DWORD            g_dwLocationListCount = 0;
BOOL             g_bLoadListDisplayed = FALSE;
BOOL             g_bEdit = FALSE;
PLOCATIONLIST    g_pEditItem;

// code start
VOID ListManager_AddItem(BOOL bGroup, PSZ pszName, PSZ pszX, PSZ pszY, PSZ pszZ, PSZ pszM, DWORD dwPos)
{
	PLOCATIONLIST* Insert = &g_pLocationList;
	PLOCATIONLIST NewItem;

	if(g_bEdit)
		NewItem = g_pEditItem;
	else
		NewItem = new LOCATIONLIST;
	strncpy(NewItem->szName, pszName, 20);
	NewItem->szName[20] = '\0';
	NewItem->bGroup = bGroup;
	if(bGroup == FALSE)
	{
		NewItem->fX = (FLOAT)atof(pszX);
		NewItem->fY = (FLOAT)atof(pszY);
		NewItem->fZ = (FLOAT)atof(pszZ);
		NewItem->dwMap = atol(pszM);
	}
	
	if(g_bEdit)
	{
		g_pTreeview->SetText(NewItem->szName, dwPos);
		g_bEdit = FALSE;
		return;
	}

	NewItem->link = g_pLocationList;

	if(dwPos > g_dwLocationListCount)
		dwPos = g_dwLocationListCount;
	g_dwLocationListCount++;

	for(DWORD x = 0; x < dwPos; x++)
	{
		Insert = &NewItem->link->link;
		NewItem->link = *Insert;
	}
	*Insert = NewItem;
	
	g_pTreeview->AddItem(NewItem->szName, bGroup, dwPos);
}

VOID ListManager_AddLocation(HWND hwndWindow, FLOAT fX, FLOAT fY, FLOAT fZ, DWORD dwMap)
{
	char szBuff[11];
	if(g_bEdit == FALSE)
		g_pAddLocationNEdit->SetText("");
	g_pAddLocationNEdit->SetFocus(hwndWindow);
	sprintf(szBuff, "%.3f", fX);
	g_pAddLocationXEdit->SetText(szBuff);
	sprintf(szBuff, "%.3f", fY);
	g_pAddLocationYEdit->SetText(szBuff);
	sprintf(szBuff, "%.3f", fZ);
	g_pAddLocationZEdit->SetText(szBuff);
	sprintf(szBuff, "%d", dwMap);
	g_pAddLocationMEdit->SetText(szBuff);
	g_pTreeview->Hide();
	g_pAddLocationWindow->Show();
	ListManager_SetEnabledButtons(0, 0, 0, 0, 0, 0, 0);
}

VOID ListManager_AddLocationCancel(HWND hwndWindow)
{
	g_pAddLocationNEdit->KillFocus(hwndWindow);
	g_pAddLocationWindow->Hide();
	g_pTreeview->Show();
	ListManager_SetupButtons();
	g_bEdit = FALSE;
}

VOID ListManager_AddLocationOk(HWND hwndWindow)
{
	char szName[21];
	char szX[11];
	char szY[11];
	char szZ[11];
	char szMap[11];
	
	g_pAddLocationNEdit->GetText(szName, 21);
	g_pAddLocationXEdit->GetText(szX, 11);
	g_pAddLocationYEdit->GetText(szY, 11);
	g_pAddLocationZEdit->GetText(szZ, 11);
	g_pAddLocationMEdit->GetText(szMap, 11);
	ListManager_AddItem(FALSE, szName, szX, szY, szZ, szMap, g_pTreeview->GetSelected());
	g_pAddLocationNEdit->KillFocus(hwndWindow);
	g_pAddLocationWindow->Hide();
	g_pTreeview->Show();
	ListManager_SetupButtons();
	ListManager_SaveTreeview();
}

VOID ListManager_DeleteEntry()
{
	PLOCATIONLIST pItem = g_pLocationList;
	PLOCATIONLIST* ppLast = &g_pLocationList;
	DWORD dwPos = g_pTreeview->GetSelected();

	if(g_dwLocationListCount && dwPos != -1)
	{
		g_dwLocationListCount--;

		for(DWORD x = 0; x < dwPos; x++)
		{
			ppLast = &pItem->link;
			pItem = pItem->link;
		}
		
		*ppLast = pItem->link;
		delete pItem;
		
		g_pTreeview->DeleteItem(dwPos);
	}
	ListManager_SetupButtons();
	ListManager_SaveTreeview();
}

VOID ListManager_EditEntry(HWND hwndWindow)
{
	DWORD dwPos = g_pTreeview->GetSelected();
	
	if(dwPos != -1)
	{
		g_bEdit = TRUE;
		g_pEditItem = g_pLocationList;
		for(DWORD x = 0; x < dwPos; x++)
			g_pEditItem = g_pEditItem->link;

		if(g_pEditItem->bGroup)
		{
			g_pNewGroupNEdit->SetText(g_pEditItem->szName);
			ListManager_NewGroup(hwndWindow);
		}
		else
		{
			g_pAddLocationNEdit->SetText(g_pEditItem->szName);
			ListManager_AddLocation(hwndWindow, g_pEditItem->fX, g_pEditItem->fY, g_pEditItem->fZ, g_pEditItem->dwMap);
		}
	}
}

VOID ListManager_FileEditChanged()
{
	char szFilename[MAX_PATH];
	char szFile[MAX_PATH];
	PLOCATIONLIST Delete, Next = g_pLocationList;

	Settings_SaveLocationFile();

	while(Next)
	{
		Delete = Next;
		Next = Next->link;
		delete Delete;
	}
	g_pLocationList = NULL;
	g_dwLocationListCount = 0;
	g_pTreeview->ClearList();

	GetMyDirectory(g_hinstDLL, szFilename);
	strcat(szFilename, "locations\\");
	CreateDirectory(szFilename, NULL);
	g_pFileEdit->GetText(szFile, MAX_PATH);
	strcat(szFilename, szFile);
	strcat(szFilename, ".txt");

	BOOL bGroup;
	char szName[21];
	char szX[101];
	char szY[101];
	char szZ[101];
	char szMap[101];

	DWORD dwSize;
	PBYTE pbFile = ListManager_OpenFile(szFilename, &dwSize);
	if(pbFile)
	{
		PBYTE pbScan = pbFile;
		while(ListManager_ParseLine(&pbScan, &dwSize, &bGroup, szName, szMap, szX, szY, szZ))
			ListManager_AddItem(bGroup, szName, szX, szY, szZ, szMap, -1);
		VirtualFree(pbFile, 0, MEM_RELEASE);
	}
	ListManager_SetupButtons();
}

BOOL ListManager_GetLocation(INT iIndex, PDWORD pdwMap, PFLOAT pfX, PFLOAT pfY, PFLOAT pfZ)
{
	PLOCATIONLIST pList = g_pLocationList;
	if(iIndex >= 0 && iIndex < (INT)g_dwLocationListCount)
	{
		for(INT x = 0; x < iIndex; x++)
			pList = pList->link;
		if(pList->bGroup == FALSE)
		{
			*pdwMap = pList->dwMap;
			*pfX = pList->fX;
			*pfY = pList->fY;
			*pfZ = pList->fZ;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ListManager_GetSelectedLocation(PDWORD pdwMap, PFLOAT pfX, PFLOAT pfY, PFLOAT pfZ)
{
	PLOCATIONLIST pList = g_pLocationList;
	DWORD dwSelected = g_pTreeview->GetSelected();
	if(dwSelected != -1)
	{
		for(DWORD x = 0; x < dwSelected; x++)
			pList = pList->link;
		if(pList->bGroup == FALSE)
		{
			*pdwMap = pList->dwMap;
			*pfX = pList->fX;
			*pfY = pList->fY;
			*pfZ = pList->fZ;
			return TRUE;
		}
	}
	return FALSE;
}

VOID ListManager_ListClick(HWND hwndWindow)
{
	if(g_bLoadListDisplayed)
	{
		g_pLoad->SetCheckState(FALSE);
		ListManager_Load(hwndWindow);
	}
	else
	{
		ListManager_SetupButtons();
	}
}

VOID ListManager_Load(HWND hwndWindow)
{
	char szFilename[MAX_PATH];
	WIN32_FIND_DATA FindData;
	PLOCATIONLIST pList = g_pLocationList;

	if(g_pLoad->GetCheckState())
	{
		GetMyDirectory(g_hinstDLL, szFilename);
		strcat(szFilename, "locations\\");
		CreateDirectory(szFilename, NULL);
		strcat(szFilename, "*.txt");

		g_pTreeview->ClearList();
		HANDLE hFind = FindFirstFile(szFilename, &FindData);
		if(hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				FindData.cFileName[strlen(FindData.cFileName) - 4] = '\0';
				g_pTreeview->AddItem(FindData.cFileName, FALSE, -1);
			} while(FindNextFile(hFind, &FindData));
			FindClose(hFind);
		}
		g_pTreeview->SetIndexStyle(FALSE);
		g_pTreeview->SetMenuStyle(TRUE);
		g_bLoadListDisplayed = TRUE;
		ListManager_SetEnabledButtons(0, 0, 0, 0, 0, 0, 0);
	}
	else
	{
		if(g_pTreeview->GetSelected() == -1)
		{
			g_pTreeview->ClearList();
			while(pList)
			{
				g_pTreeview->AddItem(pList->szName, pList->bGroup, -1);
				pList = pList->link;
			}
		}
		else
		{
			g_pTreeview->GetText(szFilename, MAX_PATH, g_pTreeview->GetSelected());
			g_pFileEdit->SetText(szFilename);
			g_pFileEdit->KillFocus(hwndWindow);
			g_pTreeview->ClearList();
			ListManager_FileEditChanged();
		}
		g_pTreeview->SetIndexStyle(TRUE);
		g_pTreeview->SetMenuStyle(FALSE);
		g_bLoadListDisplayed = FALSE;
		ListManager_SetupButtons();
	}
}

VOID ListManager_MoveItem(BOOL bUp)
{
	PLOCATIONLIST* ppPrev = &g_pLocationList;
	PLOCATIONLIST pMover = g_pLocationList;
	DWORD dwPos = g_pTreeview->GetSelected();

	if(g_dwLocationListCount > 1 && (bUp && (INT)dwPos > 0) || (bUp == FALSE && dwPos < g_dwLocationListCount - 1))
	{
		for(DWORD x = 0; x < dwPos - (bUp ? 1 : 0); x++)
		{
			ppPrev = &pMover->link;
			pMover = pMover->link;
		}
		*ppPrev = pMover->link;
		ppPrev = &pMover->link->link;
		pMover->link = pMover->link->link;
		*ppPrev = pMover;
		g_pTreeview->MoveItem(dwPos, bUp);
		ListManager_SaveTreeview();
		ListManager_SetupButtons();
	}
}

VOID ListManager_NewGroup(HWND hwndWindow)
{
	if(g_bEdit == FALSE)
		g_pNewGroupNEdit->SetText("");
	g_pNewGroupNEdit->SetFocus(hwndWindow);
	g_pTreeview->Hide();
	g_pNewGroupWindow->Show();
	ListManager_SetEnabledButtons(0, 0, 0, 0, 0, 0, 0);
}

VOID ListManager_NewGroupCancel(HWND hwndWindow)
{
	g_pNewGroupNEdit->KillFocus(hwndWindow);
	g_pNewGroupWindow->Hide();
	g_pTreeview->Show();
	ListManager_SetupButtons();
	g_bEdit = FALSE;
}

VOID ListManager_NewGroupOk(HWND hwndWindow)
{
	char szName[21];
	g_pNewGroupNEdit->GetText(szName, 21);
	ListManager_AddItem(TRUE, szName, NULL, NULL, NULL, NULL, g_pTreeview->GetSelected());
	g_pNewGroupNEdit->KillFocus(hwndWindow);
	g_pNewGroupWindow->Hide();
	g_pTreeview->Show();
	ListManager_SetupButtons();
	ListManager_SaveTreeview();
}

PBYTE ListManager_OpenFile(PSZ pszFilename, PDWORD pdwSize)
{
	DWORD dwRead = 0;
	PBYTE pbRet = NULL;
	HANDLE hFile = CreateFile(pszFilename, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		*pdwSize = GetFileSize(hFile, NULL);
		PBYTE pbFile = (PBYTE)VirtualAlloc(NULL, *pdwSize, MEM_COMMIT, PAGE_READWRITE);
		if(pbFile)
		{
			ReadFile(hFile, pbFile, *pdwSize, &dwRead, NULL);
			if(*pdwSize == dwRead)
				pbRet = pbFile;
			else
				VirtualFree(pbFile, 0, MEM_RELEASE);
		}
		CloseHandle(hFile);
	}
	return pbRet;
}

BOOL ListManager_ParseLine(PBYTE* pbFile, PDWORD pdwSize, PBOOL pbGroup, PSZ pszName, PSZ pszMap, PSZ pszX, PSZ pszY, PSZ pszZ)
{
	DWORD y, c, x = -1;
	BOOL bEntry;
	do
	{
		if(x == *pdwSize)
			return FALSE;
		for(bEntry = FALSE, c = 0, x++; x < *pdwSize; x++)
		{
			if((*pbFile)[x] == ',')
				c++;
			else if((*pbFile)[x] == 0x0A || (*pbFile)[x] == 0x0D)
				break;
			else if((*pbFile)[x] != ' ' && (*pbFile)[x] != '\t' && (*pbFile)[x] != '\0')
				bEntry = TRUE;

		}
	} while(c == 0 && bEntry == FALSE);
	if(c > 3)
	{
		(*pbFile)[x] = '\0';
		for(y = x - 1; (*pbFile)[y] != ','; y--);
		strncpy(pszZ, (PSZ)&(*pbFile)[y+1], 100);
		pszZ[100] = '\0';
		(*pbFile)[y] = '\0';
		for(y--; (*pbFile)[y] != ','; y--);
		strncpy(pszY, (PSZ)&(*pbFile)[y+1], 100);
		pszY[100] = '\0';
		(*pbFile)[y] = '\0';
		for(y--; (*pbFile)[y] != ','; y--);
		strncpy(pszX, (PSZ)&(*pbFile)[y+1], 100);
		pszX[100] = '\0';
		(*pbFile)[y] = '\0';
		for(y--; (*pbFile)[y] != ','; y--);
		strncpy(pszMap, (PSZ)&(*pbFile)[y+1], 100);
		pszMap[100] = '\0';
		*pbGroup = FALSE;
	}
	else
	{
		y = x;
		*pbGroup = TRUE;
	}
	for(y--; (*pbFile)[y] == ' ' || (*pbFile)[y] == '\t' || (*pbFile)[y] == '\0'; y--);
	(*pbFile)[y+1] = '\0';
	for(y = 0; (*pbFile)[y] == ' ' || (*pbFile)[y] == '\t' || (*pbFile)[y] == '\0' || (*pbFile)[y] == 0x0A || (*pbFile)[y] == 0x0D; y++);
	strncpy(pszName, (PSZ)&(*pbFile)[y], 20);
	pszName[20] = '\0';
	*pdwSize -= x;
	*pbFile += x;
	return TRUE;
}

VOID ListManager_SaveTreeview()
{
	char szFilename[MAX_PATH];
	char szBuff[MAX_PATH];
	HANDLE hFile = INVALID_HANDLE_VALUE;
	PLOCATIONLIST pList = g_pLocationList;
	DWORD dwWritten;

	GetMyDirectory(g_hinstDLL, szFilename);
	strcat(szFilename, "locations\\");
	CreateDirectory(szFilename, NULL);
	g_pFileEdit->GetText(szBuff, MAX_PATH);
	strcat(szFilename, szBuff);
	strcat(szFilename, ".txt");

	if(pList == NULL)
		DeleteFile(szFilename);
	else
		hFile = CreateFile(szFilename, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, NULL, NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		while(pList)
		{
			if(pList->bGroup)
				sprintf(szBuff, "\x0D\x0A%s\x0D\x0A", pList->szName);
			else
				sprintf(szBuff, "%s, %d, %.3f, %.3f, %.3f\x0D\x0A", pList->szName, pList->dwMap, pList->fX, pList->fY, pList->fZ);
			WriteFile(hFile, szBuff, (DWORD)strlen(szBuff), &dwWritten, NULL);
			pList = pList->link;
		}
		CloseHandle(hFile);
	}
}

VOID ListManager_SetEnabledButtons(BOOL bNewGroup, BOOL bAddLocation, BOOL bEditEntry, BOOL bDeleteEntry, BOOL bMoveUp, BOOL bMoveDown, BOOL bTeleport)
{
	if(bNewGroup)
		g_pNewGroup->Enable();
	else
		g_pNewGroup->Disable();
	
	if(bAddLocation)
		g_pAddLocation->Enable();
	else
		g_pAddLocation->Disable();

	if(bEditEntry)
		g_pEditEntry->Enable();
	else
		g_pEditEntry->Disable();

	if(bDeleteEntry)
		g_pDeleteEntry->Enable();
	else
		g_pDeleteEntry->Disable();

	if(bMoveUp)
		g_pMoveUp->Enable();
	else
		g_pMoveUp->Disable();

	if(bMoveDown)
		g_pMoveDown->Enable();
	else
		g_pMoveDown->Disable();

	if(bTeleport)
		g_pTeleport->Enable();
	else
		g_pTeleport->Disable();
}

VOID ListManager_SetupButtons()
{
	PLOCATIONLIST pList = g_pLocationList;
	BOOL bNewGroup, bAddLocation, bEditEntry, bDeleteEntry, bMoveUp, bMoveDown, bTeleport;
	DWORD dwPos = g_pTreeview->GetSelected();

	bNewGroup = TRUE;
	bAddLocation = TRUE;
	if(dwPos == -1)
	{
		bEditEntry = FALSE;
		bDeleteEntry = FALSE;
		bTeleport = FALSE;
		bMoveUp = FALSE;
		bMoveDown = FALSE;
	}
	else
	{
		for(DWORD x = 0; x < dwPos; x++)
			pList = pList->link;

		bEditEntry = TRUE;
		bDeleteEntry = TRUE;
		if(pList->bGroup)
			bTeleport = FALSE;
		else
			bTeleport = TRUE;
		if(dwPos == 0)
			bMoveUp = FALSE;
		else
			bMoveUp = TRUE;
		if(dwPos == g_dwLocationListCount - 1)
			bMoveDown = FALSE;
		else
			bMoveDown = TRUE;
	}
	ListManager_SetEnabledButtons(bNewGroup, bAddLocation, bEditEntry, bDeleteEntry, bMoveUp, bMoveDown, bTeleport);
}
