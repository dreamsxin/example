// DisplayWnd.cpp : implementation file
//

#include "stdafx.h"
#include "spline.h"
#include "DisplayWnd.h"
#include "LogicDevice.h"
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDisplayWnd

CDisplayWnd::CDisplayWnd()
{
}

CDisplayWnd::~CDisplayWnd()
{
	ResetData();
}

void CDisplayWnd::ShowDisplayWindow(CWnd *pParent)
{
	ASSERT(GetSafeHwnd() == NULL);

	CString strClassName= AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,::LoadCursor(NULL, IDC_ARROW),
                                              (HBRUSH) ::GetStockObject(WHITE_BRUSH),
                                              ::LoadIcon(NULL, (LPCTSTR)IDR_MAINFRAME));

	CreateEx(WS_EX_CLIENTEDGE,strClassName,_T("演示窗口"),WS_OVERLAPPEDWINDOW,CRect(0,0,640,480),pParent,NULL);

	ShowWindow(SW_SHOW);
}

void CDisplayWnd::ResetData()
{
    m_xPpoint.clear();
    m_yPpoint.clear();
    m_xSource.clear();
    m_ySource.clear();
    m_xSpline.clear();
    m_ySpline.clear();
}

void CDisplayWnd::SetInterpolationPoint(double *x, double *y, int count)
{
    m_xPpoint.assign(x, x + count);
    m_yPpoint.assign(y, y + count);
}

void CDisplayWnd::SetSourceCurve(double *x, double *y, int count)
{
    m_xSource.assign(x, x + count);
    m_ySource.assign(y, y + count);
}

void CDisplayWnd::SetSourceSplineCurve(double *x, double *y, int count)
{
    m_xSpline.assign(x, x + count);
    m_ySpline.assign(y, y + count);
}

void CDisplayWnd::SetPointLogicRange(double xmin, double xmax, double ymin, double ymax)
{
    SetXLogicRange(xmin, xmax);
    SetYLogicRange(ymin, ymax);
}

void CDisplayWnd::DrawSpline(CDC *pDC, const std::vector<double>& xArray, 
                             const std::vector<double>& yArray, COLORREF color)
{
    HDC hDC = pDC->GetSafeHdc();

	HPEN hColorPen = ::CreatePen(PS_SOLID, 1, color);
	HPEN ho = (HPEN)::SelectObject(hDC, hColorPen);

    int x0 = DeviceXFromLogicX(xArray[0]);
    int y0 = DeviceYFromLogicY(yArray[0]);
	::MoveToEx(hDC, x0, y0, NULL);
	for(int i = 1; i < (int)xArray.size(); i++)
	{
		int x1 = DeviceXFromLogicX(xArray[i]);
		int y1 = DeviceYFromLogicY(yArray[i]);
		::LineTo(hDC, x1, y1);
		x0 = x1;
		y0 = y1;
	}

	SelectObject(hDC, ho);
}

void CDisplayWnd::DrawSample(CDC *pDC, int x, int y, LPCTSTR text, COLORREF color)
{
    HDC hDC = pDC->GetSafeHdc();
	HPEN hColorPen = ::CreatePen(PS_SOLID, 1, color);
	HPEN ho = (HPEN)::SelectObject(hDC, hColorPen);
	HFONT hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	HFONT of = (HFONT)::SelectObject(hDC, hFont);

	::MoveToEx(hDC, x, y, NULL);
	::LineTo(hDC, x + 30, y);
	COLORREF ocr = ::SetTextColor(hDC, color);
	::TextOut(hDC, x + 40, y - 6, text, lstrlen(text));
	::SetTextColor(hDC, ocr);

    ::SelectObject(hDC, ho);
    ::SelectObject(hDC, of);
}

static LPCTSTR lpszFx = _T("原函数曲线");
static LPCTSTR lpszS = _T("三次样条函数曲线");

void CDisplayWnd::DrawCurve(CDC *pDC)
{
    DrawBackground(pDC, RGB(255,255,255));
    //DrawBackground(pDC, RGB(111,222,211));
    DrawAxis(pDC, RGB(0,0,0));
    //DrawAxis(pDC, RGB(255,255,0));

    HDC hDC = pDC->GetSafeHdc();

	int nMode = ::SetBkMode(hDC,TRANSPARENT);

    DrawSample(pDC, 10, 20, lpszFx, RGB(0,0,255));
    DrawSample(pDC, 10, 50, lpszS, RGB(255,0,0));

    if(m_xPpoint.size() > 0 && m_yPpoint.size() > 0)
    {
        DrawSpline(pDC, m_xPpoint, m_yPpoint, RGB(0,0,0));
    }
    if(m_xSource.size() > 0 && m_ySource.size() > 0)
    {
        DrawSpline(pDC, m_xSource, m_ySource, RGB(0,0,255));
    }
    if(m_xSpline.size() > 0 && m_ySpline.size() > 0)
    {
        DrawSpline(pDC, m_xSpline, m_ySpline, RGB(255,0,0));
    }

	::SetBkMode(hDC,nMode);
}

BEGIN_MESSAGE_MAP(CDisplayWnd, CWnd)
	//{{AFX_MSG_MAP(CDisplayWnd)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDisplayWnd message handlers

BOOL CDisplayWnd::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWnd::DestroyWindow();
}

void CDisplayWnd::PostNcDestroy() 
{
	CWnd::PostNcDestroy();

    delete this;
}

int CDisplayWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

void CDisplayWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CDisplayWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnMouseMove(nFlags, point);
}

void CDisplayWnd::OnPaint() 
{
	CPaintDC dc(this);
	
	CDC memDC;
	CBitmap bitmap;
	memDC.CreateCompatibleDC(&dc);
	bitmap.CreateCompatibleBitmap(&dc, m_rcClient.Width(), m_rcClient.Height());
	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
	
	DrawCurve(&memDC);
	
	dc.BitBlt(m_rcClient.left, m_rcClient.top, m_rcClient.Width(), m_rcClient.Height(), &memDC, 0,0, SRCCOPY);

	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();
	bitmap.DeleteObject();
}

void CDisplayWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	GetClientRect(m_rcClient);
    SetDevicePointRange(m_rcClient.left, m_rcClient.top, m_rcClient.right, m_rcClient.bottom);
	Invalidate(TRUE);
}

BOOL CDisplayWnd::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;

    //return CWnd::OnEraseBkgnd(pDC);
}

void CDisplayWnd::DrawAxis(CDC *pDC, COLORREF color)
{
    int centerX, centerY;

    GetDeviceCenter(centerX, centerY);

	HDC hDC = pDC->GetSafeHdc();

	HPEN hColorPen = ::CreatePen(PS_SOLID, 1, color);
    HPEN ho = (HPEN)::SelectObject(hDC, hColorPen);
    ::MoveToEx(hDC, 0, centerY, (LPPOINT) NULL); 
    ::LineTo(hDC, m_rcClient.Width(), centerY); 
    ::MoveToEx(hDC, centerX, 0, (LPPOINT) NULL); 
    ::LineTo(hDC, centerX, m_rcClient.Height()); 
    SelectObject(hDC, ho);
}

void CDisplayWnd::DrawBackground(CDC *pDC, COLORREF color)
{
	HDC hDC = pDC->GetSafeHdc();
	HBRUSH hBrush = ::CreateSolidBrush(color);
	::FillRect(hDC, m_rcClient, hBrush);
	::DeleteObject(hBrush);
}
