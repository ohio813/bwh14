#include "dxctrlmanager.h"
#include "dxctrlbase.h"

__declspec(dllexport) LRESULT CALLBACK D3DWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

PCDXCtrlManager g_Manager;
WNDPROC g_wpOrig = NULL;
HWND hwndD3DWindow = NULL;

CDXCtrlManager::CDXCtrlManager(DWORD dwTriangles, PSZ pszFixedFont, INT iFixedSize, BOOL bFixedBold, PSZ pszProportionalFont, INT iProportionalSize, BOOL bProportionalBold)
{
	g_Manager = this;
	m_bRedraw = FALSE;
    m_dwPrimitiveCount = 0;
	m_dwVBSize = sizeof(VERTEX) * 3 * dwTriangles;
	m_iFixedWidthFontSize = iFixedSize;
	m_iProportionalFontSize = iProportionalSize;
	m_bFixedBold = bFixedBold;
	m_bProportionalBold = bProportionalBold;
	m_pAtlas = NULL;
	m_pD3DDevice = NULL;
	m_pFixedWidthFont = NULL;
	m_pFocus = NULL;
	m_pFontSprite = NULL;
	m_pProportionalFont = NULL;
	m_pszFixedWidthFont = pszFixedFont;
	m_pszProportionalFont = pszProportionalFont;
	m_pVB = NULL;
	m_pControl = NULL;
}

CDXCtrlManager::~CDXCtrlManager()
{
	FreeD3DWindow();
	FreeResources();
	if(m_pControl)
		delete m_pControl;
}

VOID CDXCtrlManager::AddControl(PVOID pControl)
{
	if(m_pControl)
		((PCDXCtrlBase)m_pControl)->AddSibling((PCDXCtrlBase)pControl);
	else
		m_pControl = pControl;

	((PCDXCtrlBase)pControl)->SetManager(this);
}

VOID CDXCtrlManager::AddRectToBuffer(PRECT pRect, PTEXTURERECT pTextureRect)
{
	AddTriangleToBuffer(pRect->left, pRect->bottom, pTextureRect->fLeft, pTextureRect->fBottom, pRect->right, pRect->top, pTextureRect->fRight, pTextureRect->fTop, pRect->left, pRect->top, pTextureRect->fLeft, pTextureRect->fTop);
	AddTriangleToBuffer(pRect->left, pRect->bottom, pTextureRect->fLeft, pTextureRect->fBottom, pRect->right, pRect->bottom, pTextureRect->fRight, pTextureRect->fBottom, pRect->right, pRect->top, pTextureRect->fRight, pTextureRect->fTop);
//	AddTriangleToBuffer(pRect->left, pRect->bottom, pTextureRect->fLeft, pTextureRect->fBottom, pRect->left, pRect->top, pTextureRect->fLeft, pTextureRect->fTop, pRect->right, pRect->top, pTextureRect->fRight, pTextureRect->fTop);
//	AddTriangleToBuffer(pRect->left, pRect->bottom, pTextureRect->fLeft, pTextureRect->fBottom, pRect->right, pRect->top, pTextureRect->fRight, pTextureRect->fTop, pRect->right, pRect->bottom, pTextureRect->fRight, pTextureRect->fBottom);
}

VOID CDXCtrlManager::AddTriangleToBuffer(DWORD x1, DWORD y1, FLOAT tu1, FLOAT tv1, DWORD x2, DWORD y2, FLOAT tu2, FLOAT tv2, DWORD x3, DWORD y3, FLOAT tu3, FLOAT tv3)
{
	AddVertexToBuffer(x1, y1, tu1, tv1);
	AddVertexToBuffer(x2, y2, tu2, tv2);
	AddVertexToBuffer(x3, y3, tu3, tv3);
	m_dwPrimitiveCount++;
}

VOID CDXCtrlManager::AddVertexToBuffer(DWORD x, DWORD y, FLOAT tu, FLOAT tv)
{
	m_pVBMem->x = (FLOAT)(INT)x;
	m_pVBMem->y = (FLOAT)(INT)y;
	m_pVBMem->z = 1;
	m_pVBMem->rhw = 1;
	m_pVBMem->tu = tu;
	m_pVBMem->tv = tv;
	m_pVBMem++;
}

VOID CDXCtrlManager::CreateFonts()
{
	HDC hdc;
	INT iPixPerInch;

	FreeFonts();
	if(m_pD3DDevice)
	{
		hdc = GetDC(hwndD3DWindow);
		iPixPerInch = GetDeviceCaps(hdc, LOGPIXELSY);
		ReleaseDC(hwndD3DWindow, hdc);
		D3DXCreateFont(m_pD3DDevice, ~MulDiv(m_iFixedWidthFontSize, iPixPerInch, 72), 0, (m_bFixedBold ? FW_BOLD : FW_NORMAL), 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, m_pszFixedWidthFont, &m_pFixedWidthFont);
		D3DXCreateFont(m_pD3DDevice, ~MulDiv(m_iProportionalFontSize, iPixPerInch, 72), 0, (m_bProportionalBold ? FW_BOLD : FW_NORMAL), 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, m_pszProportionalFont, &m_pProportionalFont);
	}
}

VOID CDXCtrlManager::CreateFontSprite()
{
	FreeFontSprite();
	if(m_pD3DDevice)
		D3DXCreateSprite(m_pD3DDevice, &m_pFontSprite);
}

VOID CDXCtrlManager::CreateVertexBuffer()
{
	FreeVertexBuffer();
	if(m_pD3DDevice)
	{
		m_pD3DDevice->CreateVertexBuffer(m_dwVBSize, D3DUSAGE_WRITEONLY, FVF, D3DPOOL_MANAGED, &m_pVB, NULL);
		m_bRedraw = TRUE;
	}
}

PVOID CDXCtrlManager::FindControl(DWORD dwId)
{
	if(m_pControl)
		return ((PCDXCtrlBase)m_pControl)->FindControl(dwId);
	else
		return NULL;
}

VOID CDXCtrlManager::FreeD3DWindow()
{
	if(g_wpOrig && IsWindow(hwndD3DWindow))
		SetWindowLong(hwndD3DWindow, GWL_WNDPROC, PtrToLong(g_wpOrig));
	g_wpOrig = NULL;
	hwndD3DWindow = NULL;
}

VOID CDXCtrlManager::FreeFonts()
{
	if(m_pProportionalFont)
		m_pProportionalFont->Release();
	m_pProportionalFont = NULL;
	
	if(m_pFixedWidthFont)
		m_pFixedWidthFont->Release();
	m_pFixedWidthFont = NULL;
}

VOID CDXCtrlManager::FreeFontSprite()
{
	if(m_pFontSprite)
		m_pFontSprite->Release();
	m_pFontSprite = NULL;
}

VOID CDXCtrlManager::FreeResources()
{
	if(m_pD3DDevice)
	{
		FreeFonts();
		FreeFontSprite();
		FreeTextureAtlas();
		FreeVertexBuffer();
		m_pD3DDevice = NULL;
	}
}

VOID CDXCtrlManager::CreateTextureAtlas()
{
	FreeTextureAtlas();
	if(m_pD3DDevice)
        D3DXCreateTextureFromFileInMemory(m_pD3DDevice, m_pvAtlasFile, m_dwAtlasSize, &m_pAtlas);
}

VOID CDXCtrlManager::FreeTextureAtlas()
{
	if(m_pAtlas)
		m_pAtlas->Release();
	m_pAtlas = NULL;
}

VOID CDXCtrlManager::FreeVertexBuffer()
{
	if(m_pVB)
		m_pVB->Release();
	m_pVB = NULL;
}

PVOID CDXCtrlManager::GetControl()
{
	return m_pControl;
}

PDIRECT3DDEVICE9 CDXCtrlManager::GetD3DDevice()
{
	return m_pD3DDevice;
}

LPD3DXFONT CDXCtrlManager::GetFixedWidthFont()
{
	return m_pFixedWidthFont;
}

PVOID CDXCtrlManager::GetFocusedControl()
{
	return m_pFocus;
}

LPD3DXSPRITE CDXCtrlManager::GetFontSprite()
{
	return m_pFontSprite;
}

LPD3DXFONT CDXCtrlManager::GetProportionalFont()
{
	return m_pProportionalFont;
}

VOID CDXCtrlManager::Redraw()
{
	m_bRedraw = TRUE;
}

VOID CDXCtrlManager::Render()
{
	if(m_bRedraw && m_pControl)
	{
		m_dwPrimitiveCount = 0;
		m_pVB->Lock(0, 0, (PVOID*)&m_pVBMem, D3DLOCK_DISCARD);

		((PCDXCtrlBase)m_pControl)->SetupVertices();

		m_pVB->Unlock();
		m_bRedraw = FALSE;
	}
	
	SetupDevice();

	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_dwPrimitiveCount);
	
	m_pFontSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
	
	if(m_pControl)
		((PCDXCtrlBase)m_pControl)->SetupText();
	
	m_pFontSprite->End();

	RestoreDevice();	
}

VOID CDXCtrlManager::SetD3DDevice(PDIRECT3DDEVICE9 pD3DDevice)
{
	m_pD3DDevice = pD3DDevice;
	CreateFontSprite();
	CreateVertexBuffer();
	CreateFonts();
	CreateTextureAtlas();
}

VOID CDXCtrlManager::SetD3DWindow(HWND hwnd)
{
	FreeD3DWindow();
	hwndD3DWindow = hwnd;
	g_wpOrig = (WNDPROC)LongToPtr(SetWindowLong(hwnd, GWL_WNDPROC, PtrToLong(D3DWindowProc)));
}

VOID CDXCtrlManager::SetFixedWidthFont(PSZ pszFont, INT iSize)
{
	m_pszFixedWidthFont = pszFont;
	m_iFixedWidthFontSize = iSize;
	CreateFonts();
}

VOID CDXCtrlManager::SetFocusedControl(PVOID pControl)
{
	if(m_pFocus)
		((PCDXCtrlBase)m_pFocus)->FocusLost();
	m_pFocus = pControl;
}

VOID CDXCtrlManager::SetProportionalFont(PSZ pszFont, INT iSize)
{
	m_pszProportionalFont = pszFont;
	m_iProportionalFontSize = iSize;
	CreateFonts();
}

VOID CDXCtrlManager::SetTextureAtlas(PVOID pvTexture, UINT uiTextureSize)
{
	m_pvAtlasFile = pvTexture;
	m_dwAtlasSize = uiTextureSize;
}

VOID CDXCtrlManager::SetupDevice()
{

	D3DVIEWPORT9 vp;
	PDIRECT3DSURFACE9 pSurface;
	D3DSURFACE_DESC descSurface;

	m_pD3DDevice->BeginScene();

	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);

	m_pD3DDevice->GetViewport(&m_ViewPortBackup);
	m_pD3DDevice->GetRenderTarget(0, &pSurface);
	pSurface->GetDesc(&descSurface);
	pSurface->Release();
	
	vp.X = 0;
	vp.Y = 0;
	vp.MaxZ = 1;
	vp.MinZ = 0;
	vp.Width = descSurface.Width;
	vp.Height = descSurface.Height;
	m_pD3DDevice->SetViewport(&vp);

	m_pD3DDevice->GetTexture(0, &m_pTextureBackup);
	m_pD3DDevice->SetTexture(0, m_pAtlas);

	m_pD3DDevice->GetFVF(&m_dwFVFBackup);
	m_pD3DDevice->SetFVF(FVF);

	m_pD3DDevice->GetStreamSource(0, &m_pVBBackup, &m_uiStreamOffsetBackup, &m_uiStreamStrideBackup);
	m_pD3DDevice->SetStreamSource(0, m_pVB, 0, sizeof(VERTEX));	
}

VOID CDXCtrlManager::RestoreDevice()
{
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_pD3DDevice->SetStreamSource(0, m_pVBBackup, m_uiStreamOffsetBackup, m_uiStreamStrideBackup);
	m_pVBBackup->Release();
	m_pD3DDevice->SetFVF(m_dwFVFBackup);
	m_pD3DDevice->SetTexture(0, m_pTextureBackup);
	m_pTextureBackup->Release();
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetViewport(&m_ViewPortBackup);
	m_pD3DDevice->EndScene();
}

__declspec(dllexport) LRESULT CALLBACK D3DWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(((PCDXCtrlBase)g_Manager->GetControl())->WindowProc(hwnd, uMsg, wParam, lParam))
		return TRUE;

	return CallWindowProc(g_wpOrig, hwnd, uMsg, wParam, lParam);
}