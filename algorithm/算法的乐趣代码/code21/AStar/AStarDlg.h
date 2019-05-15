
// AStarDlg.h : header file
//

#pragma once

#include "GridWnd.h"

// CAStarDlg dialog
class CAStarDlg : public CDialog
{
// Construction
public:
	CAStarDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ASTAR_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
    CGridWnd m_gridWnd;
    int m_AlgoType;
    BOOL m_UsingWall;
    int m_DistanceType;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnBnClickedBtnStart();
	DECLARE_MESSAGE_MAP()
};
