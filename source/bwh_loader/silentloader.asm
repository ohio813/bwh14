.386
.model flat, stdcall
option casemap:none

include	   windows.inc
includelib kernel32.lib

CLOSEHANDLE	typedef	proto hObject:dword
CREATEFILE typedef proto lpFileName:dword, dwDesiredAccess:dword, dwShareMode:dword, lpSecurityAttributes:dword, dwCreationDisposition:dword, dwFlagsAndAttributes:dword, hTemplateFile:dword
DLLMAIN	typedef	proto hinstDLL:dword, fdwReason:dword, lpvReserved:dword
FREELIBRARY	typedef	proto hModule:dword
GETPROCADDRESS	typedef	proto hModule:dword, lpProcName:dword
GETMODULEHANDLE	typedef	proto lpModuleName:dword
LOADLIBRARY	typedef	proto lpFileName:dword
LSTRCMPI typedef proto lpString1:dword,	lpString2:dword
LSTRCPY	typedef	proto lpString1:dword, lpString2:dword
OPENEVENT typedef proto	dwDesiredAccess:dword, bInheritHandle:dword, lpName:dword
READFILE typedef proto hFile:dword,	lpBuffer:dword,	nNumberOfBytesToRead:dword,	lpNumberOfBytesRead:dword, lpOverlapped:dword
SETEVENT typedef proto hEvent:dword
SETFILEPOINTER typedef proto hFile:dword, lDistanceToMove:dword, lpDistanceToMoveHigh:dword, dwMoveMethod:dword
VIRTUALALLOC typedef proto lpAddress:dword,	dwSize:dword, flAllocationType:dword, flProtect:dword
VIRTUALFREE	typedef	proto lpAddress:dword, dwSize:dword, dwFreeType:dword

.data
szModuleFilename  db "szModuleFilename", 0
szKernel		  db "kernel32.dll", 0
szCloseHandle	  db "CloseHandle",	0
szCreateFile	  db "CreateFileA",	0
szFreeLibrary	  db "FreeLibrary",	0
szGetModuleHandle db "GetModuleHandleA", 0
szGetProcAddress  db "GetProcAddress", 0
szLoadLibrary	  db "LoadLibraryA", 0
szlstrcmpi		  db "lstrcmpiA", 0
szlstrcpy		  db "lstrcpyA", 0
szReadFile		  db "ReadFile", 0
szSetFilePointer  db "SetFilePointer", 0
szVirtualAlloc	  db "VirtualAlloc", 0
szVirtualFree	  db "VirtualFree",	0

.code

; ********** DLL Injector Code **********
DllInjectorStart:

DLLINJECTOR	struct
	szDll			   db 256 dup(?)
	szModuleFilename   db 17 dup(?)
	bOrigCode		   db 5	dup(?)
	pOrigAddy		   dd ?
	pCloseHandle	   dd ?
	pCreateFile		   dd ?
	pGetModuleHandle   dd ?
	pGetProcAddress	   dd ?
	pLoadLibrary	   dd ?
	plstrcmpi		   dd ?
	plstrcpy		   dd ?
	pReadFile		   dd ?
	pSetFilePointer	   dd ?
	pVirtualAlloc	   dd ?
	pVirtualFree	   dd ?
DLLINJECTOR	ends

DllInjector_GetStructPointer proto
DllInjector_RestoreCodePatch proto pLoaderStruct:dword
DllInjector_LoadDllIntoMemory proto	pLoaderStruct:dword
DllInjector_BindDllImage proto pLoaderStruct:dword,	hinstDLL:dword
DllInjector_RelocateImage proto	hinstDLL:dword
DllInjector_SetModuleFilename proto	pLoaderStruct:dword, hinstDLL:dword
DllInjector_CallEntryPoint proto hinstDLL:dword

DllInjector_Hooker:
    ; get pointer to loader structure and store in ecx
    invoke DllInjector_GetStructPointer
    mov ecx, eax

    ; get pointer to loader function and store in eax
    sub eax, DllInjectorEnd - DllInjector

    ; get pointer to window procedure and store in edx
    pop edx
    sub edx, 5

    ; backup window procedure return address and change it to the address of the loader function
    pop (DLLINJECTOR ptr[ecx]).pOrigAddy
    push eax

    ; restore window procedure code
    lea ecx, (DLLINJECTOR ptr[ecx]).bOrigCode
    mov eax, [ecx]
    mov [edx], eax
    mov al, [ecx + 4]
    mov [edx + 4], al

    ; jump back to window procedure
    jmp edx

DllInjector:
    push ebx ; pLoaderStruct
    push esi ; hinstDLL
    push eax
	; get a	pointer	to the structure that resides at the end of	the	code
	invoke DllInjector_GetStructPointer
	mov	ebx, eax

	; load the dll into	memory
	invoke DllInjector_LoadDllIntoMemory, ebx
	mov	esi, eax
	.if	esi !=	NULL

		; bind the dll to the os
		invoke DllInjector_BindDllImage, ebx,	esi
		.if	eax	!= NULL

			; relocate the image
			invoke DllInjector_RelocateImage, esi

			; set module filename
			invoke DllInjector_SetModuleFilename, ebx, esi

			; call entry point
			invoke DllInjector_CallEntryPoint, esi
		.endif
	.endif
	
	mov ecx, (DLLINJECTOR ptr[ebx]).pOrigAddy
	pop eax
        pop esi
	pop ebx
	jmp ecx

DllInjector_GetStructPointer proc
	call ni
ni:	pop	eax
	add	eax, DllInjectorEnd	- ni
	ret
DllInjector_GetStructPointer endp

DllInjector_LoadDllIntoMemory proc uses	ebx	esi	edi	pLoaderStruct:dword
	local hDll:dword
	local dwRead:dword
	local hinstDLL:dword
	local bLoadError:dword

	mov	bLoadError,	FALSE
	mov	hinstDLL, NULL
	mov	ebx, pLoaderStruct

	; open dll for reading
	invoke (CREATEFILE ptr[(DLLINJECTOR	ptr[ebx]).pCreateFile]), addr (DLLINJECTOR ptr[ebx]).szDll,	GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,	FILE_FLAG_SEQUENTIAL_SCAN, NULL
	mov	hDll, eax
	.if	hDll !=	INVALID_HANDLE_VALUE

		; allocate some	temporary memory for reading the dll file header
		invoke (VIRTUALALLOC ptr[(DLLINJECTOR ptr[ebx]).pVirtualAlloc]), NULL, 1000h, MEM_COMMIT, PAGE_EXECUTE_READWRITE
		mov	edi, eax
		.if	edi	!= NULL

			; read the dos header in from the dll
			invoke (READFILE ptr[(DLLINJECTOR ptr[ebx]).pReadFile]), hDll, edi,	sizeof(IMAGE_DOS_HEADER), addr dwRead, NULL
			.if	eax	!= NULL	&& dwRead == sizeof(IMAGE_DOS_HEADER)

				; set file pointer to beginning	of nt headers
				invoke (SETFILEPOINTER ptr[(DLLINJECTOR	ptr[ebx]).pSetFilePointer]), hDll, (IMAGE_DOS_HEADER ptr[edi]).e_lfanew, NULL, FILE_BEGIN
				.if	eax	!= INVALID_SET_FILE_POINTER

					; read the nt headers in from the dll
					invoke (READFILE ptr[(DLLINJECTOR ptr[ebx]).pReadFile]), hDll, edi,	sizeof(IMAGE_NT_HEADERS), addr dwRead, NULL
					.if	eax	!= NULL	&& dwRead == sizeof(IMAGE_NT_HEADERS)

						; calculate	size of	section	headers
						movzx eax, (IMAGE_NT_HEADERS ptr[edi]).FileHeader.NumberOfSections
						mov	ecx, sizeof(IMAGE_SECTION_HEADER)
						mul	ecx
						mov	ecx, eax
						lea	esi, [edi +	sizeof(IMAGE_NT_HEADERS)]

						; read the section headers in from the dll
						push ecx
						invoke (READFILE ptr[(DLLINJECTOR ptr[ebx]).pReadFile]), hDll, esi,	ecx, addr dwRead, NULL
						pop	ecx
						.if	eax	!= NULL	&& dwRead == ecx

							; now edi is a pointer to the nt headers and esi is	a pointer to the section headers
							; create memory	for	dll	image
							invoke (VIRTUALALLOC ptr[(DLLINJECTOR ptr[ebx]).pVirtualAlloc]), NULL, (IMAGE_NT_HEADERS ptr[edi]).OptionalHeader.SizeOfImage, MEM_COMMIT, PAGE_EXECUTE_READWRITE
							mov	hinstDLL, eax
							.if	hinstDLL !=	NULL

								; set file pointer to the beginning	of the file
								invoke (SETFILEPOINTER ptr[(DLLINJECTOR	ptr[ebx]).pSetFilePointer]), hDll, 0, NULL,	FILE_BEGIN
								.if	eax	!= INVALID_SET_FILE_POINTER

									; read in dll header section
									invoke (READFILE ptr[(DLLINJECTOR ptr[ebx]).pReadFile]), hDll, hinstDLL, (IMAGE_NT_HEADERS ptr[edi]).OptionalHeader.SizeOfHeaders, addr	dwRead,	NULL
									.if	eax	!= NULL

										; start	of section loader loop
										jmp	ent
									@@:	dec	(IMAGE_NT_HEADERS ptr[edi]).FileHeader.NumberOfSections

										; set file pointer to beginning	of section
										invoke (SETFILEPOINTER ptr[(DLLINJECTOR	ptr[ebx]).pSetFilePointer]), hDll, (IMAGE_SECTION_HEADER ptr[esi]).PointerToRawData, NULL, FILE_BEGIN
										.if	eax	!= INVALID_SET_FILE_POINTER

											; calculcate beginning of section in memory
											mov	ecx, hinstDLL
											add	ecx, (IMAGE_SECTION_HEADER ptr[esi]).VirtualAddress

											; read the section in from the dll
											invoke (READFILE ptr[(DLLINJECTOR ptr[ebx]).pReadFile]), hDll, ecx,	(IMAGE_SECTION_HEADER ptr[esi]).SizeOfRawData, addr	dwRead,	NULL
											.if	eax	== NULL
												mov	bLoadError,	TRUE
												mov	(IMAGE_NT_HEADERS ptr[edi]).FileHeader.NumberOfSections, 0
											.endif
										.else
											mov	bLoadError,	TRUE
											mov	(IMAGE_NT_HEADERS ptr[edi]).FileHeader.NumberOfSections, 0
										.endif
										add	esi, sizeof(IMAGE_SECTION_HEADER)
									ent:cmp	(IMAGE_NT_HEADERS ptr[edi]).FileHeader.NumberOfSections, 0
										jne	@B
									.else
										mov	bLoadError,	TRUE
									.endif
								.else
									mov	bLoadError,	TRUE
								.endif
							.endif
						.endif
					.endif
				.endif
			.endif
			invoke (VIRTUALFREE	ptr[(DLLINJECTOR ptr[ebx]).pVirtualFree]), edi,	0, MEM_RELEASE
		.endif
		invoke (CLOSEHANDLE	ptr[(DLLINJECTOR ptr[ebx]).pCloseHandle]), hDll
	.endif

	.if	bLoadError == TRUE && hinstDLL != NULL
		invoke (VIRTUALFREE	ptr[(DLLINJECTOR ptr[ebx]).pVirtualFree]), hinstDLL, 0,	MEM_RELEASE
		mov	hinstDLL, NULL
	.endif

	mov	eax, hinstDLL
	ret
DllInjector_LoadDllIntoMemory endp

DllInjector_BindDllImage proc uses ebx esi edi pLoaderStruct:dword,	hinstDLL:dword
	local pszDllName:dword
	local hDll:dword

	mov	ebx, pLoaderStruct

	; find import descriptor table
	mov	esi, hinstDLL
	add	esi, (IMAGE_DOS_HEADER ptr[esi]).e_lfanew
	mov	esi, (IMAGE_NT_HEADERS ptr[esi]).OptionalHeader.DataDirectory[sizeof(IMAGE_DATA_DIRECTORY) * IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress
	.if	esi	!= NULL
		add	esi, hinstDLL

		; start	of import descriptor table loop
		jmp	ent

		; get dll name
	@@:	mov	eax, (IMAGE_IMPORT_DESCRIPTOR ptr[esi]).Name1
		add	eax, hinstDLL
		mov	pszDllName,	eax

		; find base	address	of dll
		invoke (GETMODULEHANDLE	ptr[(DLLINJECTOR ptr[ebx]).pGetModuleHandle]), pszDllName
		mov	hDll, eax
		.if	hDll ==	NULL

			; dll isn't	loaded so let's	load it
			invoke (LOADLIBRARY	ptr[(DLLINJECTOR ptr[ebx]).pLoadLibrary]), pszDllName
			mov	hDll, eax		
		.endif

		.if	hDll !=	NULL
			push esi

			; calculate	pointers to	int	and	iat
			mov	edi, (IMAGE_IMPORT_DESCRIPTOR ptr[esi]).FirstThunk
			mov	esi, (IMAGE_IMPORT_DESCRIPTOR ptr[esi]).OriginalFirstThunk
			.if	esi	== NULL
				mov	esi, edi
			.endif
			add	edi, hinstDLL
			add	esi, hinstDLL

			; now edi is a pointer to the iat and esi is a pointer to the int
			; start	of int loop
			jmp	int_ent
	int_top:
			; get pointer to function name or get ordinal value
			.if	eax	& IMAGE_ORDINAL_FLAG
				movzx eax, ax
			.else
				add	eax, 2
				add	eax, hinstDLL
			.endif

			; lookup function address
			invoke (GETPROCADDRESS ptr[(DLLINJECTOR	ptr[ebx]).pGetProcAddress]), hDll, eax
			.if	eax	== NULL
				pop	esi
				mov	eax, FALSE
				ret
			.endif

			; write	function address to	iat
			stosd
	int_ent:
			; load int entry
			lodsd
			test eax, eax
			jnz	int_top

			pop	esi
		.else
			mov	eax, FALSE
			ret
		.endif

		add	esi, sizeof(IMAGE_IMPORT_DESCRIPTOR)
	ent:cmp	(IMAGE_IMPORT_DESCRIPTOR ptr[esi]).FirstThunk, NULL
		jnz	@B

	.endif

	mov	eax, TRUE
	ret
DllInjector_BindDllImage endp

DllInjector_RelocateImage proc uses	esi	hinstDLL:dword
	local dwDelta:dword
	local dwVA:dword

	; find base	relocations
	mov	ecx, hinstDLL
	add	ecx, (IMAGE_DOS_HEADER ptr[ecx]).e_lfanew
	mov	edx, (IMAGE_NT_HEADERS ptr[ecx]).OptionalHeader.DataDirectory[sizeof(IMAGE_DATA_DIRECTORY) * IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress
	.if	edx	!= NULL
		add	edx, hinstDLL

		; compute delta
		mov	eax, hinstDLL
		sub	eax, (IMAGE_NT_HEADERS ptr[ecx]).OptionalHeader.ImageBase
		mov	dwDelta, eax

		.if	dwDelta	!= 0

			; start	of relocation loop
			jmp	ent
		@@:	
			; point	esi	to start of	relocation data
			lea	esi, [edx +	sizeof(IMAGE_BASE_RELOCATION)]

			; point	edx	to next	relocation block
			add	edx, (IMAGE_BASE_RELOCATION	ptr[edx]).SizeOfBlock

			; loop through all relocation data
			.while esi != edx

				; load relocation data
				lodsw

				; make sure	it's valid
				xor	ecx, ecx
				shld cx, ax, 4
				.if	cl == IMAGE_REL_BASED_HIGHLOW

					; compute memory address of	relocation
					and	eax, 0FFFh
					add	eax, dwVA
					add	eax, hinstDLL

					; add the delta	to memory address
					mov	ecx, dwDelta
					add	[eax], ecx
				.endif
			.endw

			; get va of	current	block
		ent:mov	eax, (IMAGE_BASE_RELOCATION	ptr[edx]).VirtualAddress
			mov	dwVA, eax
			cmp	dwVA, 0
			jnz	@B
		.endif
	.endif
	ret
DllInjector_RelocateImage endp

DllInjector_SetModuleFilename proc uses	ebx	esi	edi	pLoaderStruct:dword, hinstDLL:dword
	local dwNumNames:dword
	local pExportDir:dword

	mov	ebx, pLoaderStruct

	; get export directory pointer
	mov	edi, hinstDLL
	add	edi, (IMAGE_DOS_HEADER ptr[edi]).e_lfanew
	mov	edi, (IMAGE_NT_HEADERS ptr[edi]).OptionalHeader.DataDirectory[sizeof(IMAGE_DATA_DIRECTORY) * IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress
	.if	edi
		add	edi, hinstDLL
		mov	pExportDir,	edi

		; get number of	names
		mov	eax, (IMAGE_EXPORT_DIRECTORY ptr[edi]).NumberOfNames
		mov	dwNumNames,	eax

		; get pointer to name array
		mov	esi, (IMAGE_EXPORT_DIRECTORY ptr[edi]).AddressOfNames
		add	esi, hinstDLL

		; get pointer to ordinal array
		mov	edi, (IMAGE_EXPORT_DIRECTORY ptr[edi]).AddressOfNameOrdinals
		add	edi, hinstDLL

		; start	of name	loop
		.while dwNumNames

			; get pointer to current name and see if it's "szModuleFilename"
			lodsd
			add	eax, hinstDLL
			invoke (LSTRCMPI ptr[(DLLINJECTOR ptr[ebx]).plstrcmpi]), addr (DLLINJECTOR ptr[ebx]).szModuleFilename, eax
			.if	eax	== 0

				; found	a match; get it's index	in the address array
				movzx eax, word	ptr[edi]

				; calculate	pointer	to appropiate index	in address array
				shl	eax, 2
				mov	edi, pExportDir
				add	eax, (IMAGE_EXPORT_DIRECTORY ptr[edi]).AddressOfFunctions
				add	eax, hinstDLL

				; calculate	address	of export
				mov	ecx, [eax]
				add	ecx, hinstDLL

				; copy module filename
				invoke (LSTRCPY	ptr[(DLLINJECTOR ptr[ebx]).plstrcpy]), ecx,	addr (DLLINJECTOR ptr[ebx]).szDll
				.break
			.endif
			add	edi, 2
			dec	dwNumNames
		.endw
	.endif	 
	ret
DllInjector_SetModuleFilename endp

DllInjector_CallEntryPoint proc	uses ebx hinstDLL:dword

	; find entry point
	mov	ebx, hinstDLL
	add	ebx, (IMAGE_DOS_HEADER ptr[ebx]).e_lfanew
	mov	ebx, (IMAGE_NT_HEADERS ptr[ebx]).OptionalHeader.AddressOfEntryPoint
	add	ebx, hinstDLL

	; call entry point
	invoke (DLLMAIN	ptr	ebx), hinstDLL,	DLL_PROCESS_ATTACH,	NULL

	; on failure call entry	point again
	.if	eax	== FALSE
		invoke (DLLMAIN	ptr	ebx), hinstDLL,	DLL_PROCESS_DETACH,	NULL
	.endif
	ret
DllInjector_CallEntryPoint endp

DllInjectorEnd:

ExportDllInjector proc uses	ebx	esi	pbMem:dword, pszDll:dword, pbOrigBytes:dword
	.if	pbMem
		mov	ebx, pbMem
		invoke RtlMoveMemory, ebx, DllInjectorStart, DllInjectorEnd	- DllInjectorStart
		add	ebx, DllInjectorEnd	- DllInjectorStart
		invoke lstrcpy,	addr (DLLINJECTOR ptr[ebx]).szDll, pszDll
		invoke lstrcpy,	addr (DLLINJECTOR ptr[ebx]).szModuleFilename, addr szModuleFilename
		invoke RtlMoveMemory, addr (DLLINJECTOR	ptr[ebx]).bOrigCode, pbOrigBytes, 5
		invoke GetModuleHandle,	addr szKernel
		mov	esi, eax
		invoke GetProcAddress, esi,	addr szCloseHandle
		mov	(DLLINJECTOR ptr[ebx]).pCloseHandle, eax
		invoke GetProcAddress, esi,	addr szCreateFile
		mov	(DLLINJECTOR ptr[ebx]).pCreateFile,	eax
		invoke GetProcAddress, esi,	addr szGetModuleHandle
		mov	(DLLINJECTOR ptr[ebx]).pGetModuleHandle, eax
		invoke GetProcAddress, esi,	addr szGetProcAddress
		mov	(DLLINJECTOR ptr[ebx]).pGetProcAddress,	eax
		invoke GetProcAddress, esi,	addr szLoadLibrary
		mov	(DLLINJECTOR ptr[ebx]).pLoadLibrary, eax
		invoke GetProcAddress, esi,	addr szlstrcmpi
		mov	(DLLINJECTOR ptr[ebx]).plstrcmpi, eax
		invoke GetProcAddress, esi,	addr szlstrcpy
		mov	(DLLINJECTOR ptr[ebx]).plstrcpy, eax
		invoke GetProcAddress, esi,	addr szReadFile
		mov	(DLLINJECTOR ptr[ebx]).pReadFile, eax
		invoke GetProcAddress, esi,	addr szSetFilePointer
		mov	(DLLINJECTOR ptr[ebx]).pSetFilePointer,	eax
		invoke GetProcAddress, esi,	addr szVirtualAlloc
		mov	(DLLINJECTOR ptr[ebx]).pVirtualAlloc, eax
		invoke GetProcAddress, esi,	addr szVirtualFree
		mov	(DLLINJECTOR ptr[ebx]).pVirtualFree, eax
	.endif
	mov	eax, sizeof(DLLINJECTOR) + (DllInjectorEnd - DllInjectorStart)
	ret
ExportDllInjector endp

; ********** DLL Uninjector	Code **********
DllUninjectorStart:

DLLUNINJECTOR struct
	hinstDLL		   dd ?
	bOrigCode		   db 5	dup(?)
	pOrigAddy		   dd ?
	pVirtualFree	   dd ?
DLLUNINJECTOR ends

DllUninjector_GetStructPointer proto
DllUninjector_RestoreCodePatch proto pLoaderStruct:dword
DllUninjector_CallEntryPoint proto hinstDLL:dword
DllUninjector_FreeDllMemory	proto pLoaderStruct:dword

DllUninjector_Hooker:
    ; get pointer to loader structure and store in ecx
    invoke DllUninjector_GetStructPointer
    mov ecx, eax

    ; get pointer to loader function and store in eax
    sub eax, DllUninjectorEnd - DllUninjector

    ; get pointer to window procedure and store in edx
    pop edx
    sub edx, 5

    ; backup window procedure return address and change it to the address of the loader function
    pop (DLLUNINJECTOR ptr[ecx]).pOrigAddy
    push eax

    ; restore window procedure code
    lea ecx, (DLLUNINJECTOR ptr[ecx]).bOrigCode
    mov eax, [ecx]
    mov [edx], eax
    mov al, [ecx + 4]
    mov [edx + 4], al

    ; jump back to window procedure
    jmp edx

DllUninjector:
	push ebx ; pLoaderStruct
	push esi ; hinstDLL
        push eax

	; get a	pointer	to the structure that resides at the end of	the	code
	invoke DllUninjector_GetStructPointer
	mov	ebx, eax
	mov	esi, (DLLUNINJECTOR	ptr[ebx]).hinstDLL

	; call entry point
	invoke DllUninjector_CallEntryPoint, esi

	; free memory
	invoke DllUninjector_FreeDllMemory,	ebx
	
	mov ecx, (DLLUNINJECTOR ptr[ebx]).pOrigAddy
	pop eax
	pop esi
	pop ebx
	jmp ecx

DllUninjector_GetStructPointer proc
	call ni
ni:	pop	eax
	add	eax, DllUninjectorEnd -	ni
	ret
DllUninjector_GetStructPointer endp

DllUninjector_CallEntryPoint proc uses ebx hinstDLL:dword

	; find entry point
	mov	ebx, hinstDLL
	add	ebx, (IMAGE_DOS_HEADER ptr[ebx]).e_lfanew
	mov	ebx, (IMAGE_NT_HEADERS ptr[ebx]).OptionalHeader.AddressOfEntryPoint
	add	ebx, hinstDLL

	; call entry point
	invoke (DLLMAIN	ptr	ebx), hinstDLL,	DLL_PROCESS_DETACH,	NULL
	ret
DllUninjector_CallEntryPoint endp

DllUninjector_FreeDllMemory	proc uses ebx pLoaderStruct:dword
	mov	ebx, pLoaderStruct

	mov	eax, (DLLUNINJECTOR	ptr[ebx]).hinstDLL
	invoke (VIRTUALFREE	ptr[(DLLUNINJECTOR ptr[ebx]).pVirtualFree]), eax, 0, MEM_RELEASE
	ret
DllUninjector_FreeDllMemory	endp

DllUninjectorEnd:

ExportDllUninjector	proc uses ebx esi pbMem:dword, hinstDLL:dword, pbOrigBytes:dword
	.if	pbMem
		mov	ebx, pbMem
		invoke RtlMoveMemory, ebx, DllUninjectorStart, DllUninjectorEnd	- DllUninjectorStart
		add	ebx, DllUninjectorEnd -	DllUninjectorStart
		mov	eax, hinstDLL
		mov	(DLLUNINJECTOR ptr[ebx]).hinstDLL, eax
		invoke RtlMoveMemory, addr (DLLUNINJECTOR ptr[ebx]).bOrigCode, pbOrigBytes,	5
		invoke GetModuleHandle,	addr szKernel
		mov	esi, eax
		invoke GetProcAddress, esi,	addr szVirtualFree
		mov	(DLLUNINJECTOR ptr[ebx]).pVirtualFree, eax
	.endif
	mov	eax, sizeof(DLLUNINJECTOR) + (DllUninjectorEnd - DllUninjectorStart)
	ret
ExportDllUninjector	endp

; ********** DLL Loader	Code **********
DllLoaderStart:

DLLLOADER struct
	szDll			   db 256 dup(?)
	bOrigCode		   db 5	dup(?)
	pOrigAddy		   dd ?
	pLoadLibrary	   dd ?
DLLLOADER ends

DllLoader_GetStructPointer proto
DllLoader_RestoreCodePatch proto pLoaderStruct:dword
DllLoader_LoadDll proto	pLoaderStruct:dword

DllLoader_Hooker:
    ; get pointer to loader structure and store in ecx
    invoke DllLoader_GetStructPointer
    mov ecx, eax

    ; get pointer to loader function and store in eax
    sub eax, DllLoaderEnd - DllLoader

    ; get pointer to window procedure and store in edx
    pop edx
    sub edx, 5

    ; backup window procedure return address and change it to the address of the loader function
    pop (DLLLOADER ptr[ecx]).pOrigAddy
    push eax

    ; restore window procedure code
    lea ecx, (DLLLOADER ptr[ecx]).bOrigCode
    mov eax, [ecx]
    mov [edx], eax
    mov al, [ecx + 4]
    mov [edx + 4], al

    ; jump back to window procedure
    jmp edx

DllLoader:
    push ebx ; pLoaderStruct
    push eax
	; get a	pointer	to the structure that resides at the end of	the	code
	invoke DllLoader_GetStructPointer
    mov ebx, eax
    
	; load the dll
	invoke DllLoader_LoadDll, ebx
	
	mov ecx, (DLLLOADER ptr[ebx]).pOrigAddy
	pop eax
	pop ebx
	jmp ecx

DllLoader_GetStructPointer proc
	call ni
ni:	pop	eax
	add	eax, DllLoaderEnd -	ni
	ret
DllLoader_GetStructPointer endp

DllLoader_LoadDll proc uses	ebx	pLoaderStruct:dword
	mov	ebx, pLoaderStruct

	invoke (LOADLIBRARY	ptr[(DLLLOADER ptr[ebx]).pLoadLibrary]), addr (DLLLOADER ptr[ebx]).szDll
	ret
DllLoader_LoadDll endp

DllLoaderEnd:

ExportDllLoader	proc uses ebx esi pbMem:dword, pszDll:dword, pbOrigBytes:dword
	.if	pbMem
		mov	ebx, pbMem
		invoke RtlMoveMemory, ebx, DllLoaderStart, DllLoaderEnd	- DllLoaderStart
		add	ebx, DllLoaderEnd -	DllLoaderStart
		invoke lstrcpy,	addr (DLLLOADER	ptr[ebx]).szDll, pszDll
		invoke RtlMoveMemory, addr (DLLLOADER ptr[ebx]).bOrigCode, pbOrigBytes,	5
		invoke GetModuleHandle,	addr szKernel
		mov	esi, eax
		invoke GetProcAddress, esi,	addr szLoadLibrary
		mov	(DLLLOADER ptr[ebx]).pLoadLibrary, eax
	.endif
	mov	eax, sizeof(DLLLOADER) + (DllLoaderEnd - DllLoaderStart)
	ret
ExportDllLoader	endp

; ********** DLL Unloader Code **********
DllUnloaderStart:

DLLUNLOADER	struct
	hinstDLL		   dd ?
	bOrigCode		   db 5	dup(?)
	pOrigAddy		   dd ?
	pFreeLibrary	   dd ?
DLLUNLOADER	ends

DllUnloader_GetStructPointer proto
DllUnloader_RestoreCodePatch proto pLoaderStruct:dword
DllUnloader_UnloadDll proto	pLoaderStruct:dword

DllUnloader_Hooker:
    ; get pointer to loader structure and store in ecx
    invoke DllUnloader_GetStructPointer
    mov ecx, eax

    ; get pointer to loader function and store in eax
    sub eax, DllUnloaderEnd - DllUnloader

    ; get pointer to window procedure and store in edx
    pop edx
    sub edx, 5

    ; backup window procedure return address and change it to the address of the loader function
    pop (DLLUNLOADER ptr[ecx]).pOrigAddy
    push eax

    ; restore window procedure code
    lea ecx, (DLLUNLOADER ptr[ecx]).bOrigCode
    mov eax, [ecx]
    mov [edx], eax
    mov al, [ecx + 4]
    mov [edx + 4], al

    ; jump back to window procedure
    jmp edx

DllUnloader:
	push ebx ; pLoaderStruct
	push eax
	; get a	pointer	to the structure that resides at the end of	the	code
	invoke DllUnloader_GetStructPointer
	mov	ebx, eax

	; unload the dll
	invoke DllUnloader_UnloadDll, ebx
    
    mov ecx, (DLLUNLOADER ptr[ebx]).pOrigAddy
    pop eax
    pop ebx
    jmp ecx
    
DllUnloader_GetStructPointer proc
	call ni
ni:	pop	eax
	add	eax, DllUnloaderEnd	- ni
	ret
DllUnloader_GetStructPointer endp

DllUnloader_UnloadDll proc uses	ebx	pLoaderStruct:dword
	mov	ebx, pLoaderStruct

	invoke (FREELIBRARY	ptr[(DLLUNLOADER ptr[ebx]).pFreeLibrary]), (DLLUNLOADER ptr[ebx]).hinstDLL
	ret
DllUnloader_UnloadDll endp

DllUnloaderEnd:

ExportDllUnloader proc uses	ebx	esi	pbMem:dword, hinstDLL:dword, pbOrigBytes:dword
	.if	pbMem
		mov	ebx, pbMem
		invoke RtlMoveMemory, ebx, DllUnloaderStart, DllUnloaderEnd	- DllUnloaderStart
		add	ebx, DllUnloaderEnd	- DllUnloaderStart
		mov	eax, hinstDLL
		mov	(DLLUNLOADER ptr[ebx]).hinstDLL, eax
		invoke RtlMoveMemory, addr (DLLUNLOADER	ptr[ebx]).bOrigCode, pbOrigBytes, 5
		invoke GetModuleHandle,	addr szKernel
		mov	esi, eax
		invoke GetProcAddress, esi,	addr szFreeLibrary
		mov	(DLLUNLOADER ptr[ebx]).pFreeLibrary, eax
	.endif
	mov	eax, sizeof(DLLUNLOADER) + (DllUnloaderEnd - DllUnloaderStart)
	ret
ExportDllUnloader endp

end