#if !defined(AFX_DISPLAYWND_H__79010946_FB5A_4F34_8FD9_918770E8D034__INCLUDED_)
#define AFX_DISPLAYWND_H__79010946_FB5A_4F34_8FD9_918770E8D034__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DisplayWnd.h : header file
//

#include <vector>
/////////////////////////////////////////////////////////////////////////////
// CDisplayWnd window

class CDisplayWnd : public CWnd
{
// Construction
public:
	CDisplayWnd();

// Attributes
public:

protected:
	CRect m_rcClient;
// Operations
public:
    void ShowDisplayWindow(CWnd *pParent);

	void ResetData();
    void SetInterpolationPoint(double *x, double *y, int count);
    void SetSourceCurve(double *x, double *y, int count);
    void SetSourceSplineCurve(double *x, double *y, int count);
    void SetPointLogicRange(double xmin, double xmax, double ymin, double ymax);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDisplayWnd)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDisplayWnd();
protected:
    std::vector<double> m_xPpoint;
    std::vector<double> m_yPpoint;
    std::vector<double> m_xSource;
    std::vector<double> m_ySource;
    std::vector<double> m_xSpline;
    std::vector<double> m_ySpline;
	// Generated message map functions
protected:
	//{{AFX_MSG(CDisplayWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
private:
	void DrawSample(CDC *pDC, int x, int y, LPCTSTR text, COLORREF color);
	void DrawCurve(CDC *pDC);
	void DrawSpline(CDC *pDC, const std::vector<double>& xArray, 
                    const std::vector<double>& yArray, COLORREF color);
    void DrawAxis(CDC *pDC, COLORREF color);
    void DrawBackground(CDC *pDC, COLORREF color);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISPLAYWND_H__79010946_FB5A_4F34_8FD9_918770E8D034__INCLUDED_)
