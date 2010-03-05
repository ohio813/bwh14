#ifndef _DXCTRLDIALOG_H
#define _DXCTRLDIALOG_H

#include <windows.h>
#include "dxctrlwindow.h"

#define PCDXCtrlDialog CDXCtrlDialog*
class CDXCtrlDialog : public CDXCtrlWindow
{
public:
	CDXCtrlDialog(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, DWORD dwFrameWidth, PTEXTURERECT ptrFrame, PTEXTURERECT ptrBackground, PSZ pszName);

	BOOL OnLeftButtonDown(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL OnLeftButtonUp(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL OnMouseMove(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	BOOL  m_bDrag;
	POINT m_lastMouse;
};

#endif