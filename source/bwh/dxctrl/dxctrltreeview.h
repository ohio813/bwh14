#ifndef _DXCTRLTREEVIEW_H
#define _DXCTRLTREEVIEW_H

#include "dxctrlwindow.h"

typedef struct _LISTITEM
{
	char szText[101];
	BOOL bGroup;
	BOOL bVisible;
	struct _LISTITEM* link;	
}LISTITEM, *PLISTITEM;

#define PCDXCtrlTreeview CDXCtrlTreeview*
class CDXCtrlTreeview : public CDXCtrlWindow
{
public:
	CDXCtrlTreeview(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, DWORD dwFrameWidth, PTEXTURERECT ptrFrame, PTEXTURERECT ptrBackground, PTEXTURERECT ptrItemHighlight, PTEXTURERECT ptrGroupHighlight, PTEXTURERECT ptrCollapse, PTEXTURERECT ptrExpand, PTEXTURERECT ptrScrollUpUp, PTEXTURERECT ptrScrollUpDown, PTEXTURERECT ptrScrollUpDisabled, PTEXTURERECT ptrScrollDownUp, PTEXTURERECT ptrScrollDownDown, PTEXTURERECT ptrScrollDownDisabled, PTEXTURERECT ptrKnob, BOOL bShowItemIndex);
	~CDXCtrlTreeview();

	VOID  AddItem(PSZ pszText, BOOL bGroup, DWORD dwPos);
	VOID  ClearList();
	VOID  DeleteItem(DWORD dwPos);
	VOID  Draw();
	VOID  DrawText();
	DWORD GetClickedIndex(LONG lX, LONG lY);
	DWORD GetSelected();
	VOID  GetText(PSZ pszText, LONG lBuffSize, DWORD dwIndex);
	VOID  SetText(PSZ pszText, DWORD dwIndex);
	VOID  MoveItem(DWORD dwPos, BOOL bUp);
	BOOL  OnCommand(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL  OnLeftButtonDown(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL  OnMouseMove(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL  OnMouseWheel(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	VOID  RemoveItem(DWORD dwPos);
	VOID  SetIndexStyle(BOOL bIndex);
	VOID  SetMenuStyle(BOOL bMenu);
	VOID  ScrollDown(DWORD dwCount);
	VOID  ScrollUp(DWORD dwCount);
	VOID  ToggleGroup(DWORD dwItemIndex);
	BOOL  ValidateTopIndex();

	PLISTITEM m_pList;
	DWORD m_dwListCount;
	DWORD m_dwTopIndex;
	DWORD m_dwSelectedIndex;
	D3DCOLOR m_GroupColor;
	D3DCOLOR m_ItemColor;
	D3DCOLOR m_SelectedColor;
	TEXTURERECT m_trExpand;
	TEXTURERECT m_trCollapse;
	TEXTURERECT m_trItemHighlight;
	TEXTURERECT m_trGroupHighlight;
	TEXTURERECT m_trKnob;
	BOOL m_bShowIndex;
	BOOL m_bMenuStyle;
	DWORD m_dwLastClickPos;
	DWORD m_dwLastClickTime;
};

#endif