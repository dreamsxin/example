
// splineDlg.h : 头文件
//

#pragma once

#include "DisplayWnd.h"


// CsplineDlg 对话框
class CsplineDlg : public CDialog
{
// 构造
public:
	CsplineDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SPLINE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
    CDisplayWnd *m_pDispWnd;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedBtnSpline();
    afx_msg void OnDestroy();
};
