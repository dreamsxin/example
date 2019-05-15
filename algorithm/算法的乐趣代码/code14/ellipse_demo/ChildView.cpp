
// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "ellipse_demo.h"
#include "ChildView.h"
#include "ellipse.h"
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
    MP_Ellipse(0, 0, 100, 50);
    MP_Ellipse(-100, 100, 200, 100);
    MP_Ellipse(-100, -100, 200, 100);
    MP_Ellipse(100, -100, 200, 100);
    MP_Ellipse(100, 100, 200, 100);

*/    

    Bresenham_Ellipse(0, 0, 200, 100);
    Bresenham_Ellipse(-100, 100, 200, 100);
    Bresenham_Ellipse(-100, -100, 200, 100);
    Bresenham_Ellipse(100, -100, 200, 100);
    Bresenham_Ellipse(100, 100, 200, 100);


}

