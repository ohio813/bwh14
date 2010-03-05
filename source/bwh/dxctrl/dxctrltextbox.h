#ifndef _DXCTRLTEXTBOX_H
#define _DXCTRLTEXTBOX_H

#include <windows.h>
#include "dxctrlbase.h"

#define PCDXCtrlTextbox CDXCtrlTextbox*
class CDXCtrlTextbox : public CDXCtrlBase
{
public:
	CDXCtrlTextbox(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, DWORD dwBufferSize, PTEXTURERECT ptrTextbox, PTEXTURERECT ptrCaret);
	~CDXCtrlTextbox();

	VOID DeleteCharacter(BOOL bPreceeding);
	VOID Draw();
	VOID DrawText();
	VOID FocusLost();
	VOID GetText(PSZ pszText, LONG lBuffSize);
	VOID KillFocus(HWND hwndWindow);
	BOOL OnChar(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL OnKeyDown(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL OnKeyUp(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL OnLeftButtonDown(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL OnTimer(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	VOID InsertCharacter(char c);
	VOID SetCaretTimer(HWND hwnd);
	VOID SetFocus(HWND hwndWindow);
	VOID SetNumbersOnly(BOOL bNumbersOnly);
	VOID SetText(PSZ pszText);

	BOOL        m_bCaretVisible;
	BOOL        m_bControlDown;
	BOOL        m_bNumbersOnly;
	D3DCOLOR    m_Color;
	DWORD       m_dwAlignment;
	DWORD       m_dwBufferLength;
	DWORD       m_dwCaretPosition;
	DWORD       m_dwMaxLength;
	PSZ         m_pszBuffer;
	TEXTURERECT m_trTextbox;
	TEXTURERECT m_trCaret;
};

#endif