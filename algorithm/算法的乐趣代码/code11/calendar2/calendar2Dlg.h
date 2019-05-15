
// calendar2Dlg.h : 头文件
//

#pragma once

#include "WzCalendarCtrl.h"


// Ccalendar2Dlg 对话框
class Ccalendar2Dlg : public CDialog
{
// 构造
public:
	Ccalendar2Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CALENDAR2_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

protected:
// 实现
protected:
	HICON m_hIcon;
    int m_CurrentMonth;
    CWzCalendarCtrl m_CalendarCtrl;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedBtnSetYear();
    afx_msg void OnBnClickedBtnLastMonth();
    afx_msg void OnBnClickedBtnNextMonth();
    afx_msg void OnBnClickedBtnInformation();
    afx_msg void OnBnClickedBtnTest();
    afx_msg void OnBnClickedBtnSolarterm();
    afx_msg void OnBnClickedBtnNewmoon();
};
