#include "dxctrlwindow.h"

CDXCtrlWindow::CDXCtrlWindow(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, DWORD dwFrameWidth, PTEXTURERECT ptrFrame, PTEXTURERECT ptrBackground):CDXCtrlBase(dwId, dwX, dwY, dwWidth, dwHeight)
{
	m_trBackground = *ptrBackground;
	m_trFrame = *ptrFrame;
	m_dwFrameWidth = dwFrameWidth;
}

VOID CDXCtrlWindow::Draw()
{
	RECT rect, curr;
	TEXTURERECT tcurr;
	FLOAT inc = (m_trFrame.fRight - m_trFrame.fLeft) / 8;

	GetRect(&rect);

	tcurr = m_trBackground;
	curr = rect;
	curr.left += m_dwFrameWidth / 3;
	curr.right -= m_dwFrameWidth / 4;
	curr.top += m_dwFrameWidth / 5;
	curr.bottom -= m_dwFrameWidth / 4;
	m_pManager->AddRectToBuffer(&curr, &tcurr);
	
	tcurr = m_trFrame;
	tcurr.fRight = inc;
	curr = rect;
	curr.left = curr.left;
	curr.top = curr.top + m_dwFrameWidth;
	curr.right = curr.left + m_dwFrameWidth;
	curr.bottom = curr.bottom - m_dwFrameWidth;
	m_pManager->AddRectToBuffer(&curr, &tcurr);

	tcurr.fLeft += inc;
	tcurr.fRight += inc;
	curr = rect;
	curr.left = curr.right - m_dwFrameWidth;
	curr.top = curr.top + m_dwFrameWidth;
	curr.right = curr.right;
	curr.bottom = curr.bottom - m_dwFrameWidth;
	m_pManager->AddRectToBuffer(&curr, &tcurr);

	tcurr.fLeft += inc;
	tcurr.fRight += inc;
	curr = rect;
	curr.left = curr.left + m_dwFrameWidth;
	curr.top = curr.top;
	curr.right = curr.right - m_dwFrameWidth;
	curr.bottom = curr.top + m_dwFrameWidth;
	m_pManager->AddRectToBuffer(&curr, &tcurr);

	tcurr.fLeft += inc;
	tcurr.fRight += inc;
	curr = rect;
	curr.left = curr.left + m_dwFrameWidth;
	curr.top = curr.bottom - m_dwFrameWidth;
	curr.right = curr.right - m_dwFrameWidth;
	curr.bottom = curr.bottom;
	m_pManager->AddRectToBuffer(&curr, &tcurr);

	tcurr.fLeft += inc;
	tcurr.fRight += inc;
	curr = rect;
	curr.left = curr.left;
	curr.top = curr.top;
	curr.right = curr.left + m_dwFrameWidth;
	curr.bottom = curr.top + m_dwFrameWidth;
	m_pManager->AddRectToBuffer(&curr, &tcurr);

	tcurr.fLeft += inc;
	tcurr.fRight += inc;
	curr = rect;
	curr.left = curr.right - m_dwFrameWidth;
	curr.top = curr.top;
	curr.right = curr.right;
	curr.bottom = curr.top + m_dwFrameWidth;
	m_pManager->AddRectToBuffer(&curr, &tcurr);

	tcurr.fLeft += inc;
	tcurr.fRight += inc;
	curr = rect;
	curr.left = curr.left;
	curr.top = curr.bottom - m_dwFrameWidth;
	curr.right = curr.left + m_dwFrameWidth;
	curr.bottom = curr.bottom;
	m_pManager->AddRectToBuffer(&curr, &tcurr);

	tcurr.fLeft += inc;
	tcurr.fRight += inc;
	curr = rect;
	curr.left = curr.right - m_dwFrameWidth;
	curr.top = curr.bottom - m_dwFrameWidth;
	curr.right = curr.right;
	curr.bottom = curr.bottom;
	m_pManager->AddRectToBuffer(&curr, &tcurr);
}

DWORD CDXCtrlWindow::GetFrameWidth()
{
	return m_dwFrameWidth;
}