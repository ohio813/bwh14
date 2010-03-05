// Public includes
#include <windows.h>

// GetFileCRC - Determines the checksum of a given file.
//
// PSZ pszFilename - Path and filename of the file to check.
// pbStop          - Pointer to a stop variable. Must not be NULL.
// hwndProgress    - Window handle of a progress bar. Can be NULL.
//
// Returns: Checksum of the file. If the functions fails, -1 will
//          be returned, but note that -1 is also a valid checksum!
DWORD GetFileCRC(PSZ pszFilename, PBOOL pbStop, HWND hwndProgress);
