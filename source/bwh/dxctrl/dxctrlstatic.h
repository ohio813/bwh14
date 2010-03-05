#ifndef _DXCTRLSTATIC_H
#define _DXCTRLSTATIC_H

#include <windows.h>
#include "dxctrlbase.h"

#define PCDXCtrlStatic CDXCtrlStatic*
class CDXCtrlStatic : public CDXCtrlBase
{
public:
	CDXCtrlStatic(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, BOOL bFixedWidth, PSZ pszText);
	
	VOID DrawText();
	VOID SetAlignment(DWORD dwAlignment);
	VOID SetColor(D3DCOLOR Color);
	VOID SetText(PSZ pszText);

	D3DCOLOR m_Color;
	DWORD    m_dwAlignment;
	char     m_szText[31];
	BOOL     m_bFixedWidth;
};

#endif