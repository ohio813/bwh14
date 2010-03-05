#include "scan_reset.h"
#include "scan_common.h"
#include <commctrl.h>
#include <d3d9.h>

#undef RtlZeroMemory
extern "C" WINBASEAPI void WINAPI RtlZeroMemory(PVOID Destination, SIZE_T Length);

#define SCAN_CATEGORY "D3D"
#define SCAN_NAME     "Reset"
#define SCAN_REQUIRED TRUE

VOID scanReset_ScanProc(INT iItem, HWND hwndDlg, PBYTE pbFile)
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

	/* enter scan algorithim here */
	DWORD dwResetRVA = NULL;
	LPDIRECT3D9 pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(pD3D)
	{
		IDirect3DDevice9 *pDevice;
		D3DPRESENT_PARAMETERS pp;
		RtlZeroMemory(&pp, sizeof(pp));
		pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		pp.Windowed = TRUE;
		if(SUCCEEDED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwndDlg, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &pDevice)))
		{
			__asm mov eax, pDevice
			__asm mov eax, [eax]
			__asm mov eax, [eax+40h]
			__asm mov dwResetRVA, eax
			pDevice->Release();
		}
		pD3D->Release();
	}
	if(dwResetRVA)
	{
		dwResetRVA -= PtrToUlong(GetModuleHandle("d3d9.dll"));
		GetPPD(iItem, hwndDlg)->dwValue = dwResetRVA;
		SetItemStatus(iItem, hwndDlg, "Success");
	}
	else
	{
		GetPPD(iItem, hwndDlg)->dwValue = 0;
		SetItemStatus(iItem, hwndDlg, "Fail");
	}
}

VOID scanReset_Init(HWND hwndList)
{
	PPOINTER_DATA ppd;
	LVITEM lvi;
	
	ppd = (PPOINTER_DATA)HeapAlloc(GetProcessHeap(), 0, sizeof(POINTER_DATA));
	ppd->pszCategory = SCAN_CATEGORY;
	ppd->pszName = SCAN_NAME;
	ppd->bRequired = SCAN_REQUIRED;
	ppd->dwValue = 0;
	ppd->pfnScanProc = scanReset_ScanProc;

	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.pszText = SCAN_NAME;
	lvi.iSubItem = 0;
	lvi.iItem = ListView_GetItemCount(hwndList);
	lvi.lParam = (LPARAM)ppd;
	ListView_InsertItem(hwndList, &lvi);
}