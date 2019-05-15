// GridWnd.cpp : implementation file
//

#include "stdafx.h"
#include "GridWnd.h"
#include "MemDC.h"


// CGridWnd

IMPLEMENT_DYNAMIC(CGridWnd, CStatic)

CGridWnd::CGridWnd()
{
    m_InitCell = FALSE;
    ::ZeroMemory(&m_Cells, sizeof(GRID_CELL));
    InitDefaultColor();
    InitDefaultFonts();
}

CGridWnd::~CGridWnd()
{
    ReleaseDefaultFonts();
}

void CGridWnd::SetCellsInfo(GRID_CELL *gc)
{
    for(int i = 0; i < N_SCALE; i++)
    {
        for(int j = 0; j < N_SCALE; j++)
        {
            m_Cells.cell[i][j] = gc->cell[i][j];
        }
    }
    m_InitCell = TRUE;
}

void CGridWnd::UpdateGridWnd()
{
    if(GetSafeHwnd() != NULL)
    {
        RedrawWindow();
    }
}

void CGridWnd::InitDefaultColor()
{
    m_clrBkgnd = RGB(192, 192, 192);
    m_clrBorder = RGB(64, 64, 128);
    m_clrCellNormal = m_clrBkgnd;
    m_clrCellMark = RGB(200, 255, 64);
    m_clrCellWall = RGB(0, 64, 128);
    m_clrCellSource = RGB(0, 0, 255);
    m_clrCellTarget = RGB(200, 255, 64);
    m_clrCellProcessed = RGB(64, 128, 255);
}

void CGridWnd::InitDefaultFonts()
{
    LOGFONT lf;
    memset(&lf, 0, sizeof(LOGFONT)); 
    lf.lfHeight = 32; 
    lf.lfWeight = 700;
    _tcscpy_s(lf.lfFaceName, LF_FACESIZE, _T("Arial")); 
    m_DateFont.CreateFontIndirect(&lf);

    memset(&lf, 0, sizeof(LOGFONT)); 
    lf.lfHeight = 10; 
    lf.lfWeight = 400;
    _tcscpy_s(lf.lfFaceName, LF_FACESIZE, _T("System")); 
    m_ChnDateFont.CreateFontIndirect(&lf);
}

void CGridWnd::ReleaseDefaultFonts()
{
    m_DateFont.DeleteObject();
}

COLORREF CGridWnd::GetCellColor(CELL *cell)
{
    if(cell->type == 1)
        return m_clrCellMark;
    else if(cell->type == 2)
        return m_clrCellWall;
    else if(cell->type == 3)
        return m_clrCellSource;
    else if(cell->type == 4)
        return m_clrCellTarget;
    else
    {
        if(cell->processed)
            return m_clrCellProcessed;
        else
            return m_clrCellNormal;
    }
}

BEGIN_MESSAGE_MAP(CGridWnd, CStatic)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CGridWnd::DrawGridCtrl(CDC *pDC, const CRect& rcClient)
{
    CRect rcGrid = rcClient;
    rcGrid.DeflateRect(1, 1, 0, 0);
    DrawFrameBorder(pDC, rcGrid);
    rcGrid.DeflateRect(BORDER_WIDTH - 1, BORDER_WIDTH - 1, BORDER_WIDTH, BORDER_WIDTH);
    if(!m_InitCell)
    {
        DrawEmptyGrid(pDC, rcGrid);
    }
    else
    {
        DrawCellsGrid(pDC, rcGrid);
    }
}

void CGridWnd::DrawEmptyGrid(CDC *pDC, const CRect& rcGrid)
{
    pDC->FillSolidRect(rcGrid, m_clrBkgnd);
}

void CGridWnd::DrawCellsGrid(CDC *pDC, const CRect& rcGrid)
{
    int cellwidth = rcGrid.Width() / N_SCALE;
    int cellHeight = rcGrid.Height() / N_SCALE;
    int lr = (rcGrid.Width() - cellwidth * N_SCALE) / 2;
    int tr = (rcGrid.Height() - cellHeight * N_SCALE) / 2;

    for(int i = 0; i < N_SCALE; i++)
    {
        int rs = BORDER_WIDTH + tr + cellHeight * i;
        int cs = BORDER_WIDTH + lr;
        for(int j = 0; j < N_SCALE; j++)
        {
            CRect rcCell(cs, rs, cs + cellwidth, rs + cellHeight);
            DrawCell(pDC, &m_Cells.cell[i][j], rcCell);
            cs += cellwidth;
        }
    }
}

void CGridWnd::DrawFrameBorder(CDC *pDC, const CRect& rcClient)
{
    CPen pen(PS_SOLID, BORDER_WIDTH, m_clrBorder);
    CPen *oldPen = pDC->SelectObject(&pen);
    pDC->SelectStockObject(NULL_BRUSH);
    pDC->Rectangle(rcClient);
    pDC->SelectObject(oldPen);
}

void CGridWnd::DrawCell(CDC *pDC, CELL *cell, const CRect& rcCell)
{
    DrawCellBorder(pDC, rcCell);
    CRect rcInner = rcCell;
    rcInner.DeflateRect(1, 1, 1, 1);
    COLORREF color = GetCellColor(cell);
    pDC->FillSolidRect(rcInner, color);
    if(cell->inPath)
    {
        CBrush brush(RGB(255,128,0));
        CBrush *old = pDC->SelectObject(&brush);
        int cx = (rcCell.left + rcCell.right) / 2;
        int cy = (rcCell.top + rcCell.bottom) / 2;
        pDC->Ellipse(cx - 5, cy - 5, cx + 5, cy + 5);
        pDC->SelectObject(old);
    }
}

void CGridWnd::DrawCellBorder(CDC *pDC, const CRect& rcCell)
{
    CPen pen(PS_SOLID, 1, m_clrBorder);
    CPen *oldPen = pDC->SelectObject(&pen);
    pDC->SelectStockObject(NULL_BRUSH);
    pDC->Rectangle(rcCell);
    pDC->SelectObject(oldPen);
}

// CGridWnd message handlers
void CGridWnd::OnPaint()
{
    CPaintDC dc(this); // device context for painting
	
    CRect rcClient;
	GetClientRect(rcClient);

	CMemoryDC memDC(&dc,&rcClient);
    DrawGridCtrl(&memDC, rcClient);
}

BOOL CGridWnd::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
    //return CStatic::OnEraseBkgnd(pDC);
}

void CGridWnd::PreSubclassWindow()
{
    DWORD dwStyle = GetStyle();
    SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);

    CStatic::PreSubclassWindow();
}

BOOL CGridWnd::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Add your specialized code here and/or call the base class

    return CStatic::PreCreateWindow(cs);
}
