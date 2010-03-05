#include <windows.h>
#include <commctrl.h>
#include "crc.h"
#include "hotkeys.h"
#include "fonts.h"
#include "scan\scan_common.h"
#include "scan\scan_present.h"
#include "scan\scan_reset.h"
#include "scan\scan_wowwindowproc.h"
#include "scan\scan_bwhwindowproc.h"
#include "scan\scan_getmap.h"
#include "scan\scan_getguiddata.h"
#include "scan\scan_getguiddataconstant1.h"
#include "scan\scan_getguiddataconstant2.h"
#include "scan\scan_getcharacterguid.h"
#include "scan\scan_fallingteleport.h"
#include "scan\scan_cameraangle.h"
#include "scan\scan_sendchatmessage.h"
#include "scan\scan_trackinghook.h"
#include "scan\scan_trackingjumpsize.h"
#include "scan\scan_trackingtrackstructoffset.h"
#include "scan\scan_trackingtrackdataoffset.h"
#include "scan\scan_trackingcharstruct2offset.h"
#include "scan\scan_trackingchartypeoffset.h"
#include "scan\scan_trackingnpcdataoffset.h"
#include "scan\scan_namelookup.h"
#include "scan\scan_corpse.h"
#include "scan\scan_nofalldamagehook.h"
#include "scan\scan_nofalldamageoffset.h"
#include "scan\scan_zerogravity.h"
#include "scan\scan_mountainclimb.h"
#include "scan\scan_lockspeed.h"
#include "scan\scan_teleporttoplane.h"
#include "scan\scan_follownpc1.h"
#include "scan\scan_follownpc2.h"
#include "scan\scan_followfaction.h"
#include "scan\scan_followdistance.h"
#include "resource.h"

#undef RtlZeroMemory
extern "C" WINBASEAPI void WINAPI RtlZeroMemory(PVOID Destination, SIZE_T Length);

HMODULE g_hinstApp;
HANDLE g_hScanProc = NULL;
char g_szFixedFont[100];
LONG g_lFixedSize;
BOOL g_bFixedBold;
char g_szProportionalFont[100];
LONG g_lProportionalSize;
BOOL g_bProportionalBold;

VOID FindWOW(HWND hwndDlg)
{
    HKEY hKey;
    char szValue[MAX_PATH];
    DWORD dwSize = MAX_PATH;
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Blizzard Entertainment\\World of Warcraft", 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
    {
        if(RegQueryValueEx(hKey, "GamePath", NULL, NULL, (LPBYTE)szValue, &dwSize) == ERROR_SUCCESS)
            SendDlgItemMessage(hwndDlg, IDC_FILENAME, WM_SETTEXT, 0, (LPARAM)szValue);
        RegCloseKey(hKey);
    }
}

VOID GetFilename(HWND hwndDlg)
{
    char szFile[MAX_PATH];
    szFile[0] = 0;

    OPENFILENAME ofn;
    RtlZeroMemory(&ofn, sizeof(ofn));
    ofn.hwndOwner = hwndDlg;
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrTitle = "Locate wow.exe";
    ofn.Flags = OFN_FILEMUSTEXIST;
    if(GetOpenFileName(&ofn))
    {
        SendDlgItemMessage(hwndDlg, IDC_FILENAME, WM_SETTEXT, 0, (LPARAM)ofn.lpstrFile);
    }
}

VOID Init(HWND hwndDlg)
{
	char szINI[MAX_PATH];
	INT x;
	LVCOLUMN lvc;
	HWND hwndList = GetDlgItem(hwndDlg, IDC_POINTERS);
	HWND hwndHotkeys = GetDlgItem(hwndDlg, IDC_HOTKEYS);

	// set icon
	HICON hMain = LoadIcon(g_hinstApp, MAKEINTRESOURCE(IDI_MAIN));
    SendMessage(hwndDlg, WM_SETICON, TRUE, (LPARAM)hMain);
    SendMessage(hwndDlg, WM_SETICON, FALSE, (LPARAM)hMain);

	// setup pointer list box
	ListView_SetExtendedListViewStyle(hwndList, LVS_EX_FULLROWSELECT);
	lvc.mask = LVCF_WIDTH;
	lvc.cx = 140;
	ListView_InsertColumn(hwndList, 0, &lvc);
	lvc.cx = 70;
	ListView_InsertColumn(hwndList, 1, &lvc);

	// setup hotkey list box
	ListView_SetExtendedListViewStyle(hwndHotkeys, LVS_EX_FULLROWSELECT);
	lvc.mask = LVCF_WIDTH;
	lvc.cx = 110;
	ListView_InsertColumn(hwndHotkeys, 0, &lvc);
	lvc.cx = 100;
	ListView_InsertColumn(hwndHotkeys, 1, &lvc);

	// setup buttons
	EnableWindow(GetDlgItem(hwndDlg, IDC_SCAN), FALSE);
	EnableWindow(GetDlgItem(hwndDlg, IDC_SAVE), FALSE);

	// add pointers
	scanPresent_Init(hwndList);
	scanReset_Init(hwndList);
	scanWowWindowProc_Init(hwndList);
	scanBwhWindowProc_Init(hwndList);
	scanGetMap_Init(hwndList);
	scanGetGuidData_Init(hwndList);
	scanGetGuidDataConstant1_Init(hwndList);
	scanGetGuidDataConstant2_Init(hwndList);
	scanGetCharacterGuid_Init(hwndList);
	scanFallingTeleport_Init(hwndList);
	scanCameraAngle_Init(hwndList);
	scanSendChatMessage_Init(hwndList);
	scanTrackingHook_Init(hwndList);
	scanTrackingJumpSize_Init(hwndList);
	scanTrackingTrackStructOffset_Init(hwndList);
	scanTrackingTrackDataOffset_Init(hwndList);
	scanTrackingCharStruct2Offset_Init(hwndList);
	scanTrackingCharTypeOffset_Init(hwndList);
	scanTrackingNpcDataOffset_Init(hwndList);
	scanNameLookup_Init(hwndList);
	scanCorpse_Init(hwndList);
	scanNoFallDamageHook_Init(hwndList);
	scanNoFallDamageOffset_Init(hwndList);
	scanZeroGravity_Init(hwndList);
	scanMountainClimb_Init(hwndList);
	scanLockSpeed_Init(hwndList);
	scanTeleportToPlane_Init(hwndList);
	scanFollowNpc1_Init(hwndList);
	scanFollowNpc2_Init(hwndList);
	scanFollowFaction_Init(hwndList);
	scanFollowDistance_Init(hwndList);

	// add hotkeys
	AddHotkey(hwndHotkeys, "ShowHide", VK_F12);
	AddHotkey(hwndHotkeys, "X-Plus", VK_NUMPAD6);
	AddHotkey(hwndHotkeys, "X-Minus", VK_NUMPAD4);
	AddHotkey(hwndHotkeys, "Y-Plus", VK_NUMPAD8);
	AddHotkey(hwndHotkeys, "Y-Minus", VK_NUMPAD2);
	AddHotkey(hwndHotkeys, "Z-Plus", VK_NUMPAD9);
	AddHotkey(hwndHotkeys, "Z-Minus", VK_NUMPAD3);
	AddHotkey(hwndHotkeys, "Speed-Plus", VK_NUMPAD7);
	AddHotkey(hwndHotkeys, "Speed-Minus", VK_NUMPAD1);
	AddHotkey(hwndHotkeys, "NormalSpeed", VK_NUMPAD5);
	AddHotkey(hwndHotkeys, "Teleport", VK_MULTIPLY);
	AddHotkey(hwndHotkeys, "QuickSave", VK_DECIMAL);
	AddHotkey(hwndHotkeys, "QuickLoad", VK_NUMPAD0);
	AddHotkey(hwndHotkeys, "TrackAlliance", CTRL | ALT | 'A');
	AddHotkey(hwndHotkeys, "TrackHorde", CTRL | ALT | 'H');
	AddHotkey(hwndHotkeys, "TrackBeasts", CTRL | ALT | '1');
	AddHotkey(hwndHotkeys, "TrackDemons", CTRL | ALT | '2');
	AddHotkey(hwndHotkeys, "TrackDragons", CTRL | ALT | '3');
	AddHotkey(hwndHotkeys, "TrackElementals", CTRL | ALT | '4');
	AddHotkey(hwndHotkeys, "TrackGiants", CTRL | ALT | '5');
	AddHotkey(hwndHotkeys, "TrackHumanoids", CTRL | ALT | '6');
	AddHotkey(hwndHotkeys, "TrackUndead", CTRL | ALT | '7');
	AddHotkey(hwndHotkeys, "TrackMachines", CTRL | ALT | '8');
	AddHotkey(hwndHotkeys, "TrackSlimes", CTRL | ALT | '9');
	AddHotkey(hwndHotkeys, "TrackCritters", CTRL | ALT | '0');
	AddHotkey(hwndHotkeys, "TrackTreasure", CTRL | ALT | 'T');
	AddHotkey(hwndHotkeys, "TrackObjects", CTRL | ALT | 'O');
	AddHotkey(hwndHotkeys, "TrackHerbs", CTRL | ALT | 'F');
	AddHotkey(hwndHotkeys, "TrackMinerals", CTRL | ALT | 'M');
	AddHotkey(hwndHotkeys, "NoFallDamage", CTRL | 'N');
	AddHotkey(hwndHotkeys, "MountainClimb", CTRL | 'M');
	AddHotkey(hwndHotkeys, "LockSpeed", CTRL | 'L');
	AddHotkey(hwndHotkeys, "ZeroGravity", CTRL | 'Z');
	AddHotkey(hwndHotkeys, "TeleportToPlane", CTRL | 'T');
	AddHotkey(hwndHotkeys, "FollowNpc", CTRL | 'F');
	AddHotkey(hwndHotkeys, "GoToCorpse", CTRL | 'C');
	AddHotkey(hwndHotkeys, "GPS", CTRL | 'G');

	// setup font boxes
	GetModuleFileName(GetModuleHandle(NULL), szINI, MAX_PATH);
	for(x = lstrlen(szINI); x && szINI[x] != '\\'; x--);
	szINI[x] = '\0';
	lstrcat(szINI, "\\bwh.ini");

	GetPrivateProfileString("FONTS", "FixedFont", "Terminal", g_szFixedFont, sizeof(g_szFixedFont), szINI);
	g_lFixedSize = GetPrivateProfileInt("FONTS", "FixedSize", 9, szINI);
	g_bFixedBold = GetPrivateProfileInt("FONTS", "FixedBold", FALSE, szINI);
	GetPrivateProfileString("FONTS", "ProportionalFont", "Arial", g_szProportionalFont, sizeof(g_szProportionalFont), szINI);
	g_lProportionalSize = GetPrivateProfileInt("FONTS", "ProportionalSize", 8, szINI);
	g_bProportionalBold = GetPrivateProfileInt("FONTS", "ProportionalBold", FALSE, szINI);

	SetFixedWidthFont(hwndDlg, g_szFixedFont, g_lFixedSize, g_bFixedBold);
	SetProportionalFont(hwndDlg, g_szProportionalFont, g_lProportionalSize, g_bProportionalBold);

	// fill in wow path	
	FindWOW(hwndDlg);
}

VOID ClearStatus(HWND hwndDlg)
{
	LVITEM lvi;
	HWND hwndList = GetDlgItem(hwndDlg, IDC_POINTERS);
	
	lvi.mask = LVIF_TEXT;
	lvi.iSubItem = 1;
	lvi.pszText = "";
	for(lvi.iItem = 0; lvi.iItem < ListView_GetItemCount(hwndList); lvi.iItem++)
		ListView_SetItem(hwndList, &lvi);
}

VOID HandlePointerErrors(HWND hwndDlg)
{
	HWND hwndList = GetDlgItem(hwndDlg, IDC_POINTERS);
	LVITEM lvi;
	BOOL bWarn = FALSE, bCrit = FALSE;

	lvi.mask = LVIF_PARAM;
	lvi.iSubItem = 0;
	for(lvi.iItem = 0; lvi.iItem < ListView_GetItemCount(hwndList); lvi.iItem++)
	{
		ListView_GetItem(hwndList, &lvi);
		if(PPOINTER_DATA(lvi.lParam)->dwValue == NULL)
		{
			if(PPOINTER_DATA(lvi.lParam)->bRequired == TRUE)
				bCrit = TRUE;
			else
				bWarn = TRUE;
		}
	}
	if(bCrit)
		MessageBox(hwndDlg, "One or more required pointers were not found!\nBWH cannot be run without these pointers.\nYou will need to find these pointers manually or get an updated version of this program.", "BWH Setup", MB_ICONERROR);
	else
	{
		if(bWarn)
			MessageBox(hwndDlg, "One or more optional pointers were not found!\nBWH can still run but will be missing some features.\nYou should try finding these pointers manually or looking for an updated version of this program.", "BWH Setup", MB_ICONWARNING);
		EnableWindow(GetDlgItem(hwndDlg, IDC_SAVE), TRUE);
	}
}

DWORD WINAPI ScanProc(HWND hwndDlg)
{   
    char szFile[MAX_PATH];
    HANDLE hFile, hMap;
    HWND hwndList = GetDlgItem(hwndDlg, IDC_POINTERS);
	PBYTE pbFile;

	EnableWindow(GetDlgItem(hwndDlg, IDC_SCAN), FALSE);
	EnableWindow(GetDlgItem(hwndDlg, IDC_SAVE), FALSE);
	ClearStatus(hwndDlg);

    SendDlgItemMessage(hwndDlg, IDC_FILENAME, WM_GETTEXT, MAX_PATH, (LPARAM)szFile);
    hFile = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if(hFile != INVALID_HANDLE_VALUE)
    {
        hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
        if(hMap)
		{
            pbFile = (PBYTE)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
			if(pbFile)
			{
				for(INT x = 0; x < ListView_GetItemCount(hwndList); x++)
				{
					ListView_EnsureVisible(hwndList, x, FALSE);
					ListView_SetItemState(hwndList, x, LVIS_SELECTED, LVIS_SELECTED);
					GetPPD(x, hwndDlg)->pfnScanProc(x, hwndDlg, pbFile);
				}
				
				HandlePointerErrors(hwndDlg);

				UnmapViewOfFile(pbFile);
			}
			else
				MessageBox(hwndDlg, "Could not gain read access to the file!", "BWH Setup", MB_ICONERROR);
			CloseHandle(hMap);
		}
		else
			MessageBox(hwndDlg, "Could not gain read access to the file!", "BWH Setup", MB_ICONERROR);
		CloseHandle(hFile);
    }
	else
		MessageBox(hwndDlg, "Could not gain read access to the file!", "BWH Setup", MB_ICONERROR);

    EnableWindow(GetDlgItem(hwndDlg, IDC_SCAN), TRUE);
    CloseHandle(g_hScanProc);
    g_hScanProc = NULL;
    return 0;
}

VOID TestFile(HWND hwndDlg)
{
    char szFile[MAX_PATH];
    HANDLE hFile;   
    
	if(g_hScanProc)
    {
        TerminateThread(g_hScanProc, 0);
        g_hScanProc = NULL;
    }

	ClearStatus(hwndDlg);

	SendDlgItemMessage(hwndDlg, IDC_FILENAME, WM_GETTEXT, MAX_PATH, (LPARAM)szFile);
    hFile = CreateFile(szFile, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if(hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
        EnableWindow(GetDlgItem(hwndDlg, IDC_SCAN), TRUE);
    }
    else
    {
        EnableWindow(GetDlgItem(hwndDlg, IDC_SCAN), FALSE);
    }
    EnableWindow(GetDlgItem(hwndDlg, IDC_SAVE), FALSE);
}

VOID ViewPointer(HWND hwndDlg)
{
	char szBuffer[100];
	HWND hwndList = GetDlgItem(hwndDlg, IDC_POINTERS);
	LVITEM lvi;

	lvi.iItem = ListView_GetNextItem(hwndList, -1, LVNI_SELECTED);
	if(lvi.iItem != -1)
	{
		lvi.mask = LVIF_PARAM;
		lvi.iSubItem = 0;
		ListView_GetItem(hwndList, &lvi);

		wsprintf(szBuffer, "Value of %s is 0x%X.", PPOINTER_DATA(lvi.lParam)->pszName, PPOINTER_DATA(lvi.lParam)->dwValue);
		MessageBox(hwndDlg, szBuffer, "bwh_setup", MB_ICONINFORMATION);
	}
}

VOID FreeLists(HWND hwndDlg)
{
	HWND hwndPointers = GetDlgItem(hwndDlg, IDC_POINTERS);
	HWND hwndHotkeys = GetDlgItem(hwndDlg, IDC_HOTKEYS);
	LVITEM lvi;
	
	lvi.mask = LVIF_PARAM;
	lvi.iSubItem = 0;

	for(lvi.iItem = 0; lvi.iItem < ListView_GetItemCount(hwndPointers); lvi.iItem++)
	{
		ListView_GetItem(hwndPointers, &lvi);
		HeapFree(GetProcessHeap(), 0, (PVOID)lvi.lParam);
	}

	for(lvi.iItem = 0; lvi.iItem < ListView_GetItemCount(hwndHotkeys); lvi.iItem++)
	{
		ListView_GetItem(hwndHotkeys, &lvi);
		HeapFree(GetProcessHeap(), 0, (PVOID)lvi.lParam);
	}
}

VOID SaveINI(HWND hwndDlg)
{
	BOOL bStop = FALSE;
	char szBuffer[100];
	char szINI[MAX_PATH];
	char szFilename[MAX_PATH];
	INT x;
	LVITEM lvi;
	HWND hwndPointers = GetDlgItem(hwndDlg, IDC_POINTERS);
	HWND hwndHotkeys = GetDlgItem(hwndDlg, IDC_HOTKEYS);
	lvi.mask = LVIF_PARAM;
	lvi.iSubItem = 0;

	GetModuleFileName(GetModuleHandle(NULL), szINI, MAX_PATH);
	for(x = lstrlen(szINI); x && szINI[x] != '\\'; x--);
	szINI[x] = '\0';
	lstrcat(szINI, "\\bwh.ini");

	// save wow exe location
	GetDlgItemText(hwndDlg, IDC_FILENAME, szFilename, MAX_PATH);
	WritePrivateProfileString("WOW", "EXE", szFilename, szINI);
	
	// save pointers
	for(lvi.iItem = 0; lvi.iItem < ListView_GetItemCount(hwndPointers); lvi.iItem++)
	{
		ListView_GetItem(hwndPointers, &lvi);
		wsprintf(szBuffer, "%d", PPOINTER_DATA(lvi.lParam)->dwValue);
		WritePrivateProfileString(PPOINTER_DATA(lvi.lParam)->pszCategory, PPOINTER_DATA(lvi.lParam)->pszName, szBuffer, szINI);
	}

	// save hotkeys
	for(lvi.iItem = 0; lvi.iItem < ListView_GetItemCount(hwndHotkeys); lvi.iItem++)
	{
		ListView_GetItem(hwndHotkeys, &lvi);
		wsprintf(szBuffer, "%d", PHOTKEY_DATA(lvi.lParam)->dwValue);
		WritePrivateProfileString("HOTKEYS", PHOTKEY_DATA(lvi.lParam)->pszName, szBuffer, szINI);
	}

	// save fonts
	WritePrivateProfileString("FONTS", "FixedFont", g_szFixedFont, szINI);
	wsprintf(szBuffer, "%d", g_lFixedSize);
	WritePrivateProfileString("FONTS", "FixedSize", szBuffer, szINI);
	wsprintf(szBuffer, "%d", g_bFixedBold);
	WritePrivateProfileString("FONTS", "FixedBold", szBuffer, szINI);
	WritePrivateProfileString("FONTS", "ProportionalFont", g_szProportionalFont, szINI);
	wsprintf(szBuffer, "%d", g_lProportionalSize);
	WritePrivateProfileString("FONTS", "ProportionalSize", szBuffer, szINI);
	wsprintf(szBuffer, "%d", g_bProportionalBold);
	WritePrivateProfileString("FONTS", "ProportionalBold", szBuffer, szINI);

	// save crc's
	wsprintf(szBuffer, "%d", GetFileCRC(szFilename, &bStop, NULL));
	WritePrivateProfileString("WOW", "CRC", szBuffer, szINI);

	GetSystemDirectory(szFilename, MAX_PATH);
	lstrcat(szFilename, "\\d3d9.dll");
	wsprintf(szBuffer, "%d", GetFileCRC(szFilename, &bStop, NULL));
	WritePrivateProfileString("D3D", "CRC", szBuffer, szINI);

	lstrcpy(szFilename, szINI);
	szFilename[lstrlen(szFilename) - 3] = '\0';
	lstrcat(szFilename, "dll");
	wsprintf(szBuffer, "%d", GetFileCRC(szFilename, &bStop, NULL));
	WritePrivateProfileString("BWH", "CRC", szBuffer, szINI);
	

	MessageBox(hwndDlg, "Saved settings to bwh.ini", "bwh_setup", MB_ICONINFORMATION);
}

INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WORD wCode, wId;
	DWORD dwThreadId;
    switch(uMsg)
    {
    case WM_CLOSE:
        if(g_hScanProc)
            TerminateThread(g_hScanProc, 0);
        FreeLists(hwndDlg);
		FreeFonts();
		EndDialog(hwndDlg, 0);
        break;
    case WM_COMMAND:
        wId = LOWORD(wParam);
        wCode = HIWORD(wParam);
        switch(wId)
        {
		case IDC_BROWSE:
			if(wCode == BN_CLICKED)
				GetFilename(hwndDlg);
			break;
        case IDC_SCAN:
            if(wCode == BN_CLICKED)
				g_hScanProc = CreateThread(NULL, 0, (PTHREAD_START_ROUTINE)ScanProc, hwndDlg, NULL, &dwThreadId);
            break;
		case IDC_FILENAME:
			if(wCode == EN_CHANGE)
				TestFile(hwndDlg);
			break;
		case IDC_VIEW:
			if(wCode == BN_CLICKED)
				ViewPointer(hwndDlg);
			break;
		case IDC_DEFAULT:
			if(wCode == BN_CLICKED)
				SetHotkey(GetDlgItem(hwndDlg, IDC_HOTKEYS), SET_DEFAULT);
			break;
		case IDC_UNBIND:
			if(wCode == BN_CLICKED)
				SetHotkey(GetDlgItem(hwndDlg, IDC_HOTKEYS), SET_UNBIND);
			break;
		case IDC_ASSIGN:
			if(wCode == BN_CLICKED)
				SetHotkey(GetDlgItem(hwndDlg, IDC_HOTKEYS), GetHotkey(hwndDlg));
			break;
		case IDC_SETFIXED:
			if(wCode == BN_CLICKED)
			{
				if(SelectFont(hwndDlg, g_szFixedFont, &g_lFixedSize, &g_bFixedBold))
					SetFixedWidthFont(hwndDlg, g_szFixedFont, g_lFixedSize, g_bFixedBold);
			}
			break;
		case IDC_SETPROPORTIONAL:
			if(wCode == BN_CLICKED)
			{
				if(SelectFont(hwndDlg, g_szProportionalFont, &g_lProportionalSize, &g_bProportionalBold))
					SetProportionalFont(hwndDlg, g_szProportionalFont, g_lProportionalSize, g_bProportionalBold);
			}
			break;
		case IDC_SAVE:
			if(wCode == BN_CLICKED)
				SaveINI(hwndDlg);
			break;
		}
       break;
	case WM_INITDIALOG:
		Init(hwndDlg);
		break;
    default:
        return FALSE;
        break;
    }
    return TRUE;
}

void WinMainCRTStartup()
{
    InitCommonControls();
    g_hinstApp = GetModuleHandle(NULL);
    ExitProcess((UINT)DialogBoxParam(g_hinstApp, MAKEINTRESOURCE(IDD_MAIN), NULL, &DialogProc, NULL));
}