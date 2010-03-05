#ifndef _DXCTRLMANAGER_H
#define _DXCTRLMANAGER_H

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#define FVF (D3DFVF_XYZRHW | D3DFVF_TEX1)

typedef struct _TEXTURERECT
{
	FLOAT fLeft;
	FLOAT fTop;
	FLOAT fRight;
	FLOAT fBottom;
} TEXTURERECT, *PTEXTURERECT;

typedef struct _VERTEX
{
	FLOAT x, y, z, rhw;
	FLOAT tu, tv;
} VERTEX, *PVERTEX;

#define PCDXCtrlManager CDXCtrlManager*
class CDXCtrlManager
{
public:
	CDXCtrlManager(DWORD dwTriangles, PSZ pszFixedFont, INT iFixedSize, BOOL bFixedBold, PSZ pszProportionalFont, INT iProportionalSize, BOOL bProportionalBold);
	~CDXCtrlManager();

	VOID             AddControl(PVOID pControl);
	VOID             AddRectToBuffer(PRECT pRect, PTEXTURERECT pTextureRect);
	VOID             AddTriangleToBuffer(DWORD x1, DWORD y1, FLOAT tu1, FLOAT tv1, DWORD x2, DWORD y2, FLOAT tu2, FLOAT tv2, DWORD x3, DWORD y3, FLOAT tu3, FLOAT tv3);
	VOID             AddVertexToBuffer(DWORD x, DWORD y, FLOAT tu, FLOAT tv);
	VOID             CreateFonts();
	VOID             CreateFontSprite();
	VOID             CreateVertexBuffer();
	VOID             CreateTextureAtlas();
	PVOID            FindControl(DWORD dwId);
	VOID             FreeD3DWindow();
	VOID             FreeFonts();
	VOID             FreeFontSprite();
	VOID             FreeResources();
	VOID             FreeTextureAtlas();
	VOID             FreeVertexBuffer();
	PVOID            GetControl();
	PDIRECT3DDEVICE9 GetD3DDevice();
	LPD3DXFONT       GetFixedWidthFont();
	PVOID            GetFocusedControl();
	LPD3DXSPRITE     GetFontSprite();
	LPD3DXFONT       GetProportionalFont();
	VOID             Redraw();
	VOID             Render();
	VOID             RestoreDevice();
	VOID             SetD3DDevice(PDIRECT3DDEVICE9 pD3DDevice);
	VOID             SetD3DWindow(HWND hwnd);
	VOID             SetFixedWidthFont(PSZ pszFont, INT iSize);
	VOID             SetFocusedControl(PVOID pControl);
	VOID             SetProportionalFont(PSZ pszFont, INT iSize);
	VOID             SetTextureAtlas(PVOID pvTexture, UINT uiTextureSize);
	VOID             SetupDevice();

	BOOL                   m_bFixedBold;
	BOOL                   m_bProportionalBold;
	BOOL                   m_bRedraw;
	DWORD                  m_dwPrimitiveCount;
	DWORD                  m_dwVBSize;
	INT                    m_iFixedWidthFontSize;
	INT                    m_iProportionalFontSize;
	PDIRECT3DTEXTURE9      m_pAtlas;
	PVOID                  m_pControl;
	PDIRECT3DDEVICE9       m_pD3DDevice;
	LPD3DXFONT             m_pFixedWidthFont;
	LPD3DXSPRITE           m_pFontSprite;
	LPD3DXFONT             m_pProportionalFont;
	PSZ                    m_pszFixedWidthFont;
	PSZ                    m_pszProportionalFont;
	PDIRECT3DVERTEXBUFFER9 m_pVB;
	PVERTEX                m_pVBMem;
	PVOID                  m_pFocus;
	PVOID                  m_pvAtlasFile;
	DWORD                  m_dwAtlasSize;

	PDIRECT3DBASETEXTURE9  m_pTextureBackup;
	DWORD                  m_dwFVFBackup;
	PDIRECT3DVERTEXBUFFER9 m_pVBBackup;
	UINT                   m_uiStreamOffsetBackup;
	UINT                   m_uiStreamStrideBackup;
	D3DVIEWPORT9           m_ViewPortBackup;
};

#endif