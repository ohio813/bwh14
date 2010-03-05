#include "scan_common.h"
#include <commctrl.h>
#include "..\resource.h"

PBYTE RvaToPointer(PBYTE pbImage, DWORD dwRva)
{
    DWORD dwSecBorder = -1;
    PIMAGE_NT_HEADERS pNt = PIMAGE_NT_HEADERS(pbImage + PIMAGE_DOS_HEADER(pbImage)->e_lfanew);
    PIMAGE_SECTION_HEADER pSec = PIMAGE_SECTION_HEADER(PBYTE(pNt) + sizeof(IMAGE_NT_HEADERS));
    for(DWORD x = 0; x < pNt->FileHeader.NumberOfSections; x++)
    {
        if(dwRva >= pSec[x].VirtualAddress && dwRva < pSec[x].VirtualAddress + pSec[x].Misc.VirtualSize)
            return (pbImage + (dwRva - pSec[x].VirtualAddress + pSec[x].PointerToRawData));
        if(pSec[x].PointerToRawData && pSec[x].PointerToRawData < dwSecBorder)
            dwSecBorder = pSec[x].PointerToRawData;
    }
    if(dwRva < dwSecBorder)
        return pbImage + dwRva;
    else
        return NULL;
}

PPOINTER_DATA GetPPD(INT iItem, HWND hwndDlg)
{
	LVITEM lvi;
	HWND hwndList = GetDlgItem(hwndDlg, IDC_POINTERS);

	lvi.mask = LVIF_PARAM;
	lvi.iItem = iItem;
	lvi.iSubItem = 0;
	ListView_GetItem(hwndList, &lvi);
	return (PPOINTER_DATA)lvi.lParam;
}

VOID SetItemStatus(INT iItem, HWND hwndDlg, PSZ pszStatus)
{
	LVITEM lvi;
	HWND hwndList = GetDlgItem(hwndDlg, IDC_POINTERS);

	lvi.mask = LVIF_TEXT;
	lvi.iItem = iItem;
	lvi.iSubItem = 1;
	lvi.pszText = pszStatus;
	ListView_SetItem(hwndList, &lvi);
}
BOOL MemoryCompare(PBYTE pb1, PBYTE pb2, DWORD dwSize)
{
    for(DWORD x = 0; x < dwSize; x++)
    {
        if(pb1[x] != pb2[x])
            return FALSE;
    }
    return TRUE;
}

DWORD OffsetToRva(PBYTE pbImage, DWORD dwOffset)
{
    DWORD dwSecBorder = -1;
    PIMAGE_NT_HEADERS pNt = PIMAGE_NT_HEADERS(pbImage + PIMAGE_DOS_HEADER(pbImage)->e_lfanew);
    PIMAGE_SECTION_HEADER pSec = PIMAGE_SECTION_HEADER(PBYTE(pNt) + sizeof(IMAGE_NT_HEADERS));
    for(DWORD x = 0; x < pNt->FileHeader.NumberOfSections; x++)
    {
        if(dwOffset >= pSec[x].PointerToRawData && dwOffset < pSec[x].PointerToRawData + pSec[x].SizeOfRawData)
            return (dwOffset - pSec[x].PointerToRawData + pSec[x].VirtualAddress);
        if(pSec[x].PointerToRawData && pSec[x].PointerToRawData < dwSecBorder)
            dwSecBorder = pSec[x].PointerToRawData;
    }
    if(dwOffset < dwSecBorder)
        return dwOffset;
    else
        return NULL;
}
