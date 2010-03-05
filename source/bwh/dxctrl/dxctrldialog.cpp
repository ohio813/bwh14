#include "dxctrldialog.h"
#include "dxctrlstatic.h"

CDXCtrlDialog::CDXCtrlDialog(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, DWORD dwFrameWidth, PTEXTURERECT ptrFrame, PTEXTURERECT ptrBackground, PSZ pszName):CDXCtrlWindow(dwId, dwX, dwY, dwWidth, dwHeight, dwFrameWidth, ptrFrame, ptrBackground)
{
	PCDXCtrlStatic pTitlebar = new CDXCtrlStatic(0, 0, 10, dwWidth, 12, TRUE, pszName);
	pTitlebar->SetAlignment(DT_CENTER | DT_BOTTOM);
	AddChild(pTitlebar);
	m_bDrag = FALSE;
}

BOOL CDXCtrlDialog::OnLeftButtonDown(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(IsVisible() && m_pChild->HitTest(LOWORD(lParam), HIWORD(lParam)))
	{
		m_bDrag = TRUE;
		m_lastMouse.x = LOWORD(lParam);
		m_lastMouse.y = HIWORD(lParam);
		return TRUE;
	}
	return FALSE;
}

BOOL CDXCtrlDialog::OnLeftButtonUp(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_bDrag)
	{
		m_bDrag = FALSE;
		return TRUE;
	}
	return FALSE;
}

BOOL CDXCtrlDialog::OnMouseMove(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	if(m_bDrag)
	{
		GetRect(&rect);
		SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(m_dwId, EN_CHANGE), MAKELPARAM(rect.left, rect.top));
		ShiftWindow(LOWORD(lParam) - m_lastMouse.x, HIWORD(lParam) - m_lastMouse.y);
		m_lastMouse.x = LOWORD(lParam);
		m_lastMouse.y = HIWORD(lParam);
		return TRUE;
	}
	return FALSE;
}
