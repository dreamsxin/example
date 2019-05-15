#pragma once

#include "CellDef.h"

// CGridWnd
const int BORDER_WIDTH = 2;

class CGridWnd : public CStatic
{
	DECLARE_DYNAMIC(CGridWnd)

public:
	CGridWnd();
	virtual ~CGridWnd();

    void SetCellsInfo(GRID_CELL *gc);
    void UpdateGridWnd();
    void SetBkgndColor(COLORREF color) { m_clrBkgnd = color; };
    void SetBorderColor(COLORREF color) { m_clrBorder = color; };
    void SetCellNormalColor(COLORREF color) { m_clrCellNormal = color; };
    void SetCellWallColor(COLORREF color) { m_clrCellWall = color; };
    void SetCellSourceColor(COLORREF color) { m_clrCellSource = color; };
    void SetCellTargetColor(COLORREF color) { m_clrCellTarget = color; };
    void SetCellMarkColor(COLORREF color) { m_clrCellMark = color; };

protected:
	DECLARE_MESSAGE_MAP()

    virtual void PreSubclassWindow();
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
    void DrawGridCtrl(CDC *pDC, const CRect& rcClient);
    void DrawEmptyGrid(CDC *pDC, const CRect& rcGrid);
    void DrawCellsGrid(CDC *pDC, const CRect& rcGrid);
    void DrawFrameBorder(CDC *pDC, const CRect& rcClient);
    void DrawCell(CDC *pDC, CELL *cell, const CRect& rcCell);
    void DrawCellBorder(CDC *pDC, const CRect& rcCell);
    void InitDefaultColor();
    void InitDefaultFonts();
    void ReleaseDefaultFonts();
    COLORREF GetCellColor(CELL *cell);
protected:
    GRID_CELL m_Cells;
    BOOL m_InitCell;
    CFont m_DateFont;
    CFont m_ChnDateFont;
    
    COLORREF m_clrBkgnd;
    COLORREF m_clrBorder;
    COLORREF m_clrCellNormal;
    COLORREF m_clrCellMark;
    COLORREF m_clrCellWall;
    COLORREF m_clrCellSource;
    COLORREF m_clrCellTarget;
    COLORREF m_clrCellProcessed;
};



