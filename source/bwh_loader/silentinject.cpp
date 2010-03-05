#include "silentinject.h"
#include <tlhelp32.h>
#include "filemap.h"
#include "openprocessu.h"

extern "C" DWORD __stdcall ExportDllInjector(PVOID pbMem, PSZ pszDll, PBYTE pbOrigBytes);
extern "C" DWORD __stdcall ExportDllLoader(PVOID pbMem, PSZ pszDll, PBYTE pbOrigBytes);
extern "C" DWORD __stdcall ExportDllUninjector(PVOID pbMem, DWORD hinstDLL, PBYTE pbOrigBytes);
extern "C" DWORD __stdcall ExportDllUnloader(PVOID pbMem, DWORD hinstDLL, PBYTE pbOrigBytes);

DWORD FindBaseAddress(HANDLE hProcess, DWORD dwPId, PSZ pszDll)
{
	char szDirectory[MAX_PATH];
	DWORD dwLength, dwPresentRva, dwBaseAddress = NULL, dwRead;
	HANDLE hSnap;
	MODULEENTRY32 me;
	BYTE bBuffer[5];
	PVOID pvReadAddress;

	lstrcpy(szDirectory, pszDll);
	for(dwLength = (DWORD)lstrlen(szDirectory); szDirectory[dwLength] != '\\'; dwLength--)
		szDirectory[dwLength] = '\0';
	
	lstrcat(szDirectory, "bwh.ini");
	dwPresentRva = GetPrivateProfileInt("D3D", "Present", 0, szDirectory);
	if(dwPresentRva)
	{
		hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPId);
		Module32First(hSnap, &me);
		do
		{
			if(lstrcmpi(me.szModule, "d3d9.dll") == 0)
			{
				pvReadAddress = UlongToPtr(PtrToUlong(me.hModule) + dwPresentRva);
				ReadProcessMemory(hProcess, pvReadAddress, bBuffer, 5, &dwRead);
				if(bBuffer[0] == 0xE8)
				{
					pvReadAddress = UlongToPtr((PtrToUlong(pvReadAddress) + *(PDWORD)&bBuffer[1]) & 0xFFFF0000);
					while(1)
					{
						ReadProcessMemory(hProcess, pvReadAddress, bBuffer, 2, &dwRead);
						if(*(PWORD)bBuffer == IMAGE_DOS_SIGNATURE)
						{
							return PtrToUlong(pvReadAddress);
						}
						pvReadAddress = UlongToPtr(PtrToUlong(pvReadAddress) - 0x10000);
					}
				}
				break;
			}
		}while(Module32Next(hSnap, &me));
		CloseHandle(hSnap);
	}
	return dwBaseAddress;
}

VOID SilentInjectDll(HWND hwndWoW, DWORD dwLoadRVA, DWORD dwUnloadRVA, PSZ pszDll, BOOL bInject)
{
	DWORD dwPId, dwRead, dwBaseAddress, dwLoaderSize, dwHeaderOldProtect, dwJumpOldProtect, dwInjectAddress;
	HANDLE hProcess;
	BYTE bOrigBytes[5], bCallBytes[5];
	PVOID pCodeBackup, pLoaderCode;

	// does dll exist?
	if(GetFileAttributes(pszDll) != INVALID_FILE_ATTRIBUTES)
	{
		// get a handle to wow's process
		GetWindowThreadProcessId(hwndWoW, &dwPId);
		hProcess = OpenProcessU(PROCESS_ALL_ACCESS, dwPId);
		if(hProcess)
		{
			// backup wow memory
			VirtualProtectEx(hProcess, (PVOID)0x400000, 0x1000, PAGE_EXECUTE_READWRITE, &dwHeaderOldProtect);
			pCodeBackup = VirtualAlloc(NULL, 0x1000, MEM_COMMIT, PAGE_READWRITE);
			ReadProcessMemory(hProcess, (PVOID)0x400000, pCodeBackup, 0x1000, &dwRead);

			// backup jump bytes
			dwBaseAddress = FindBaseAddress(hProcess, dwPId, pszDll);
			if(dwBaseAddress) dwInjectAddress = dwBaseAddress + dwUnloadRVA;
			else dwInjectAddress = 0x400000 + dwLoadRVA;
			VirtualProtectEx(hProcess, UlongToPtr(dwInjectAddress), 5, PAGE_EXECUTE_READWRITE, &dwJumpOldProtect);
			ReadProcessMemory(hProcess, UlongToPtr(dwInjectAddress), bOrigBytes, sizeof(bOrigBytes), &dwRead);

			// get loader code
			pLoaderCode = VirtualAlloc(NULL, 0x1000, MEM_COMMIT, PAGE_READWRITE);
			dwBaseAddress = FindBaseAddress(hProcess, dwPId, pszDll);
			if(dwBaseAddress)
			{
				if(bInject)
					dwLoaderSize = ExportDllUninjector(pLoaderCode, dwBaseAddress, bOrigBytes);
				else
					dwLoaderSize = ExportDllUnloader(pLoaderCode, dwBaseAddress, bOrigBytes);
			}
			else
			{
				if(bInject)
					dwLoaderSize = ExportDllInjector(pLoaderCode, pszDll, bOrigBytes);
				else
					dwLoaderSize = ExportDllLoader(pLoaderCode, pszDll, bOrigBytes);
			}

			// write the loader
			WriteProcessMemory(hProcess, (PVOID)0x400000, pLoaderCode, dwLoaderSize, &dwRead);
			*bCallBytes = 0xE8;
			*((PDWORD)&bCallBytes[1]) = 0x400000 - (dwInjectAddress + sizeof(bCallBytes));
			WriteProcessMemory(hProcess, UlongToPtr(dwInjectAddress), bCallBytes, sizeof(bCallBytes), &dwRead);
			
			// ensure the loader runs
			SendMessage(hwndWoW, WM_NULL, 0, 0);

			// cleanup
			WriteProcessMemory(hProcess, (PVOID)0x400000, pCodeBackup, 0x1000, &dwRead);
			VirtualFree(pLoaderCode, 0, MEM_RELEASE);
			VirtualFree(pCodeBackup, 0, MEM_RELEASE);
			VirtualProtectEx(hProcess, UlongToPtr(dwInjectAddress), 5, dwJumpOldProtect, &dwJumpOldProtect);
			VirtualProtectEx(hProcess, (PVOID)0x400000, 0x1000, dwHeaderOldProtect, &dwHeaderOldProtect);
			CloseHandle(hProcess);
		}
	}
}