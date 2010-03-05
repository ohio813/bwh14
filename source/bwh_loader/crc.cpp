#include "crc.h"

// Private includes
#include <commctrl.h>

// Private constants
#define CRC_POLYNOMIAL 0x0EDB88320
#define CRC_BUFFERSIZE 0x18000

// Private globals
BOOL  bTable = FALSE;
DWORD dwCRCTable[256];

// Private prototypes
void        CRC32Init();
inline void CRC32Compute(PDWORD pdwCRC, PVOID pvData, DWORD dwSize);

void CRC32Init()
{
    for(DWORD x = 255; x > 0; x--)
    {
        DWORD z = x;
        for(DWORD y = 8; y > 0; y--)
        {
            if(z & 1)
                z = (z >> 1) ^ CRC_POLYNOMIAL;
            else
                z >>= 1;
        }
        dwCRCTable[x] =  z;
    }
    bTable = TRUE;
    return;
}
inline void CRC32Compute(PDWORD pdwCRC, PVOID pvData, DWORD dwSize)
{
    __asm
    {
        xor  eax,   eax                    // eax = trash register
        mov  ebx,   [pdwCRC]               // ebx = crc value memory address
        push ebx                           // backup crc value memory address
        mov  ebx,   [ebx]                  // get crc value from memory
        mov  ecx,   [dwSize]               // ecx = byte counter
        mov  edx,   [pvData]               // edx = data pointer
    j1: mov  al,    [edx]                  // grab next data byte
        xor  al,    bl                     // xor data byte with crc byte
        shr  ebx,   8                      // move out 1 byte of crc
        xor  ebx,   [dwCRCTable + eax * 4] // xor crc with value in table
        inc  edx                           // increase data pointer
        dec  ecx                           // decrease byte counter
        jnz  j1                            // if more bytes do it again
        pop  eax                           // get crc value memory address
        mov  [eax], ebx                    // put crc value in memory
    }
}
DWORD GetFileCRC(PSZ pszFilename, PBOOL pbStop, HWND hwndProgress)
{
    DWORD dwRead;
    DWORD dwCRC = -1;
    DWORD dwCurrent = 0;
    HANDLE hFile = CreateFile(pszFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NOT_CONTENT_INDEXED | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if(hFile != INVALID_HANDLE_VALUE)
    {
        if(!bTable) CRC32Init();
        PVOID pMem = HeapAlloc(GetProcessHeap(), NULL, CRC_BUFFERSIZE);
        DWORD dwTotal = GetFileSize(hFile, NULL);
        while(!*pbStop)
        {
            ReadFile(hFile, pMem, CRC_BUFFERSIZE, &dwRead, NULL);
            if(dwRead) 
                CRC32Compute(&dwCRC, pMem, dwRead);
            else
                break;
            if(hwndProgress) 
            {
                dwCurrent += dwRead;
                SendMessage(hwndProgress, PBM_SETPOS, dwCurrent * 100 / dwTotal, 0);
            }
        }
        HeapFree(GetProcessHeap(), NULL, pMem);
        CloseHandle(hFile);
        if(hwndProgress) SendMessage(hwndProgress, PBM_SETPOS, 0, 0);
    }
    return ~dwCRC;
}