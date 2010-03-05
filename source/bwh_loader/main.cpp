#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include "crc.h"
#include "silentinject.h"
#include "resource.h"

PVOID CheckINI();
INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID GetMyDirectory(HMODULE hModule, PSZ pszDir);
VOID UpdateProcesses(HWND hwndDlg);

HMODULE hModule;

void WinMainCRTStartup()
{
	hModule = GetModuleHandle(NULL);
	InitCommonControls();
	ExitProcess((UINT)DialogBoxParam(hModule, MAKEINTRESOURCE(IDD_MAIN), NULL, &DialogProc, NULL));
}

DWORD CheckINI(PDWORD pdwUnloadRVA)
{
	DWORD dwWoWCRC, dwD3DCRC, dwBWHCRC;
	char szWoW[MAX_PATH];
	char szINI[MAX_PATH];
	char szD3D[MAX_PATH];
	char szBWH[MAX_PATH];
	BOOL bFalse = FALSE;

	GetMyDirectory(GetModuleHandle(NULL), szINI);
	lstrcpy(szBWH, szINI);
	lstrcat(szBWH, "bwh.dll");
	lstrcat(szINI, "bwh.ini");
	GetSystemDirectory(szD3D, MAX_PATH);
	lstrcat(szD3D, "\\d3d9.dll");

	if( GetPrivateProfileString("WOW", "EXE", "", szWoW, MAX_PATH, szINI) &&
		GetFileAttributes(szWoW) != INVALID_FILE_ATTRIBUTES &&
		GetFileAttributes(szD3D) != INVALID_FILE_ATTRIBUTES)
    {
        dwWoWCRC = GetPrivateProfileInt("WOW", "CRC", 0, szINI);
		dwD3DCRC = GetPrivateProfileInt("D3D", "CRC", 0, szINI);
		dwBWHCRC = GetPrivateProfileInt("BWH", "CRC", 0, szINI);
        if(dwWoWCRC && dwWoWCRC == GetFileCRC(szWoW, &bFalse, NULL) &&
		   dwD3DCRC && dwD3DCRC == GetFileCRC(szD3D, &bFalse, NULL) &&
		   dwBWHCRC && dwBWHCRC == GetFileCRC(szBWH, &bFalse, NULL))
        {
            if( GetPrivateProfileInt("D3D", "Present", 0, szINI) &&
                GetPrivateProfileInt("D3D", "Reset", 0, szINI))
			{
				*pdwUnloadRVA = GetPrivateProfileInt("BWH", "BwhWindowProc", 0, szINI);
				return GetPrivateProfileInt("WOW", "WowWindowProc", 0, szINI);
			}
		}
    }
    return FALSE;
}

INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char szDll[MAX_PATH];
	WORD wCode, wId;
	HICON hMain;
	DWORD dwLoadRVA, dwUnloadRVA;

	switch(uMsg)
	{
	case WM_CLOSE:
		KillTimer(hwndDlg, 0);
		EndDialog(hwndDlg, 0);
		break;
	case WM_COMMAND:
		wId = LOWORD(wParam);
		wCode = HIWORD(wParam);
		switch(wId)
		{
		case IDC_INSTALL:
			dwLoadRVA = CheckINI(&dwUnloadRVA);
			if(dwLoadRVA && dwUnloadRVA)
			{
				GetMyDirectory(GetModuleHandle(NULL), szDll);
				lstrcat(szDll, "bwh.dll");
				SilentInjectDll((HWND)SendDlgItemMessage(hwndDlg, IDC_LIST, LB_GETITEMDATA, SendDlgItemMessage(hwndDlg, IDC_LIST, LB_GETCURSEL, 0, 0), 0), dwLoadRVA, dwUnloadRVA, szDll, SendDlgItemMessage(hwndDlg, IDC_INJECT, BM_GETCHECK, 0, 0) ? TRUE : FALSE);
			}
			else
				MessageBox(NULL, "The file \"bwh.ini\" is missing or invalid.\nPlease run bwh_setup.exe to fix the file.", "BWH Loader", MB_ICONERROR);
			break;
		default:
			return FALSE;
		}
		break;
	case WM_INITDIALOG:
		SetTimer(hwndDlg, IDT_UPDATE, 1000, NULL);
		SendMessage(hwndDlg, WM_TIMER, 0, 0);
		hMain = LoadIcon(hModule, MAKEINTRESOURCE(IDI_MAIN));
		SendMessage(hwndDlg, WM_SETICON, TRUE, (LPARAM)hMain);
		SendMessage(hwndDlg, WM_SETICON, FALSE, (LPARAM)hMain);
		SendDlgItemMessage(hwndDlg, IDC_INJECT, BM_SETCHECK, BST_CHECKED, 0);
		break;
	case WM_TIMER:
		UpdateProcesses(hwndDlg);
		break;
	default:
		return FALSE;
		break;
	}
	return TRUE;
}

VOID GetMyDirectory(HMODULE hModule, PSZ pszDir)
{
    DWORD dwLength = GetModuleFileName(hModule, pszDir, MAX_PATH);
    for(; pszDir[dwLength] != '\\'; dwLength--)
		pszDir[dwLength] = '\0';
}

VOID UpdateProcesses(HWND hwndDlg)
{
	HWND hwndSelected;
	HWND hwndWow = NULL;
	DWORD dwIndex, dwSelectedIndex = 0;
	char szBuffer[100];
	char szWindowTitle[100];

	hwndSelected = (HWND)SendDlgItemMessage(hwndDlg, IDC_LIST, LB_GETITEMDATA, SendDlgItemMessage(hwndDlg, IDC_LIST, LB_GETCURSEL, 0, 0), 0);
	SendDlgItemMessage(hwndDlg, IDC_LIST, LB_RESETCONTENT, 0, 0);

	hwndWow = FindWindowEx(NULL, hwndWow, "GxWindowClassD3D", NULL);
	while(hwndWow)
	{
		GetWindowText(hwndWow, szWindowTitle, 100);
		wsprintf(szBuffer, "%d - %s", PtrToUlong(hwndWow), szWindowTitle);
		dwIndex = (DWORD)SendDlgItemMessage(hwndDlg, IDC_LIST, LB_ADDSTRING, 0, (LPARAM)szBuffer);
		SendDlgItemMessage(hwndDlg, IDC_LIST, LB_SETITEMDATA, dwIndex, (LPARAM)hwndWow);
		if(hwndWow == hwndSelected)
			dwSelectedIndex = dwIndex;
		hwndWow = FindWindowEx(NULL, hwndWow, "GxWindowClassD3D", NULL);
	}
	SendDlgItemMessage(hwndDlg, IDC_LIST, LB_SETCURSEL, dwSelectedIndex, 0);
	if(SendDlgItemMessage(hwndDlg, IDC_LIST, LB_GETCURSEL, 0, 0) == -1)
		EnableWindow(GetDlgItem(hwndDlg, IDC_INSTALL), FALSE);
	else
		EnableWindow(GetDlgItem(hwndDlg, IDC_INSTALL), TRUE);
}