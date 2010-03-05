#include "hooker.h"
#include "interface.h"
#include "hacks\teleport.h"

// function prototypes
BOOL             HookD3DFunction(DWORD dwRVA, PVOID pvStub);
LRESULT CALLBACK HookedWindowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID             PresentStub();
VOID             ResetStub();
VOID             UnHookD3DFunction(DWORD dwRVA);
DWORD WINAPI     WaitForNewWindow(LPVOID lpParameter);

// global variables
WNDPROC       g_wpOrigWndProc = NULL;
PFNWINDOWPROC g_pfnWindowProc;
HANDLE        g_hWaitProc = NULL;
HWND          g_hwndHook;

// code start
PCDXCtrlManager HookD3D(HMODULE hModule, DWORD dwPresentRVA, DWORD dwResetRVA, PSZ pszFixedFont, INT iFixedSize, BOOL bFixedBold, PSZ pszProportionalFont, INT iProportionalSize, BOOL bProportionalBold)
{
	PCDXCtrlManager pManager = Interface_InitControls(g_hwndHook, hModule, pszFixedFont, iFixedSize, bFixedBold, pszProportionalFont, iProportionalSize, bProportionalBold);
	if( pManager &&
		HookD3DFunction(dwPresentRVA, PresentStub) &&
	    HookD3DFunction(dwResetRVA, ResetStub))
	{
		Teleport_SetWindow(g_hwndHook, 500);
		return pManager;
	}

	Interface_DeleteControls();
	UnHookD3DFunction(dwPresentRVA);
	UnHookD3DFunction(dwResetRVA);
	return FALSE;
}

BOOL HookD3DFunction(DWORD dwRVA, PVOID pvStub)
{
	HMODULE hD3D9;
	PBYTE pbFuncAddress;
	DWORD dwOldProtect;
	BOOL bRet = FALSE;

	hD3D9 = GetModuleHandle("d3d9.dll");
	if(hD3D9)
	{
		pbFuncAddress = (PBYTE)UlongToPtr(PtrToUlong(hD3D9) + dwRVA);
		if(VirtualProtect(pbFuncAddress, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect))
		{
			if(memcmp(pbFuncAddress, "\x8B\xFF\x55\x8B\xEC", 5) == 0)
			{
				*pbFuncAddress = 0xE8;
				*((PDWORD)&pbFuncAddress[1]) = PtrToUlong(pvStub) - (PtrToUlong(pbFuncAddress) + 5);
				bRet = TRUE;
			}
			VirtualProtect(pbFuncAddress, 5, dwOldProtect, &dwOldProtect);
		}
	}
	return bRet;
}

LRESULT CALLBACK HookedWindowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_DESTROY)
	{
		Interface_FreeWindow();
		Teleport_FreeWindow(hwnd);
		UnHookWindowProcedure();
		g_hWaitProc = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WaitForNewWindow, hwnd, NULL, NULL);
	}
	else if(g_pfnWindowProc(hwnd, uMsg, wParam, lParam))
		return TRUE;

	return CallWindowProc(g_wpOrigWndProc, hwnd, uMsg, wParam, lParam);
}

BOOL HookWindowProcedure(PFNWINDOWPROC pfnWindowProc)
{
    DWORD dwWindowPId, dwMyPId = GetCurrentProcessId();
	HWND hwndCurrent = NULL;
    
	if(g_wpOrigWndProc == NULL)
    {
        do
        {
            hwndCurrent = FindWindowEx(NULL, hwndCurrent, "GxWindowClassD3d", NULL);
            GetWindowThreadProcessId(hwndCurrent, &dwWindowPId);
            if(dwWindowPId == dwMyPId)
            {
                g_wpOrigWndProc = (WNDPROC)LongToPtr(SetWindowLong(hwndCurrent, GWL_WNDPROC, PtrToLong(HookedWindowProcedure)));
                g_pfnWindowProc = pfnWindowProc;
				g_hwndHook = hwndCurrent;
				if(g_wpOrigWndProc)
                    return TRUE;
                else
                    return FALSE;
            }
        } while(hwndCurrent);
    }
    return FALSE;
}

__declspec(naked) VOID PresentStub()
{
	__asm
	{
		push dword ptr[esp+8]
		call dword ptr[Interface_PresentHook]
		pop eax
		mov edi, edi
		push ebp
		mov ebp, esp
		jmp eax
	}
}

__declspec(naked) VOID ResetStub()
{
	__asm
	{
		call dword ptr[Interface_ResetHook]
		pop eax
		mov edi, edi
		push ebp
		mov ebp, esp
		jmp eax
	}
}

VOID UnHookD3D(DWORD dwPresentRVA, DWORD dwResetRVA)
{
	UnHookD3DFunction(dwPresentRVA);
	UnHookD3DFunction(dwResetRVA);
	Interface_DeleteControls();
}

VOID UnHookD3DFunction(DWORD dwRVA)
{
	HMODULE hD3D9;
	PBYTE pbFuncAddress;
	DWORD dwOldProtect;

	hD3D9 = GetModuleHandle("d3d9.dll");
	if(hD3D9)
	{
		pbFuncAddress = (PBYTE)UlongToPtr(PtrToUlong(hD3D9) + dwRVA);
		VirtualProtect(pbFuncAddress, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);
		memcpy(pbFuncAddress, "\x8B\xFF\x55\x8B\xEC", 5);
		VirtualProtect(pbFuncAddress, 5, dwOldProtect, &dwOldProtect);
	}
}

VOID UnHookWindowProcedure()
{
    DWORD dwWindowPId, dwMyPId = GetCurrentProcessId();
    HWND hwndCurrent = NULL;

    if(g_wpOrigWndProc)
    {
        if(g_hWaitProc)
		{
            TerminateThread(g_hWaitProc, 0);
			g_hWaitProc = NULL;
		}

        do
        {
            hwndCurrent = FindWindowEx(NULL, hwndCurrent, "GxWindowClassD3d", NULL);
            GetWindowThreadProcessId(hwndCurrent, &dwWindowPId);
            if(dwWindowPId == dwMyPId)
            {
                SetWindowLong(hwndCurrent, GWL_WNDPROC, PtrToLong(g_wpOrigWndProc));
                g_wpOrigWndProc = NULL;
                return;
            }
        } while(hwndCurrent);
    }
}

DWORD WINAPI WaitForNewWindow(LPVOID lpParameter)
{
    while(IsWindow((HWND)lpParameter));
    
	while(HookWindowProcedure(g_pfnWindowProc) == FALSE)
        Sleep(1000);
    
	Interface_SetWindow(g_hwndHook);
	Teleport_SetWindow(g_hwndHook, 500);

	CloseHandle(g_hWaitProc);
    g_hWaitProc = NULL;
  
	return 0;
}
//////////
#pragma pack(push)
#pragma pack(1)
typedef struct _CALLINSTRUCTION
{
	BYTE bOpcode;
	DWORD dwAddress;
	DWORD dwNop;
} CALLINSTRUCTION, *PCALLINSTRUCTION;
#pragma pack(pop)

BYTE g_bOrigChatAddressBytes[] = {0x55, 0x8B, 0xEC, 0x81, 0xEC};
DWORD g_dwOrigChatStackBytes;
PFNRECVCHATMESSAGE g_pRecvFunc;
__declspec(naked) VOID ChatStub()
{
	__asm
	{
		pushad
		mov eax, dword ptr[ecx+0Ch]
		cmp dword ptr[eax], 4
		jne procmsg
		mov eax, dword ptr[eax+08h]
		add eax, 10h
		push eax
		call dword ptr[g_pRecvFunc]
		test eax, eax
		jz procmsg
		popad
		xor eax, eax
		add esp, 4
		ret
procmsg:popad
		pop eax
		push ebp
		mov ebp, esp
		sub esp, dword ptr[g_dwOrigChatStackBytes]
		jmp eax
	}
}

VOID HookChatFunction(PVOID pChatAddress, PFNRECVCHATMESSAGE pRecvFunc)
{
	DWORD dwOldProtect;
	CALLINSTRUCTION call = {0xE8, PtrToUlong(ChatStub) - (PtrToUlong(pChatAddress) + sizeof(g_bOrigChatAddressBytes)), 0x90909090};
	if( pChatAddress == NULL || (memcmp(pChatAddress, g_bOrigChatAddressBytes, sizeof(g_bOrigChatAddressBytes)) != 0))
		return;
	
	if(VirtualProtect(pChatAddress, sizeof(CALLINSTRUCTION), PAGE_EXECUTE_READWRITE, &dwOldProtect))
	{
		g_dwOrigChatStackBytes = *((PDWORD)((PBYTE)pChatAddress + sizeof(g_bOrigChatAddressBytes)));
		memcpy(pChatAddress, &call, sizeof(call));
		VirtualProtect(pChatAddress, sizeof(CALLINSTRUCTION), dwOldProtect, &dwOldProtect);
	}

	g_pRecvFunc = pRecvFunc;
}

VOID UnHookChatFunction(PVOID pChatAddress)
{
	DWORD dwOldProtect;

	if(pChatAddress && *((PBYTE)pChatAddress) == 0xE8)
	{
		VirtualProtect(pChatAddress, sizeof(CALLINSTRUCTION), PAGE_EXECUTE_READWRITE, &dwOldProtect);
		memcpy(pChatAddress, g_bOrigChatAddressBytes, sizeof(g_bOrigChatAddressBytes));
		*((PDWORD)((PBYTE)pChatAddress + sizeof(g_bOrigChatAddressBytes))) = g_dwOrigChatStackBytes;
		VirtualProtect(pChatAddress, sizeof(CALLINSTRUCTION), dwOldProtect, &dwOldProtect);
	}
}