#ifndef _DXCTRLWINDOW_H
#define _DXCTRLWINDOW_H

#include <windows.h>
#include "dxctrlbase.h"

#define PCDXCtrlWindow CDXCtrlWindow*
class CDXCtrlWindow : public CDXCtrlBase
{
public:
	CDXCtrlWindow(DWORD dwId, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, DWORD dwFrameWidth, PTEXTURERECT ptrFrame, PTEXTURERECT ptrBackground);

	VOID  Draw();
	DWORD GetFrameWidth();

	TEXTURERECT m_trBackground;
	TEXTURERECT m_trFrame;
	DWORD       m_dwFrameWidth;
};

#endif