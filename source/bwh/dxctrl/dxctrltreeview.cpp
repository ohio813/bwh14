#include "dxctrltreeview.h"
#include "dxctrlbutton.h"

CDXCtrlTreeview::CDXCtrlTreeview(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, DWORD dwFrameWidth, PTEXTURERECT ptrFrame, PTEXTURERECT ptrBackground, PTEXTURERECT ptrItemHighlight, PTEXTURERECT ptrGroupHighlight, PTEXTURERECT ptrCollapse, PTEXTURERECT ptrExpand, PTEXTURERECT ptrScrollUpUp, PTEXTURERECT ptrScrollUpDown, PTEXTURERECT ptrScrollUpDisabled, PTEXTURERECT ptrScrollDownUp, PTEXTURERECT ptrScrollDownDown, PTEXTURERECT ptrScrollDownDisabled, PTEXTURERECT ptrKnob, BOOL bShowItemIndex):CDXCtrlWindow(dwId, dwX, dwY, dwWidth, dwHeight, dwFrameWidth, ptrFrame, ptrBackground)
{
	m_pList = NULL;
	m_dwListCount = 0;
	m_dwTopIndex = 0;
	m_ItemColor = D3DCOLOR_XRGB(0, 192, 255);
	m_GroupColor = D3DCOLOR_XRGB(192, 192, 192);
	m_SelectedColor = D3DCOLOR_XRGB(255, 255, 255);
	m_dwSelectedIndex = -1;
	m_trExpand = *ptrExpand;
	m_trCollapse = *ptrCollapse;
	m_trItemHighlight = *ptrItemHighlight;
	m_trGroupHighlight = *ptrGroupHighlight;
	m_trKnob = *ptrKnob;
	m_bShowIndex = bShowItemIndex;
	m_bMenuStyle = FALSE;

	PCDXCtrlButton pButton = new CDXCtrlButton(IDC_SCROLLUP, dwWidth - 16 - (5 * m_dwFrameWidth / 12), (5 * m_dwFrameWidth / 12), 16, 16, ptrScrollUpUp, ptrScrollUpDown, ptrScrollUpDisabled, "");
	pButton->Disable();
	pButton->SetRepeatable(TRUE);
	AddChild(pButton);
	pButton = new CDXCtrlButton(IDC_SCROLLDOWN, dwWidth - 16 - (5 * m_dwFrameWidth / 12), dwHeight - 16 - (5 * m_dwFrameWidth / 12), 16, 16, ptrScrollDownUp, ptrScrollDownDown, ptrScrollDownDisabled, "");
	pButton->Disable();
	pButton->SetRepeatable(TRUE);
	AddChild(pButton);
}

CDXCtrlTreeview::~CDXCtrlTreeview()
{
	ClearList();
}

VOID CDXCtrlTreeview::AddItem(PSZ pszText, BOOL bGroup, DWORD dwPos)
{
	PLISTITEM* Insert = &m_pList;

	PLISTITEM NewItem = new LISTITEM;
	strncpy(NewItem->szText, pszText, 100);
	NewItem->szText[100] = '\0';
	NewItem->bGroup = bGroup;
	NewItem->bVisible = TRUE;
	NewItem->link = m_pList;

	if(dwPos > m_dwListCount)
		dwPos = m_dwListCount;
	m_dwListCount++;

	for(DWORD x = 0; x < dwPos; x++)
	{
		if(NewItem->bGroup == FALSE && NewItem->link && NewItem->link->bGroup)
			NewItem->bVisible = NewItem->link->bVisible;
		Insert = &NewItem->link->link;
		NewItem->link = *Insert;
	}
	*Insert = NewItem;
	
	if(NewItem->bGroup)
	{
		Insert = &NewItem->link;
		while(*Insert && (*Insert)->bGroup == FALSE)
		{
			(*Insert)->bVisible = TRUE;
			Insert = &(*Insert)->link;
		}
	}
	ScrollDown(0);
}

VOID CDXCtrlTreeview::ClearList()
{
	PLISTITEM Delete, Next = m_pList;
	while(Next)
	{
		Delete = Next;
		Next = Next->link;
		delete Delete;
	}
	m_pList = NULL;
	m_dwListCount = 0;
	m_dwTopIndex = 0;
	m_dwSelectedIndex = -1;
	((PCDXCtrlButton)m_pChild->FindControl(IDC_SCROLLDOWN))->Disable();
	((PCDXCtrlButton)m_pChild->FindControl(IDC_SCROLLUP))->Disable();
	m_pManager->Redraw();

}

VOID CDXCtrlTreeview::DeleteItem(DWORD dwPos)
{
	PLISTITEM pVisible, pItem = m_pList;
	PLISTITEM* ppLast = &m_pList;
	BOOL bVisible = TRUE;

	if(m_dwListCount)
	{
		if(dwPos > --m_dwListCount)
			dwPos = m_dwListCount;
		
		for(DWORD x = 0; x < dwPos; x++)
		{
			bVisible = pItem->bVisible;
			ppLast = &pItem->link;
			pItem = pItem->link;
		}
		
		if(pItem->bGroup)
		{
			pVisible = pItem->link;
			while(pVisible && pVisible->bGroup == FALSE)
			{
				pVisible->bVisible = bVisible;
				pVisible = pVisible->link;
			}
		}

		*ppLast = pItem->link;
		delete pItem;

		if(m_dwSelectedIndex == m_dwListCount)
			m_dwSelectedIndex--;
	}
}

VOID CDXCtrlTreeview::Draw()
{
	RECT hrect, brect, krect;
	TEXTMETRIC tm;
	DWORD dwIndex;
	PLISTITEM pList, pItem = NULL;
	LONG lBottom;
	DWORD dwTopCount = 0, dwTotalCount = 0, dwItemCount = 0;

	CDXCtrlWindow::Draw();
	
	GetRect(&hrect);
	krect = hrect;
	brect = hrect;
	
	pList = m_pList;
	
	for(DWORD x = 0; pList; x++)
	{
		if(pList->bGroup || pList->bVisible)
		{
			dwTotalCount++;
			if(x < m_dwTopIndex)
				dwTopCount++;
		}
		if(x == m_dwTopIndex)
		{
			pItem = pList;
			dwIndex = x;
		}
		pList = pList->link;
	}
	
	((LPD3DXFONT)m_pManager->GetFixedWidthFont())->GetTextMetrics(&tm);
	
	hrect.left += (5 * m_dwFrameWidth / 8) + tm.tmHeight;
	hrect.top += (5 * m_dwFrameWidth / 8);
	lBottom = hrect.bottom - (5 * m_dwFrameWidth / 8) - tm.tmHeight;
	hrect.bottom = hrect.top + tm.tmHeight;//-= (5 * m_dwFrameWidth / 8);
	hrect.right -= ((5 * m_dwFrameWidth / 8) + 16);

	brect.left += (5 * m_dwFrameWidth / 10);
	brect.right = brect.left + tm.tmHeight;
	
	for(; hrect.top <= lBottom && pItem; dwIndex++)
	{
		if(pItem->bGroup || pItem->bVisible)
		{
			brect.top = hrect.top;
			brect.bottom = brect.top + tm.tmHeight;
			if(pItem->bGroup)
			{
				if(pItem->bVisible)
					m_pManager->AddRectToBuffer(&brect, &m_trCollapse);
				else
					m_pManager->AddRectToBuffer(&brect, &m_trExpand);
			}
			if(dwIndex == m_dwSelectedIndex)
			{
				if(pItem->bGroup)
					m_pManager->AddRectToBuffer(&hrect, &m_trGroupHighlight);
				else
					m_pManager->AddRectToBuffer(&hrect, &m_trItemHighlight);
			}
			hrect.top += tm.tmHeight;
			hrect.bottom += tm.tmHeight;
			dwTotalCount--;
		}
		pItem = pItem->link;
	}
	if(dwTotalCount)
	{
		krect.right -= (5 * m_dwFrameWidth / 12);
		krect.left = krect.right - 16;
		krect.top += ((5 * m_dwFrameWidth / 12) + 16);
		krect.bottom -= ((5 * m_dwFrameWidth / 12) + 16 + 16);
		krect.top = ((krect.bottom - krect.top) * dwTopCount / dwTotalCount) + krect.top;
		krect.bottom = krect.top + 16;
		m_pManager->AddRectToBuffer(&krect, &m_trKnob);	
	}
}

VOID CDXCtrlTreeview::DrawText()
{
	TEXTMETRIC tm;
	LPD3DXFONT pFont;
	RECT rect, irect;
	PLISTITEM pItem;
	D3DCOLOR Color;
	DWORD dwIndex;
	DWORD dwIndexTextSize;
	SIZE size;
	char szBuff[8];
	if(m_pManager)
	{

		GetRect(&rect);
		pFont = m_pManager->GetFixedWidthFont();
		pFont->GetTextMetrics(&tm);
		
		GetTextExtentPoint32(pFont->GetDC(), "999", 3, &size);
		
		if(m_bShowIndex)
			dwIndexTextSize = size.cx;
		else
			dwIndexTextSize = 0;

		pItem = m_pList;
		for(dwIndex = 0; dwIndex < m_dwTopIndex; dwIndex++)
			pItem = pItem->link;
		rect.left += (5 * m_dwFrameWidth / 8) + tm.tmHeight;
		rect.top += (5 * m_dwFrameWidth / 8);
		rect.bottom -= (5 * m_dwFrameWidth / 8);
		rect.right -= ((5 * m_dwFrameWidth / 8) + 16 + dwIndexTextSize);
		for(; rect.top <= rect.bottom - tm.tmHeight && pItem; dwIndex++)
		{
			if(pItem->bGroup || pItem->bVisible)
			{
				if(dwIndex == m_dwSelectedIndex)
				{
					Color = m_SelectedColor;
				}
				else
				{
					if(pItem->bGroup)
						Color = m_GroupColor;
					else
						Color = m_ItemColor;
				}
				pFont->DrawTextA(m_pManager->GetFontSprite(), pItem->szText, -1, &rect, DT_TOP | DT_LEFT, Color);
				if(m_bShowIndex && pItem->bGroup == FALSE)
				{
					irect = rect;
					irect.left = irect.right;
					irect.right = irect.left + dwIndexTextSize;
					pFont->DrawTextA(m_pManager->GetFontSprite(), itoa(dwIndex, szBuff, 10), -1, &irect, DT_TOP | DT_RIGHT, Color);
				}
				rect.top += tm.tmHeight;
			}
			pItem = pItem->link;
		}
	}
}

DWORD CDXCtrlTreeview::GetClickedIndex(LONG lX, LONG lY)
{
	RECT rect;
	TEXTMETRIC tm;
	PLISTITEM pItem;
	DWORD dwIndex;
	LONG lBottom;
	((LPD3DXFONT)m_pManager->GetFixedWidthFont())->GetTextMetrics(&tm);
	
	pItem = m_pList;
	for(dwIndex = 0; dwIndex < m_dwTopIndex; dwIndex++)
		pItem = pItem->link;
	
	GetRect(&rect);
	rect.left += (5 * m_dwFrameWidth / 8);
	rect.top += (5 * m_dwFrameWidth / 8);
	lBottom = rect.bottom - (5 * m_dwFrameWidth / 8) - tm.tmHeight;
	rect.bottom = rect.top + tm.tmHeight;
	rect.right -= ((5 * m_dwFrameWidth / 8) + 16);
	
	for(; rect.top <= lBottom && pItem; dwIndex++)
	{
		if(pItem->bGroup || pItem->bVisible)
		{
			if(lX >= rect.left && lX <= rect.right && lY >= rect.top && lY <= rect.bottom)
			{
				return dwIndex;
			}
			rect.top += tm.tmHeight;
			rect.bottom += tm.tmHeight;
		}
		pItem = pItem->link;
	}
	return -1;
}
DWORD CDXCtrlTreeview::GetSelected()
{
	return m_dwSelectedIndex;
}

VOID CDXCtrlTreeview::SetText(PSZ pszText, DWORD dwIndex)
{
	PLISTITEM pItem = m_pList;

	if(dwIndex >= m_dwListCount)
		dwIndex = m_dwListCount - 1;

	if(m_dwListCount != -1)
	{
		for(DWORD x = 0; x < dwIndex; x++)
			pItem = pItem->link;
		strncpy(pItem->szText, pszText, 20);
		pItem->szText[20] = '\0';
	}
}

VOID CDXCtrlTreeview::GetText(PSZ pszText, LONG lBuffSize, DWORD dwIndex)
{
	PLISTITEM pItem = m_pList;
	
	if(dwIndex >= m_dwListCount)
		dwIndex = m_dwListCount - 1;

	if(m_dwListCount == -1)
		pszText[0] = '\0';
	else
	{
		for(DWORD x = 0; x < dwIndex; x++)
			pItem = pItem->link;

		strncpy(pszText, pItem->szText, lBuffSize - 1);
		pszText[lBuffSize - 1] = '\0';
	}
}

VOID CDXCtrlTreeview::MoveItem(DWORD dwPos, BOOL bUp)
{
	PLISTITEM *ppTemp, *ppPrev = &m_pList;
	PLISTITEM pMover = m_pList, pLastGroup = NULL;

	if(dwPos > m_dwListCount)
		dwPos = m_dwListCount - (bUp ? 1 : 2);
	if(bUp == FALSE) dwPos++;
	if(m_dwListCount > 1 && dwPos)
	{
		if(pMover->bGroup)
			pLastGroup = pMover;

		for(DWORD x = 0; x < dwPos - 1; x++)
		{
			ppPrev = &pMover->link;
			pMover = pMover->link;

			if(pMover->bGroup)
				pLastGroup = pMover;
		}
		if(pLastGroup)
		{
			pLastGroup->bVisible = TRUE;
			pLastGroup = pLastGroup->link;
			while(pLastGroup && pLastGroup->bGroup == FALSE)
			{
				pLastGroup->bVisible = TRUE;
				pLastGroup = pLastGroup->link;
			}
		}
		*ppPrev = pMover->link;
		ppTemp = &pMover->link->link;
		pMover->link = pMover->link->link;
		*ppTemp = pMover;
		if((*ppPrev)->bGroup && pMover->bGroup == FALSE)
			pMover->bVisible = (*ppPrev)->bVisible;

		if(pMover->bGroup == TRUE)
		{
			pLastGroup = pMover->link;
			while(pLastGroup)
			{
				pLastGroup->bVisible = pMover->bVisible;
				pLastGroup = pLastGroup->link;
			}
		}
		if(bUp)
			m_dwSelectedIndex--;
		else
			m_dwSelectedIndex++;
		m_pManager->Redraw();
	}
}

BOOL CDXCtrlTreeview::OnCommand(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WORD wId, wCode;
	wId = LOWORD(wParam);
	wCode = HIWORD(wParam);
	if(wCode == BN_CLICKED)
	{
		if(wId == IDC_SCROLLUP && (PCDXCtrlBase)lParam == m_pChild->FindControl(IDC_SCROLLUP))
		{
			ScrollUp(1);
			return TRUE;
		}
		else if(wId == IDC_SCROLLDOWN && (PCDXCtrlBase)lParam == m_pChild->FindControl(IDC_SCROLLDOWN))
		{
			ScrollDown(1);
			return TRUE;
		}
	}
	return FALSE;
}
VOID CDXCtrlTreeview::SetIndexStyle(BOOL bIndex)
{
	m_bShowIndex = bIndex;
}

VOID CDXCtrlTreeview::SetMenuStyle(BOOL bMenu)
{
	m_bMenuStyle = bMenu;
}

BOOL CDXCtrlTreeview::OnMouseMove(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_bMenuStyle && IsVisible() && HitTest(LOWORD(lParam), HIWORD(lParam)))
	{
		m_dwSelectedIndex = GetClickedIndex(LOWORD(lParam), HIWORD(lParam));
		m_pManager->Redraw();
		return TRUE;
	}
	return FALSE;
}

BOOL CDXCtrlTreeview::OnLeftButtonDown(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DWORD dwIndex;
	DWORD dwCurTime;

	if(IsVisible() && HitTest(LOWORD(lParam), HIWORD(lParam)))
	{
		dwIndex = GetClickedIndex(LOWORD(lParam), HIWORD(lParam));
		if(dwIndex != -1 && dwIndex == m_dwSelectedIndex)
			ToggleGroup(dwIndex);
		m_dwSelectedIndex = dwIndex;
		SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(m_dwId, LBN_SELCHANGE), (LPARAM)this);
		m_pManager->Redraw();
		
		
		dwCurTime = GetTickCount();
		if(m_dwLastClickPos == (DWORD)lParam && dwCurTime - m_dwLastClickTime < 1000)
		{
			SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(m_dwId, LBN_DBLCLK), (LPARAM)this);
			m_dwLastClickTime = 0;
		}
		else
			m_dwLastClickTime = dwCurTime;
		m_dwLastClickPos = (DWORD)lParam;
		
		return TRUE;
	}
	return FALSE;
}

BOOL CDXCtrlTreeview::OnMouseWheel(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT p;
	p.x = LOWORD(lParam);
	p.y = HIWORD(lParam);
	ScreenToClient(hwnd, &p);
	if(IsVisible() && HitTest(p.x, p.y))
	{
		if(HIWORD(wParam) == 120)
			ScrollUp(3);
		else
			ScrollDown(3);
		return TRUE;
	}
	return FALSE;
}

VOID CDXCtrlTreeview::RemoveItem(DWORD dwPos)
{
	PLISTITEM* pDelete = &m_pList;
	PLISTITEM Delete;

	if(m_dwListCount == 0)
		return;

	if(dwPos > m_dwListCount--)
		dwPos = m_dwListCount;

	for(DWORD x = 0; x < dwPos; x++)
		pDelete = &(*pDelete)->link;

	Delete = *pDelete;
	*pDelete = (*pDelete)->link;

	if(Delete->bGroup)
	{
		pDelete = &Delete->link;
		while(*pDelete && (*pDelete)->bGroup == FALSE)
		{
			(*pDelete)->bVisible = Delete->bVisible;
			pDelete = &(*pDelete)->link;
		}
	}

	delete Delete;
	ScrollDown(0);
}

VOID CDXCtrlTreeview::ScrollDown(DWORD dwCount)
{
	PLISTITEM pItem;
	DWORD dwIndex;
	if(m_pManager)
	{
		for(dwIndex = 0, pItem = m_pList; pItem && dwCount; dwIndex++, pItem = pItem->link)
		{
			if(dwIndex > m_dwTopIndex && (pItem->bGroup || pItem->bVisible))
			{
				m_dwTopIndex = dwIndex;
				dwCount--;
			}
		}
		if(ValidateTopIndex())
			((PCDXCtrlButton)m_pChild->FindControl(IDC_SCROLLDOWN))->Disable();
		else
			((PCDXCtrlButton)m_pChild->FindControl(IDC_SCROLLDOWN))->Enable();
		if(m_dwTopIndex)
			((PCDXCtrlButton)m_pChild->FindControl(IDC_SCROLLUP))->Enable();
		else
			((PCDXCtrlButton)m_pChild->FindControl(IDC_SCROLLUP))->Disable();
		m_pManager->Redraw();
	}
}

VOID CDXCtrlTreeview::ScrollUp(DWORD dwCount)
{
	PLISTITEM pItem;
	DWORD dwLastIndex = 0;
	DWORD dwIndex;
	if(m_pManager)
	{
		for(; dwCount; dwCount--)
		{
			for(dwIndex = 0, pItem = m_pList; pItem && dwIndex < m_dwTopIndex; dwIndex++, pItem = pItem->link)
			{
				if(pItem->bGroup || pItem->bVisible)
					dwLastIndex = dwIndex;
			}
			m_dwTopIndex = dwLastIndex;
		}
		if((INT)(m_dwTopIndex - dwCount) <= 0)
		{
			m_dwTopIndex = 0;
			((PCDXCtrlButton)m_pChild->FindControl(IDC_SCROLLUP))->Disable();
		}
		else
			m_dwTopIndex -= dwCount;
		if(ValidateTopIndex() == FALSE)
			((PCDXCtrlButton)m_pChild->FindControl(IDC_SCROLLDOWN))->Enable();
		m_pManager->Redraw();
	}
}

VOID CDXCtrlTreeview::ToggleGroup(DWORD dwItemIndex)
{
	PLISTITEM pItem, pItem2;
	DWORD dwIndex;

	pItem = m_pList;
	for(dwIndex = 0; dwIndex < dwItemIndex; dwIndex++)
		pItem = pItem->link;

	if(pItem->bGroup)
	{
		pItem->bVisible ^= TRUE;

		for(pItem2 = pItem->link; pItem2 && pItem2->bGroup == FALSE; pItem2 = pItem2->link)
		{
			pItem2->bVisible = pItem->bVisible;
		}
	}
	ScrollDown(0);
}

BOOL CDXCtrlTreeview::ValidateTopIndex()
{
	PLISTITEM pItem;
	DWORD dwVisibleCount = 0;
	DWORD dwItemsOnPage = 0;
	RECT rect;
	TEXTMETRIC tm;
	DWORD dwIndex, dwLastIndex = 0;

	// count number of items on a page
	GetRect(&rect);
	(m_pManager->GetFixedWidthFont())->GetTextMetrics(&tm);
	rect.top += (5 * m_dwFrameWidth / 8);
	rect.bottom -= (5 * m_dwFrameWidth / 8);
	for(; rect.top <= rect.bottom - tm.tmHeight; rect.top += tm.tmHeight)
		dwItemsOnPage++;

	// first go to the current index
	for(dwIndex = 0, pItem = m_pList; pItem && dwIndex != m_dwTopIndex; pItem = pItem->link)
		dwIndex++;

	// now count how many visible entries are left
	for(; pItem; pItem = pItem->link)
	{
		if(pItem->bGroup || pItem->bVisible)
			dwVisibleCount++;
	}

	if(dwVisibleCount <= dwItemsOnPage)
	{
		// well scroll down needs to be disabled
		// also we need to adjust the top index
		for(INT iDelta = (INT)dwItemsOnPage - dwVisibleCount; iDelta; iDelta--)
		{
			for(dwIndex = 0, pItem = m_pList; pItem && dwIndex < m_dwTopIndex; dwIndex++, pItem = pItem->link)
			{
				if(pItem->bGroup || pItem->bVisible)
					dwLastIndex = dwIndex;
			}
			m_dwTopIndex = dwLastIndex;
		}
		return TRUE;
	}
	else
		return FALSE;	
}