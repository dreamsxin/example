#pragma once

#include "ChineseCalendar.h"

const int CALENDAR_WEEKBANNER_HEIGHT = 18;
const int CALENDAR_YEARBANNER_HEIGHT = 22;
const int CALENDAR_HEADER_HEIGHT = CALENDAR_WEEKBANNER_HEIGHT + CALENDAR_YEARBANNER_HEIGHT;

const int CALENDAR_NUMBER_HEIGHT = 30;

// CWzCalendarCtrl

class CWzCalendarCtrl : public CStatic
{
	DECLARE_DYNAMIC(CWzCalendarCtrl)

public:
	CWzCalendarCtrl();
	virtual ~CWzCalendarCtrl();
    BOOL SetGeriYear(int year);
    void SetCurrentMonth(int month);
    void NextMonth();
    void LastMonth();

protected:
	DECLARE_MESSAGE_MAP()

    virtual void PreSubclassWindow();
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
    void DrawYearBanner(CDC *pDC, const CRect& rcBanner);
    void DrawWeekBanner(CDC *pDC, const CRect& rcBanner);
    void DrawCalendarHeader(CDC *pDC, const CRect& rcHeader);
    void DrawCellDateInfo(CDC *pDC, const CRect& rcDate, const CDayInfo& di);
    void DrawCellChnInfo(CDC *pDC, const CRect& rcDate, const CDayInfo& di);
    void DrawBodyCell(CDC *pDC, const CRect& rcCell, const CDayInfo& di);
    void DrawAllBodyCells(CDC *pDC, const CRect& rcBody, const CMonthInfo& mi);
    void DrawCalendarBody(CDC *pDC, const CRect& rcBody);
    void DrawCalendar(CDC *pDC, const CRect& rcCalendar);
    void InitDefaultColor();
    void InitDefaultFonts();
    void ReleaseDefaultFonts();
    int CalculateBodyRow(const CMonthInfo& mi);
    void UpdateCalendarCtrl();
    void MakeYearBannerText(CString& text);
    void SetSolarTermsTextColor(COLORREF color) { m_clrSolarTermsText = color; };
    void SetDateTextColor(COLORREF color) { m_clrDateText = color; };
    void SetChnDateTextColor(COLORREF color) { m_clrChnDateText = color; };
    void SetBodyBkgndColor(COLORREF color) { m_clrBodyBkgnd = color; };
    void SetYearBannerBkgndColor(COLORREF color) { m_clrYearBannerBkgnd = color; };
    void SetWeekBanneBkgndColor(COLORREF color) { m_clrWeekBannerBkgnd = color; };
    void SetYearBannerTextColor(COLORREF color) { m_clrYearBannerText = color; };
    void SetWeekBanneTextColor(COLORREF color) { m_clrWeekBannerText = color; };
protected:
    int m_curMonth;
    CFont m_DateFont;
    CFont m_ChnDateFont;
    CChineseCalendar m_calendar;
    
    COLORREF m_clrSolarTermsText;
    COLORREF m_clrDateText;
    COLORREF m_clrChnDateText;
    COLORREF m_clrBodyBkgnd;
    COLORREF m_clrYearBannerBkgnd;
    COLORREF m_clrYearBannerText;
    COLORREF m_clrWeekBannerBkgnd;
    COLORREF m_clrWeekBannerText;
};



