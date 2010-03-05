#include "dxctrlbutton.h"
#include "dxctrlstatic.h"

CDXCtrlButton::CDXCtrlButton(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, PTEXTURERECT ptrNormal, PTEXTURERECT ptrPushed, PTEXTURERECT ptrDisabled, PSZ pszText):CDXCtrlBase(dwId, dwX, dwY, dwWidth, dwHeight)
{
	m_bClicked = FALSE;
	m_bCheck = FALSE;
	m_bChecked = FALSE;
	m_bEnabled = TRUE;
	m_bPushed = FALSE;
	m_bRepeat = FALSE;
	m_DisabledColor = D3DCOLOR_XRGB(128, 128, 128);
	m_NormalColor = D3DCOLOR_XRGB(255, 209, 0);

	m_trNormal = *ptrNormal;
	m_trPushed = *ptrPushed;
	m_trDisabled = *ptrDisabled;

	PCDXCtrlStatic pCaption = new CDXCtrlStatic(0, 0, 0, dwWidth, dwHeight, FALSE, pszText);
	pCaption->SetColor(m_NormalColor);
	AddChild(pCaption);
}

VOID CDXCtrlButton::Disable()
{
	((PCDXCtrlStatic)m_pChild)->SetColor(m_DisabledColor);
	m_bEnabled = FALSE;
	if(m_pManager)
		m_pManager->Redraw();
}

VOID CDXCtrlButton::Draw()
{
	RECT rect;
	TEXTURERECT trect;
	GetRect(&rect);

	if(m_bEnabled)
	{
		if(m_bPushed && m_bCheck == FALSE)
			trect = m_trPushed;
		else
			trect = m_trNormal;
	}
	else
		trect = m_trDisabled;

	m_pManager->AddRectToBuffer(&rect, &trect);
	if(m_bChecked)
		m_pManager->AddRectToBuffer(&rect, &m_trPushed);
}

VOID CDXCtrlButton::Enable()
{
	((PCDXCtrlStatic)m_pChild)->SetColor(m_NormalColor);
	m_bEnabled = TRUE;
	if(m_pManager)
		m_pManager->Redraw();
}

BOOL CDXCtrlButton::GetCheckState()
{
	return m_bChecked;
}

BOOL CDXCtrlButton::OnLeftButtonDown(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(IsVisible() && m_bEnabled && HitTest(LOWORD(lParam), HIWORD(lParam)))
	{
		if(m_bRepeat && m_bCheck == FALSE)
		{
			KillTimer(hwnd, IDT_REPEAT);
			SetTimer(hwnd, IDT_REPEAT, 200, NULL);
		}
		m_bClicked = TRUE;
		SetPushedState();
		return TRUE;
	}
	return FALSE;
}

BOOL CDXCtrlButton::OnLeftButtonUp(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_bClicked)
	{
		m_bClicked = FALSE;
		if(m_bPushed)
		{
			if(m_bCheck)
				m_bChecked ^= TRUE;
			SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(m_dwId, BN_CLICKED), (LPARAM)this);
		}
		SetNormalState();			
		if(m_bRepeat && m_bCheck == FALSE)
			KillTimer(hwnd, IDT_REPEAT);
		return TRUE;
	}
	return FALSE;
}

BOOL CDXCtrlButton::OnMouseMove(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_bClicked)
	{
		if(HitTest(LOWORD(lParam), HIWORD(lParam)))
			SetPushedState();
		else
			SetNormalState();
		return TRUE;
	}
	return FALSE;
}

BOOL CDXCtrlButton::OnTimer(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(wParam == IDT_REPEAT && m_bPushed)
	{
		KillTimer(hwnd, IDT_REPEAT);
		if(m_bEnabled)
		{
			SetTimer(hwnd, IDT_REPEAT, 25, NULL);
			SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(m_dwId, BN_CLICKED), (LPARAM)this);
		}
		else
		{
			SetNormalState();
			m_bClicked = FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

VOID CDXCtrlButton::SetCheckable(BOOL bCheckable)
{
	m_bCheck = bCheckable;
}

VOID CDXCtrlButton::SetCheckState(BOOL bChecked)
{
	if(m_bCheck)
	{
		if(m_bChecked != bChecked)
		{
			SetPushedState();
			m_bChecked = bChecked;
			SetNormalState();
		}
	}
}

VOID CDXCtrlButton::SetNormalState()
{
	if(m_bPushed == TRUE)
	{
		if(m_bChecked == FALSE)
			m_pChild->ShiftWindow(-1, -1);
		m_bPushed = FALSE;
		m_pManager->Redraw();
	}
}

VOID CDXCtrlButton::SetPushedState()
{
	if(m_bPushed == FALSE)
	{
		if(m_bChecked == FALSE)
			m_pChild->ShiftWindow(1, 1);
		m_bPushed = TRUE;
		m_pManager->Redraw();
	}
}

VOID CDXCtrlButton::SetRepeatable(BOOL bRepeatable)
{
	m_bRepeat = bRepeatable;
}