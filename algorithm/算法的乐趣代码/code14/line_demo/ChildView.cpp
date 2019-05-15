
// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "line_demo.h"
#include "ChildView.h"
#include "line.h"
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
    DDA_Line(-200, 120, 200, -120);
    DDA_Line(-200, -120, 200, 120);
    DDA_Line(120, -250, -120, 250);
    DDA_Line(-120, -250, 120, 250);
    DDA_Line(160, -40, -160, 40);
    DDA_Line(160, 40, -160, -40);
    DDA_Line(40, 160, -40, -160);
    DDA_Line(-40, 160, 40, -160);


    Bresenham_Line(-200, 120, 200, -120);
    Bresenham_Line(-200, -120, 200, 120);
    Bresenham_Line(120, -250, -120, 250);
    Bresenham_Line(-120, -250, 120, 250);
    Bresenham_Line(160, -40, -160, 40);
    Bresenham_Line(160, 40, -160, -40);
    Bresenham_Line(40, 160, -40, -160);
    Bresenham_Line(-40, 160, 40, -160);


    Bresenham_Line2(-200, 120, 200, -120);
    Bresenham_Line2(-200, -120, 200, 120);
    Bresenham_Line2(120, -250, -120, 250);
    Bresenham_Line2(-120, -250, 120, 250);
    Bresenham_Line2(160, -40, -160, 40);
    Bresenham_Line2(160, 40, -160, -40);
    Bresenham_Line2(40, 160, -40, -160);
    Bresenham_Line2(-40, 160, 40, -160);

    Sym_Bresenham_Line(-200, 120, 200, -120);
    Sym_Bresenham_Line(-200, -120, 200, 120);
    Sym_Bresenham_Line(120, -250, -120, 250);
    Sym_Bresenham_Line(-120, -250, 120, 250);
    Sym_Bresenham_Line(160, -40, -160, 40);
    Sym_Bresenham_Line(160, 40, -160, -40);
    Sym_Bresenham_Line(40, 160, -40, -160);
    Sym_Bresenham_Line(-40, 160, 40, -160);
*/

    //Double_Step_Line(-200, 120, 200, -120);
    Double_Step_Line(-200, -120, 200, 120);

	// Do not call CWnd::OnPaint() for painting messages
}

