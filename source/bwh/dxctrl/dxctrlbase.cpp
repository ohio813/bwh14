#include "dxctrlbase.h"

CDXCtrlBase::CDXCtrlBase(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight)
{
	m_bVisible = TRUE;
	m_dwId = dwId;
	m_pChild = NULL;
	m_pManager = NULL;
	m_pParent = NULL;
	m_pSibling = NULL;
	SetPosition(dwX, dwY, dwWidth, dwHeight);
}

CDXCtrlBase::~CDXCtrlBase()
{
	if(m_pSibling)
		delete m_pSibling;
	if(m_pChild)
		delete m_pChild;
}

VOID CDXCtrlBase::AddChild(PCDXCtrlBase pChild)
{
	if(m_pChild)
		m_pChild->AddSibling(pChild);
	else
		m_pChild = pChild;
	
	pChild->SetParent(this);
}

VOID CDXCtrlBase::AddSibling(PCDXCtrlBase pSibling)
{
	if(m_pSibling)
		m_pSibling->AddSibling(pSibling);
	else
		m_pSibling = pSibling;

	pSibling->SetManager(m_pManager);
	pSibling->SetParent(m_pParent);
}

PCDXCtrlBase CDXCtrlBase::FindControl(DWORD dwId)
{
	PCDXCtrlBase pResult = NULL;

	if(m_dwId == dwId && (pResult = this));
	else if(m_pSibling && (pResult = m_pSibling->FindControl(dwId)));
	else(m_pChild && (pResult = m_pChild->FindControl(dwId)));

	return pResult;
}

VOID CDXCtrlBase::GetRect(PRECT pRect)
{
	PCDXCtrlBase pParent = m_pParent;
	*pRect = m_rPos;
	while(pParent)
	{
		pRect->left += pParent->m_rPos.left;
		pRect->right += pParent->m_rPos.left;
		pRect->top += pParent->m_rPos.top;
		pRect->bottom += pParent->m_rPos.top;
		pParent = pParent->m_pParent;
	}
}

VOID CDXCtrlBase::Hide()
{
	m_bVisible = FALSE;
	if(m_pManager)
		m_pManager->Redraw();
}

BOOL CDXCtrlBase::HitTest(LONG lX, LONG lY)
{
	RECT rect;
	GetRect(&rect);
	if(lX >= rect.left && lX <= rect.right && lY >= rect.top && lY <= rect.bottom)
		return TRUE;
	else
		return FALSE;
}

BOOL CDXCtrlBase::IsVisible()
{
	if(m_bVisible)
	{
		if(m_pParent)
			return m_pParent->IsVisible();
		else
			return TRUE;
	}
	else
		return FALSE;
}

VOID CDXCtrlBase::SetManager(PCDXCtrlManager pManager)
{
	m_pManager = pManager;
	if(m_pChild)
		m_pChild->SetManager(pManager);
	if(m_pSibling)
		m_pSibling->SetManager(pManager);
}

VOID CDXCtrlBase::SetParent(PCDXCtrlBase pParent)
{
	m_pParent = pParent;
}

VOID CDXCtrlBase::SetPosition(DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight)
{
	m_rPos.left = dwX;
	m_rPos.top = dwY;
	m_rPos.right = dwX + dwWidth;
	m_rPos.bottom = dwY + dwHeight;
	if(m_pManager)
		m_pManager->Redraw();
}

VOID CDXCtrlBase::SetupVertices()
{
	if(IsVisible())
	{
		Draw();

		if(m_pChild)
			m_pChild->SetupVertices();

	}
	if(m_pSibling)
		m_pSibling->SetupVertices();
}

VOID CDXCtrlBase::SetupText()
{
	if(IsVisible())
	{
		DrawText();

		if(m_pChild)
			m_pChild->SetupText();
	}
	if(m_pSibling)
		m_pSibling->SetupText();
}

VOID CDXCtrlBase::ShiftWindow(LONG lX, LONG lY)
{
	m_rPos.left += lX;
	m_rPos.right += lX;
	m_rPos.top += lY;
	m_rPos.bottom += lY;
	
	if(m_pManager)
		m_pManager->Redraw();
}

VOID CDXCtrlBase::Show()
{
	m_bVisible = TRUE;
	if(m_pManager)
		m_pManager->Redraw();
}

BOOL CDXCtrlBase::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_pSibling && m_pSibling->WindowProc(hwnd, uMsg, wParam, lParam))
		return TRUE;

	if(m_pChild && m_pChild->WindowProc(hwnd, uMsg, wParam, lParam))
		return TRUE;

	switch(uMsg)
	{
	case WM_CHAR:
		return OnChar(hwnd, uMsg, wParam, lParam);
	case WM_COMMAND:
		return OnCommand(hwnd, uMsg, wParam, lParam);
	case WM_KEYDOWN:
		return OnKeyDown(hwnd, uMsg, wParam, lParam);
	case WM_KEYUP:
		return OnKeyUp(hwnd, uMsg, wParam, lParam);
	case WM_LBUTTONDOWN:
		return OnLeftButtonDown(hwnd, uMsg, wParam, lParam);
	case WM_LBUTTONUP:
		return OnLeftButtonUp(hwnd, uMsg, wParam, lParam);
	case WM_MOUSEMOVE:
		return OnMouseMove(hwnd, uMsg, wParam, lParam);
	case 0x020A:
		return OnMouseWheel(hwnd, uMsg, wParam, lParam);
	case WM_TIMER:
		return OnTimer(hwnd, uMsg, wParam, lParam);
	}

	return FALSE;
}