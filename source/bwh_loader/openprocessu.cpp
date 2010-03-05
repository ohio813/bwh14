#include "openprocessu.h"
#include <aclapi.h>

typedef DWORD(WINAPI *PFNSETSECURITYINFO)(HANDLE handle, SE_OBJECT_TYPE ObjectType, SECURITY_INFORMATION SecurityInfo, PSID psidOwner, PSID psidGroup, PACL pDacl, PACL pSacl);
typedef DWORD(WINAPI *PFNSETENTRIESINACL)(ULONG cCountOfExplicitEntries, PEXPLICIT_ACCESS pListOfExplicitEntries, PACL OldAcl, PACL* NewAcl);
typedef BOOL(WINAPI *PFNOPENPROCESSTOKEN)(HANDLE ProcessHandle, DWORD DesiredAccess, PHANDLE TokenHandle);
typedef BOOL(WINAPI *PFNLOOKUPPRIVILEGEVALUE)(LPCTSTR lpSystemName, LPCTSTR lpName, PLUID lpLuid);
typedef BOOL(WINAPI *PFNADJUSTTOKENPRIVILEGES)(HANDLE TokenHandle, BOOL DisableAllPrivileges, PTOKEN_PRIVILEGES NewState, DWORD BufferLength, PTOKEN_PRIVILEGES PreviousState, PDWORD ReturnLength);


VOID GiveDebugRights()
{
	HANDLE hToken;
	LUID luid;
	TOKEN_PRIVILEGES tp;
	HMODULE hAdvApi;
	PFNOPENPROCESSTOKEN pOpenProcessToken;
	PFNLOOKUPPRIVILEGEVALUE pLookupPrivilegeValue;
	PFNADJUSTTOKENPRIVILEGES pAdjustTokenPrivileges;

	hAdvApi = LoadLibrary("Advapi32.dll");
	if(hAdvApi)
	{
		pOpenProcessToken = (PFNOPENPROCESSTOKEN)GetProcAddress(hAdvApi, "OpenProcessToken");
		pLookupPrivilegeValue = (PFNLOOKUPPRIVILEGEVALUE)GetProcAddress(hAdvApi, "LookupPrivilegeValueA");
		pAdjustTokenPrivileges = (PFNADJUSTTOKENPRIVILEGES)GetProcAddress(hAdvApi, "AdjustTokenPrivileges");
		if(pOpenProcessToken && pLookupPrivilegeValue && pAdjustTokenPrivileges)
		{
			pOpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
			if(hToken)
			{
				pLookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);
				tp.PrivilegeCount = 1;
				tp.Privileges[0].Luid = luid;
				tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
				AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
				CloseHandle(hToken);
			}
		}
		FreeLibrary(hAdvApi);
	}
}

HANDLE OpenProcessU(DWORD dwDesiredAccess, DWORD dwPId)
{
	HANDLE hProcess;
	HMODULE hAdvapi32;
	PFNSETSECURITYINFO pSetSecurityInfo;
	PFNSETENTRIESINACL pSetEntriesInAcl;
	HANDLE hWriteDac;
	PACL pDacl;
	SID sid = {SID_REVISION, 1, SECURITY_WORLD_SID_AUTHORITY, 0};
	EXPLICIT_ACCESS ea = {dwDesiredAccess, SET_ACCESS, NO_INHERITANCE, {0, NO_MULTIPLE_TRUSTEE, TRUSTEE_IS_SID, TRUSTEE_IS_USER, (LPSTR)&sid}};
	
	// try and open the process
	hProcess = OpenProcess(dwDesiredAccess, FALSE, dwPId);
	if(hProcess == NULL)
	{
		// try and grant debug privledges and try again
		GiveDebugRights();
		hProcess = OpenProcess(dwDesiredAccess, FALSE, dwPId);
		if(hProcess == NULL)
		{
			// last ditch option: modify wow's privleges
			hAdvapi32 = LoadLibrary("advapi32.dll");
			if(hAdvapi32)
			{
				pSetSecurityInfo = (PFNSETSECURITYINFO)GetProcAddress(hAdvapi32, "SetSecurityInfo");
				pSetEntriesInAcl = (PFNSETENTRIESINACL)GetProcAddress(hAdvapi32, "SetEntriesInAclA");
				if(pSetSecurityInfo && pSetEntriesInAcl)
				{
					hWriteDac = OpenProcess(WRITE_DAC | READ_CONTROL, FALSE, dwPId);
					if(hWriteDac)
					{
						if(pSetEntriesInAcl(1, &ea, 0, &pDacl) == ERROR_SUCCESS)
						{
							pSetSecurityInfo(hWriteDac, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, 0, 0, pDacl, 0);
							DuplicateHandle(GetCurrentProcess(), hWriteDac, GetCurrentProcess(), &hProcess, dwDesiredAccess, FALSE, NULL);
							LocalFree(pDacl);
						}
						CloseHandle(hWriteDac);
					}
				}
				FreeLibrary(hAdvapi32);
			}
		}
	}
	return hProcess;
}