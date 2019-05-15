
// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "circle_demo.h"
#include "ChildView.h"
#include "circle.h"
#include "dev_adp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
    CPaintDC dc(this); // device context for painting

    CRect rcClient;

    GetClientRect(&rcClient);

    SetCurrentDevice(dc.GetSafeHdc());
    SetDeviceWidth(rcClient.Width());
    SetDeviceHeight(rcClient.Height());
    DrawAxis();

/*
    MP_Circle(0, 0, 100);
    MP_Circle(-100, 100, 200);
    MP_Circle(-100, -100, 200);
    MP_Circle(100, -100, 200);
    MP_Circle(100, 100, 200);

*/
    Bresenham_Circle(0, 0, 100);
    Bresenham_Circle(-100, 100, 200);
    Bresenham_Circle(-100, -100, 200);
    Bresenham_Circle(100, -100, 200);
    Bresenham_Circle(100, 100, 200);
/*
    Fast_Circle(0, 0, 101);
    Fast_Circle(-100, 100, 201);
    Fast_Circle(-100, -100, 201);
    Fast_Circle(100, -100, 201);
    Fast_Circle(100, 100, 201);

    Pnar_Circle(0, 0, 100);
    Pnar_Circle(-100, 100, 200);
    Pnar_Circle(-100, -100, 200);
    Pnar_Circle(100, -100, 200);
    Pnar_Circle(100, 100, 200);
*/
}

