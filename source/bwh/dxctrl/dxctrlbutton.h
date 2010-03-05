#ifndef _DXCTRLBUTTON_H
#define _DXCTRLBUTTON_H

#include "dxctrlbase.h"

#define PCDXCtrlButton CDXCtrlButton*
class CDXCtrlButton : public CDXCtrlBase
{
public:
	CDXCtrlButton(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, PTEXTURERECT ptrNormal, PTEXTURERECT ptrPushed, PTEXTURERECT ptrDisabled, PSZ pszText);

	VOID Disable();
	VOID Draw();
	VOID Enable();
    BOOL GetCheckState();
	BOOL OnLeftButtonDown(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL OnLeftButtonUp(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL OnMouseMove(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL OnTimer(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	VOID SetCheckable(BOOL bCheckable);
	VOID SetCheckState(BOOL bChecked);
	VOID SetNormalState();
	VOID SetPushedState();
	VOID SetRepeatable(BOOL bRepeatable);
	
	BOOL        m_bClicked;
	BOOL        m_bCheck;
	BOOL        m_bChecked;
	BOOL        m_bEnabled;
	BOOL        m_bPushed;
	BOOL        m_bRepeat;
	D3DCOLOR    m_DisabledColor;
	D3DCOLOR    m_NormalColor;
	TEXTURERECT m_trNormal;
	TEXTURERECT m_trPushed;
	TEXTURERECT m_trDisabled;
};

#endif