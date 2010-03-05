GetModuleHandleA PROTO :DWORD
GetModuleHandle equ <GetModuleHandleA>
GetProcAddress PROTO :DWORD,:DWORD
lstrcpyA PROTO :DWORD,:DWORD
lstrcpy equ <lstrcpyA>
RtlMoveMemory PROTO :DWORD,:DWORD,:DWORD

DLL_PROCESS_ATTACH               equ 1h
DLL_PROCESS_DETACH               equ 0h
EVENT_MODIFY_STATE               equ 2h
FALSE                            equ 0h
FILE_BEGIN                       equ 0h
FILE_FLAG_SEQUENTIAL_SCAN        equ 08000000h
FILE_SHARE_READ                  equ 1h
GENERIC_READ                     equ 80000000h
IMAGE_DIRECTORY_ENTRY_IMPORT     equ 1h
IMAGE_DIRECTORY_ENTRY_BASERELOC  equ 5h
IMAGE_DIRECTORY_ENTRY_EXPORT     equ 0h
IMAGE_NUMBEROF_DIRECTORY_ENTRIES equ 16
IMAGE_ORDINAL_FLAG               equ 80000000h
IMAGE_REL_BASED_HIGHLOW          equ 3h
IMAGE_SIZEOF_SHORT_NAME          equ 8h
INVALID_HANDLE_VALUE             equ 0FFFFFFFFh
INVALID_SET_FILE_POINTER         equ 0FFFFFFFFh
MEM_COMMIT                       equ 1000h
MEM_RELEASE                      equ 8000h
NULL                             equ 0h
OPEN_EXISTING                    equ 3h
PAGE_EXECUTE_READWRITE           equ 40h 
TRUE                             equ 1h

IMAGE_BASE_RELOCATION struct
    VirtualAddress dd ?
    SizeOfBlock    dd ?
IMAGE_BASE_RELOCATION ends

IMAGE_DATA_DIRECTORY struct
    VirtualAddress dd ?
    isize          dd ?
IMAGE_DATA_DIRECTORY ends

IMAGE_DOS_HEADER struct
    e_magic    dw ?
    e_cblp     dw ?
    e_cp       dw ?
    e_crlc     dw ?
    e_cparhdr  dw ?
    e_minalloc dw ?
    e_maxalloc dw ?
    e_ss       dw ?
    e_sp       dw ?
    e_csum     dw ?
    e_ip       dw ?
    e_cs       dw ?
    e_lfarlc   dw ?
    e_ovno     dw ?
    e_res      dw 4 dup(?)
    e_oemid    dw ?
    e_oeminfo  dw ?
    e_res2     dw 10 dup(?)
    e_lfanew   dd ?
IMAGE_DOS_HEADER ends

IMAGE_EXPORT_DIRECTORY STRUCT
  Characteristics           DWORD      ?
  TimeDateStamp             DWORD      ?
  MajorVersion              WORD       ?
  MinorVersion              WORD       ?
  nName                     DWORD      ?
  nBase                     DWORD      ?
  NumberOfFunctions         DWORD      ?
  NumberOfNames             DWORD      ?
  AddressOfFunctions        DWORD      ?
  AddressOfNames            DWORD      ?
  AddressOfNameOrdinals     DWORD      ?
IMAGE_EXPORT_DIRECTORY ENDS

IMAGE_FILE_HEADER struct
    Machine              dw ?
    NumberOfSections     dw ?
    TimeDateStamp        dd ?
    PointerToSymbolTable dd ?
    NumberOfSymbols      dd ?
    SizeOfOptionalHeader dw ?
    Characteristics      dw ?
IMAGE_FILE_HEADER ends

IMAGE_IMPORT_BY_NAME struct
    Hint  dw ?
    Name1 db ?
IMAGE_IMPORT_BY_NAME ends

IMAGE_IMPORT_DESCRIPTOR struct
    union
        Characteristics    dd ?
        OriginalFirstThunk dd ?
	ends
    TimeDateStamp		   dd ?
    ForwarderChain         dd ?
    Name1                  dd ?
    FirstThunk             dd ?
IMAGE_IMPORT_DESCRIPTOR ends

IMAGE_OPTIONAL_HEADER struct
    Magic                       dw ?
    MajorLinkerVersion          db ?
    MinorLinkerVersion          db ?
    SizeOfCode                  dd ?
    SizeOfInitializedData       dd ?
    SizeOfUninitializedData     dd ?
    AddressOfEntryPoint         dd ?
    BaseOfCode                  dd ?
    BaseOfData                  dd ?
    ImageBase                   dd ?
    SectionAlignment            dd ?
    FileAlignment               dd ?
    MajorOperatingSystemVersion dw ?
    MinorOperatingSystemVersion dw ?
    MajorImageVersion           dw ?
    MinorImageVersion           dw ?
    MajorSubsystemVersion       dw ?
    MinorSubsystemVersion       dw ?
    Win32VersionValue           dd ?
    SizeOfImage                 dd ?
    SizeOfHeaders               dd ?
    CheckSum                    dd ?
    Subsystem                   dw ?
    DllCharacteristics          dw ?
    SizeOfStackReserve          dd ?
    SizeOfStackCommit           dd ?
    SizeOfHeapReserve           dd ?
    SizeOfHeapCommit            dd ?
    LoaderFlags                 dd ?
    NumberOfRvaAndSizes         dd ?
    DataDirectory               IMAGE_DATA_DIRECTORY IMAGE_NUMBEROF_DIRECTORY_ENTRIES dup(<>)
IMAGE_OPTIONAL_HEADER ends

IMAGE_NT_HEADERS struct
  Signature      dd ?
  FileHeader     IMAGE_FILE_HEADER <>
  OptionalHeader IMAGE_OPTIONAL_HEADER <>
IMAGE_NT_HEADERS ends

IMAGE_SECTION_HEADER struct
    Name1                db IMAGE_SIZEOF_SHORT_NAME dup(?)
    union Misc
        PhysicalAddress  dd ?
        VirtualSize      dd ?
    ends
    VirtualAddress       dd ?
    SizeOfRawData        dd ?
    PointerToRawData     dd ?
    PointerToRelocations dd ?
    PointerToLinenumbers dd ?
    NumberOfRelocations  dw ?
    NumberOfLinenumbers  dw ?
    Characteristics      dd ?
IMAGE_SECTION_HEADER ends

