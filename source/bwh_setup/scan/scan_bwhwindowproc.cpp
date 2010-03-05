#include "scan_bwhwindowproc.h"
#include "scan_common.h"
#include <commctrl.h>

#define SCAN_CATEGORY "BWH"
#define SCAN_NAME     "BwhWindowProc"
#define SCAN_REQUIRED TRUE

VOID scanBwhWindowProc_ScanProc(INT iItem, HWND hwndDlg, PBYTE pbFile)
{
	PIMAGE_NT_HEADERS pNtHdr = PIMAGE_NT_HEADERS(pbFile + PIMAGE_DOS_HEADER(pbFile)->e_lfanew);
    PIMAGE_SECTION_HEADER pSecHdr = PIMAGE_SECTION_HEADER(pNtHdr + 1);
    PBYTE pbCode = RvaToPointer(pbFile, pSecHdr[0].VirtualAddress);
    DWORD dwCodeSize = pSecHdr[0].SizeOfRawData;
	PBYTE pbRData = RvaToPointer(pbFile, pSecHdr[1].VirtualAddress);
	DWORD dwRDataSize = pSecHdr[1].SizeOfRawData;
	PBYTE pbData = RvaToPointer(pbFile, pSecHdr[2].VirtualAddress);
    DWORD dwDataSize = pSecHdr[2].SizeOfRawData;
	
	SetItemStatus(iItem, hwndDlg, "Scanning...");

	INT x;
	char szBwh[MAX_PATH];
	GetModuleFileName(GetModuleHandle(NULL), szBwh, MAX_PATH);
	for(x = lstrlen(szBwh); x && szBwh[x] != '\\'; x--);
	szBwh[x] = '\0';
	lstrcat(szBwh, "\\bwh.dll");
	HANDLE hFile = CreateFile(szBwh, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	DWORD dwRva = NULL;
	if(hFile != INVALID_HANDLE_VALUE)
	{
		PBYTE pbBwh = (PBYTE)VirtualAlloc(NULL, GetFileSize(hFile, NULL), MEM_COMMIT, PAGE_READWRITE);
		if(pbBwh)
		{
			DWORD dwRead;
			ReadFile(hFile, pbBwh, GetFileSize(hFile, NULL), &dwRead, NULL);
			PIMAGE_NT_HEADERS pBwhNtHdr = PIMAGE_NT_HEADERS(pbBwh + PIMAGE_DOS_HEADER(pbBwh)->e_lfanew);
			if(pBwhNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress)
			{
				PIMAGE_EXPORT_DIRECTORY  pBwhExportDir = PIMAGE_EXPORT_DIRECTORY(RvaToPointer(pbBwh, pBwhNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress));
				PDWORD pdwNameRvaArray = (PDWORD)RvaToPointer(pbBwh, pBwhExportDir->AddressOfNames);
				PDWORD pdwAddressRvaArray = (PDWORD)RvaToPointer(pbBwh, pBwhExportDir->AddressOfFunctions);
				PWORD pdwOrdinalArray = (PWORD)RvaToPointer(pbBwh, pBwhExportDir->AddressOfNameOrdinals);
				for(DWORD dwNameIndex = 0; dwNameIndex < pBwhExportDir->NumberOfNames; dwNameIndex++)
				{
					if(lstrcmpi((PSZ)RvaToPointer(pbBwh, pdwNameRvaArray[dwNameIndex]), "D3DWindowProc") == 0)
					{
						dwRva = pdwAddressRvaArray[pdwOrdinalArray[dwNameIndex]];
					}
				}
			}
			VirtualFree(pbBwh, 0, MEM_RELEASE);
		}
		CloseHandle(hFile);
	}
	if(dwRva)
	{
		GetPPD(iItem, hwndDlg)->dwValue = dwRva;
		SetItemStatus(iItem, hwndDlg, "Success");
	}
	else
	{
		GetPPD(iItem, hwndDlg)->dwValue = 0;
		SetItemStatus(iItem, hwndDlg, "Fail");
	}
}

VOID scanBwhWindowProc_Init(HWND hwndList)
{
	PPOINTER_DATA ppd;
	LVITEM lvi;
	
	ppd = (PPOINTER_DATA)HeapAlloc(GetProcessHeap(), 0, sizeof(POINTER_DATA));
	ppd->pszCategory = SCAN_CATEGORY;
	ppd->pszName = SCAN_NAME;
	ppd->bRequired = SCAN_REQUIRED;
	ppd->dwValue = 0;
	ppd->pfnScanProc = scanBwhWindowProc_ScanProc;

	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.pszText = SCAN_NAME;
	lvi.iSubItem = 0;
	lvi.iItem = ListView_GetItemCount(hwndList);
	lvi.lParam = (LPARAM)ppd;
	ListView_InsertItem(hwndList, &lvi);
}