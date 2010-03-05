#ifndef _DXCTRLBASE_H
#define _DXCTRLBASE_H

#include <windows.h>
#include "dxctrlmanager.h"

#define IDT_REPEAT 1000
#define IDT_CARET  1001
#define IDC_SCROLLUP 1000
#define IDC_SCROLLDOWN 1001

#define PCDXCtrlBase CDXCtrlBase*
class CDXCtrlBase
{
public:
	CDXCtrlBase(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight);
	~CDXCtrlBase();

	VOID         AddChild(PCDXCtrlBase pChild);
	VOID         AddSibling(PCDXCtrlBase pSibling);
	virtual VOID Draw() {}
	virtual VOID DrawText() {}
	PCDXCtrlBase FindControl(DWORD dwId);
	VOID         GetRect(PRECT pRect);
	VOID         Hide();
	BOOL         HitTest(LONG lX, LONG lY);
	virtual VOID FocusLost() {}
	BOOL         IsVisible();
	virtual BOOL OnChar(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {return FALSE;}
	virtual BOOL OnCommand(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {return FALSE;}
	virtual BOOL OnKeyDown(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {return FALSE;}
	virtual BOOL OnKeyUp(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {return FALSE;}
	virtual BOOL OnLeftButtonDown(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {return FALSE;}
	virtual BOOL OnLeftButtonUp(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {return FALSE;}
	virtual BOOL OnMouseMove(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {return FALSE;}
	virtual BOOL OnMouseWheel(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {return FALSE;}
	virtual BOOL OnTimer(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {return FALSE;}
	VOID         SetManager(PCDXCtrlManager pManager);
	VOID         SetParent(PCDXCtrlBase pParent);
	VOID         SetPosition(DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight);
	VOID         SetupText();
	VOID         SetupVertices();
	VOID         ShiftWindow(LONG lX, LONG lY);
	VOID         Show();
	BOOL         WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	BOOL            m_bVisible;
	DWORD           m_dwId;
	PCDXCtrlBase    m_pChild;
	PCDXCtrlManager m_pManager;
	PCDXCtrlBase    m_pParent;
	PCDXCtrlBase    m_pSibling;
	RECT            m_rPos;

};

#endif