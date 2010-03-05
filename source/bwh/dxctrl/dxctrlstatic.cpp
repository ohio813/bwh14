#include "dxctrlstatic.h"

CDXCtrlStatic::CDXCtrlStatic(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, BOOL bFixedWidth, PSZ pszText):CDXCtrlBase(dwId, dwX, dwY, dwWidth, dwHeight)
{
	m_Color = D3DCOLOR_XRGB(255, 209, 0);
	m_dwAlignment = DT_CENTER | DT_VCENTER;
	SetText(pszText);
	m_bFixedWidth = bFixedWidth;
}

VOID CDXCtrlStatic::DrawText()
{
	RECT rect;
	LPD3DXFONT pFont;
	if(m_pManager)
	{
		if(m_bFixedWidth)
			pFont = m_pManager->GetFixedWidthFont();
		else
			pFont = m_pManager->GetProportionalFont();
		GetRect(&rect);
		pFont->DrawText(m_pManager->GetFontSprite(), m_szText, -1, &rect, m_dwAlignment, m_Color);
	}
}

VOID CDXCtrlStatic::SetAlignment(DWORD dwAlignment)
{
	m_dwAlignment = dwAlignment;
}

VOID CDXCtrlStatic::SetColor(D3DCOLOR Color)
{
	m_Color = Color;
}

VOID CDXCtrlStatic::SetText(PSZ pszText)
{
	strcpy(m_szText, pszText);
}