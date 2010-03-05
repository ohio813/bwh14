#include "dxctrltextbox.h"

CDXCtrlTextbox::CDXCtrlTextbox(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, DWORD dwMaxLength, PTEXTURERECT ptrTextbox, PTEXTURERECT ptrCaret):CDXCtrlBase(dwId, dwX, dwY, dwWidth, dwHeight)
{
	m_bNumbersOnly = FALSE;
	m_Color = D3DCOLOR_XRGB(255, 255, 255);
	m_dwAlignment = DT_LEFT | DT_BOTTOM;
	m_dwBufferLength = 0;
	m_dwMaxLength = dwMaxLength;
	m_pszBuffer = new char[dwMaxLength + 1];
	m_pszBuffer[0] = '\0';
	m_trTextbox = *ptrTextbox;
	m_trCaret = *ptrCaret;
    m_bCaretVisible = FALSE;
    m_bControlDown = FALSE;
    m_dwCaretPosition = 0;

}

CDXCtrlTextbox::~CDXCtrlTextbox()
{
	delete [] m_pszBuffer;
}

VOID CDXCtrlTextbox::DeleteCharacter(BOOL bPreceeding)
{
	if(bPreceeding)
	{
		if(m_dwCaretPosition)
		{
			for(DWORD x = m_dwCaretPosition; x <= m_dwBufferLength; x++)
				m_pszBuffer[x-1] = m_pszBuffer[x];
			m_dwCaretPosition--;
			m_dwBufferLength--;
		}
	}
	else
	{
		if(m_dwCaretPosition < m_dwBufferLength)
		{
			for(DWORD x = m_dwCaretPosition; x < m_dwBufferLength; x++)
				m_pszBuffer[x] = m_pszBuffer[x+1];
			m_dwBufferLength--;
		}
	}
}

VOID CDXCtrlTextbox::Draw()
{
	RECT rect;
	SIZE size;
	GetRect(&rect);

	m_pManager->AddRectToBuffer(&rect, &m_trTextbox);
	
	if(m_bCaretVisible)
	{
		GetTextExtentPoint32((m_pManager->GetFixedWidthFont())->GetDC(), m_pszBuffer, m_dwCaretPosition, &size);
		rect.left += size.cx + 4;
		rect.top += 4;
		rect.right = rect.left + 2;
		rect.bottom -= 2;
		m_pManager->AddRectToBuffer(&rect, &m_trCaret);

	}
}

VOID CDXCtrlTextbox::DrawText()
{
	RECT rect;
	if(m_pManager)
	{
		GetRect(&rect);
		rect.left += 4;
		rect.right -= 4;
		rect.bottom -= 2;
		(m_pManager->GetFixedWidthFont())->DrawText(m_pManager->GetFontSprite(), m_pszBuffer, -1, &rect, m_dwAlignment, m_Color);
	}
}
VOID CDXCtrlTextbox::KillFocus(HWND hwndWindow)
{
	KillTimer(hwndWindow, IDT_CARET);
	m_pManager->SetFocusedControl(NULL);
}

VOID CDXCtrlTextbox::FocusLost()
{
	m_bCaretVisible = FALSE;
	m_pManager->Redraw();
}

VOID CDXCtrlTextbox::GetText(PSZ pszText, LONG lBuffSize)
{
	strncpy(pszText, m_pszBuffer, lBuffSize - 1);
	pszText[lBuffSize - 1] = '\0';
}

BOOL CDXCtrlTextbox::OnChar(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_pManager->GetFocusedControl() == this)
	{
		if(wParam == VK_ESCAPE)
		{
			KillFocus(hwnd);
		}
		else if(wParam == VK_RETURN)
		{
			KillFocus(hwnd);
			SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(m_dwId, EN_UPDATE), (LPARAM)this);
		}
		else if(wParam == VK_BACK && m_dwBufferLength)
		{
			DeleteCharacter(TRUE);
			SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(m_dwId, EN_CHANGE), (LPARAM)this);
			SetCaretTimer(hwnd);
		}
		else if((m_bNumbersOnly == FALSE && wParam >= 0x20 && wParam <= 0x7E) || (m_bNumbersOnly && wParam >= 0x2D && wParam <= 0x39 && wParam != 0x2F))
		{
			InsertCharacter((char)wParam);
			SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(m_dwId, EN_CHANGE), (LPARAM)this);
			SetCaretTimer(hwnd);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CDXCtrlTextbox::OnKeyDown(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HGLOBAL hglb;
	LPTSTR lptstr;
	if(m_pManager->GetFocusedControl() == this)
	{
		switch(wParam)
		{
		case VK_LEFT:
			if(m_dwCaretPosition)
			{
				m_dwCaretPosition--;
				SetCaretTimer(hwnd);
			}
			break;
		case VK_RIGHT:
			if(m_dwCaretPosition < m_dwBufferLength)
			{
				m_dwCaretPosition++;
				SetCaretTimer(hwnd);
			}
			break;
		case VK_DELETE:
			DeleteCharacter(FALSE);
			SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(m_dwId, EN_CHANGE), (LPARAM)this);
			SetCaretTimer(hwnd);
			break;
		case VK_HOME:
			m_dwCaretPosition = 0;
			SetCaretTimer(hwnd);
			break;
		case VK_END:
			m_dwCaretPosition = m_dwBufferLength;
			SetCaretTimer(hwnd);
			break;
		case VK_CONTROL:
			m_bControlDown = TRUE;
			break;
		case 'C':
			if(m_bControlDown)
				if(OpenClipboard(hwnd))
				{
					EmptyClipboard();
					hglb = GlobalAlloc(GMEM_MOVEABLE, strlen(m_pszBuffer) + 1);
					lptstr = (LPTSTR)GlobalLock(hglb);
					strcpy(lptstr, m_pszBuffer);
					GlobalUnlock(hglb);
					SetClipboardData(CF_TEXT, hglb); 
					CloseClipboard();
				}
			break;
		case 'V':
			if(m_bControlDown)
				if(IsClipboardFormatAvailable(CF_TEXT) && OpenClipboard(hwnd))
				{
					hglb = GetClipboardData(CF_TEXT);
					if(hglb)
					{
						lptstr = (LPTSTR)GlobalLock(hglb);
						if(lptstr)
						{
							strncpy(m_pszBuffer, lptstr, m_dwMaxLength);
							m_pszBuffer[m_dwMaxLength] = '\0';
							m_dwBufferLength = (DWORD)strlen(m_pszBuffer);
							m_dwCaretPosition = m_dwBufferLength;
						}
						GlobalUnlock(hglb);
					}
					CloseClipboard();
				}
			break;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CDXCtrlTextbox::OnKeyUp(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_pManager->GetFocusedControl() == this)
	{
		if(wParam == VK_CONTROL)
		{
			m_bControlDown = FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CDXCtrlTextbox::OnLeftButtonDown(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(IsVisible() && HitTest(LOWORD(lParam), HIWORD(lParam)))
	{
		SetFocus(hwnd);
		return TRUE;
	}
	return FALSE;
}

BOOL CDXCtrlTextbox::OnTimer(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(wParam == IDT_CARET)
	{
		if(m_pManager->GetFocusedControl() == this)
		{
			m_bCaretVisible ^= TRUE;
			m_pManager->Redraw();
			return TRUE;
		}
		else
			m_bCaretVisible = FALSE;
	}
	return FALSE;
}

VOID CDXCtrlTextbox::InsertCharacter(char c)
{
	if(m_dwBufferLength < m_dwMaxLength)
	{
		for(DWORD x = m_dwBufferLength; x > m_dwCaretPosition; x--)
			m_pszBuffer[x] = m_pszBuffer[x-1];

		m_pszBuffer[m_dwCaretPosition++] = c;

		m_pszBuffer[++m_dwBufferLength] = '\0';
	}
}

VOID CDXCtrlTextbox::SetCaretTimer(HWND hwnd)
{
	m_bCaretVisible = TRUE;
	m_pManager->Redraw();
	KillTimer(hwnd, IDT_CARET);
	SetTimer(hwnd, IDT_CARET, GetCaretBlinkTime(), NULL);
}

VOID CDXCtrlTextbox::SetFocus(HWND hwndWindow)
{
	m_pManager->SetFocusedControl(this);
	m_bControlDown = FALSE;
	m_dwCaretPosition = m_dwBufferLength;
	SetCaretTimer(hwndWindow);	
}

VOID CDXCtrlTextbox::SetNumbersOnly(BOOL bNumbersOnly)
{
	m_bNumbersOnly = bNumbersOnly;
}

VOID CDXCtrlTextbox::SetText(PSZ pszText)
{
	strncpy(m_pszBuffer, pszText, m_dwMaxLength);
	m_pszBuffer[m_dwMaxLength] = '\0';
	m_dwBufferLength = (DWORD)strlen(pszText);
}
