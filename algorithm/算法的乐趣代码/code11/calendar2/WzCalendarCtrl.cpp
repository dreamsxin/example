// WzCalendarCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "calendar2.h"
#include "WzCalendarCtrl.h"
#include "calendar_const.h"
#include "calendar_func.h"
#include "Names.h"
#include "MemDC.h"


// CWzCalendarCtrl

IMPLEMENT_DYNAMIC(CWzCalendarCtrl, CStatic)

CWzCalendarCtrl::CWzCalendarCtrl()
{
    InitDefaultColor();
    InitDefaultFonts();
}

CWzCalendarCtrl::~CWzCalendarCtrl()
{
    ReleaseDefaultFonts();
}

BOOL CWzCalendarCtrl::SetGeriYear(int year)
{
    if(m_calendar.SetGeriYear(year))
    {
        SetCurrentMonth(1);

        return TRUE;
    }

    return FALSE;
}

void CWzCalendarCtrl::SetCurrentMonth(int month)
{
    if(!m_calendar.IsCalendarReady())
        return;

    if((month >= 1) && (month <= MONTHES_FOR_YEAR))
    {
        m_curMonth = month;
        UpdateCalendarCtrl();
    }
}

void CWzCalendarCtrl::NextMonth()
{
    if(!m_calendar.IsCalendarReady())
        return;

    if(m_curMonth < 12)
    {
        m_curMonth++;
        UpdateCalendarCtrl();
    }
}

void CWzCalendarCtrl::LastMonth()
{
    if(!m_calendar.IsCalendarReady())
        return;

    if(m_curMonth > 1)
    {
        m_curMonth--;
        UpdateCalendarCtrl();
    }
}

void CWzCalendarCtrl::InitDefaultColor()
{
    m_clrDateText = RGB(0, 0, 255);
    m_clrSolarTermsText = RGB(128, 128, 0);
    m_clrChnDateText = RGB(0, 64, 128);
    m_clrBodyBkgnd = RGB(64, 255, 200);
    m_clrYearBannerBkgnd = RGB(64, 128, 255);
    m_clrYearBannerText = RGB(0, 0, 255);
    m_clrWeekBannerBkgnd = RGB(200, 255, 64);
    m_clrWeekBannerText = RGB(64, 64, 128);
}

void CWzCalendarCtrl::InitDefaultFonts()
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

void CWzCalendarCtrl::ReleaseDefaultFonts()
{
    m_DateFont.DeleteObject();
}

int CWzCalendarCtrl::CalculateBodyRow(const CMonthInfo& mi)
{
    int days = mi.GetDaysCount();
    int firstRowDays = DAYS_FOR_WEEK - mi.GetFirstDayWeek();

    int rows = 1 + (days - firstRowDays) / DAYS_FOR_WEEK;
    if(((days - firstRowDays) % DAYS_FOR_WEEK) != 0)
    {
        rows++;
    }

    return rows;
}

void CWzCalendarCtrl::UpdateCalendarCtrl()
{
    if(GetSafeHwnd() != NULL)
    {
        RedrawWindow();
    }
}

void CWzCalendarCtrl::MakeYearBannerText(CString& text)
{
    int stems,branchs,sbMonth,sbDay;

    int year = m_calendar.GetCurrentYear();
    m_calendar.GetSpringBeginsDay(&sbMonth, &sbDay);
    //过了立春，才是新的农历年开始
    CalculateStemsBranches((m_curMonth >= sbMonth) ? year : year - 1, &stems, &branchs);

    text.Format(_T("%4d年%02d月, 农历【%s%s】%s年"), 
                year, m_curMonth, nameOfStems[stems - 1], nameOfBranches[branchs - 1], nameOfShengXiao[branchs - 1]);
}


BEGIN_MESSAGE_MAP(CWzCalendarCtrl, CStatic)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CWzCalendarCtrl::DrawYearBanner(CDC *pDC, const CRect& rcBanner)
{
    CString strYear;
    MakeYearBannerText(strYear);
    pDC->FillSolidRect(&rcBanner, m_clrYearBannerBkgnd);
    pDC->SetTextColor(m_clrYearBannerText);
    pDC->SetBkMode(TRANSPARENT);
    //CFont *pof = (CFont *)pDC->SelectStockObject(SYSTEM_FONT);
    pDC->DrawText(strYear, const_cast<CRect *>(&rcBanner), DT_SINGLELINE|DT_CENTER|DT_VCENTER);
    //pDC->SelectObject(pof);
}

void CWzCalendarCtrl::DrawWeekBanner(CDC *pDC, const CRect& rcBanner)
{
    int weekWidth = rcBanner.Width() / DAYS_FOR_WEEK;
    int rem = rcBanner.Width() % DAYS_FOR_WEEK;
    weekWidth -= 2;
    CRect rcWeek = rcBanner;
    rcWeek.left += (rem / 2);

    pDC->FillSolidRect(&rcBanner, m_clrBodyBkgnd);

    for(int i = 0; i < DAYS_FOR_WEEK; i++)
    {
        rcWeek.left++;
        rcWeek.right = rcWeek.left + weekWidth;
        
        pDC->FillSolidRect(&rcWeek, m_clrWeekBannerBkgnd);
        pDC->SetTextColor(m_clrWeekBannerText);
        pDC->SetBkMode(TRANSPARENT);
        CString strWeek = nameOfWeek[i];
        pDC->DrawText(strWeek, rcWeek, DT_SINGLELINE|DT_CENTER|DT_VCENTER);
        
        rcWeek.left = rcWeek.right + 1;
    }
}

void CWzCalendarCtrl::DrawCalendarHeader(CDC *pDC, const CRect& rcHeader)
{
    CRect rcYearBanner = rcHeader;
    CRect rcWeekBanner = rcHeader;

    rcYearBanner.bottom = rcYearBanner.top + CALENDAR_YEARBANNER_HEIGHT;
    DrawYearBanner(pDC, rcYearBanner);

    rcWeekBanner.top = rcWeekBanner.bottom - CALENDAR_WEEKBANNER_HEIGHT;
    DrawWeekBanner(pDC, rcWeekBanner);
}

void CWzCalendarCtrl::DrawCellDateInfo(CDC *pDC, const CRect& rcDate, const CDayInfo& di)
{
    CString strText;
    strText.Format(_T("%d"), di.GetDayDate());
    bool weekend = ((di.GetDayWeek() % DAYS_FOR_WEEK) == 0) || ((di.GetDayWeek() % DAYS_FOR_WEEK) == 6);
    pDC->SetTextColor(weekend ? RGB(255, 0, 0) : m_clrDateText);
    pDC->SetBkMode(TRANSPARENT);

    if(m_DateFont.GetSafeHandle() != NULL)
    {
        CFont *pOf = pDC->SelectObject(&m_DateFont);
        pDC->DrawText(strText, const_cast<CRect *>(&rcDate), DT_SINGLELINE|DT_CENTER|DT_VCENTER);
        pDC->SelectObject(pOf);
    }
    else
    {
        pDC->DrawText(strText, const_cast<CRect *>(&rcDate), DT_SINGLELINE|DT_CENTER|DT_VCENTER);
    }
}

void CWzCalendarCtrl::DrawCellChnInfo(CDC *pDC, const CRect& rcDate, const CDayInfo& di)
{
    CString strText;

    CChnMonthInfo& cmi = m_calendar.GetChnMonthInfo(di.GetDayChnMonth());
    if(di.GetDayChnDate() == 0) /*每月的第一天？*/
    {
        if(cmi.GetMonthName() == 1)
        {
            strText += _T("春节"); 
        }
        else
        {
            if(cmi.IsLeapMonth())
                strText += _T("闰"); 

            strText += nameOfChnMonth[cmi.GetMonthName() - 1];
            strText += (cmi.GetMonthDays() == CHINESE_L_MONTH_DAYS) ? _T("月大") : _T("月小");
        }
        pDC->SetTextColor(RGB(255, 0, 0));
    }
    else
    {
        if(di.GetDaySolarTerm() != -1)
        {
            pDC->SetTextColor(m_clrSolarTermsText);
            strText += nameOfJieQi[di.GetDaySolarTerm()];
        }
        else
        {
            pDC->SetTextColor(m_clrChnDateText);
            strText += nameOfChnDay[di.GetDayChnDate()];
        }
    }

    pDC->SetBkMode(TRANSPARENT);
    if(m_ChnDateFont.GetSafeHandle() != NULL)
    {
        CFont *pOf = pDC->SelectObject(&m_ChnDateFont);
        pDC->DrawText(strText, const_cast<CRect *>(&rcDate), DT_CENTER|DT_VCENTER);
        pDC->SelectObject(pOf);
    }
    else
    {
        pDC->DrawText(strText, const_cast<CRect *>(&rcDate), DT_CENTER|DT_VCENTER);
    }
}

void CWzCalendarCtrl::DrawBodyCell(CDC *pDC, const CRect& rcCell, const CDayInfo& di)
{
    CRect rcDate = rcCell;
    rcDate.bottom = rcDate.top + CALENDAR_NUMBER_HEIGHT;
    DrawCellDateInfo(pDC, rcDate, di);

    CRect rcChnDate = rcCell;
    rcChnDate.top = rcCell.top + CALENDAR_NUMBER_HEIGHT;
    DrawCellChnInfo(pDC, rcChnDate, di);
}

void CWzCalendarCtrl::DrawAllBodyCells(CDC *pDC, const CRect& rcBody, const CMonthInfo& mi)
{
    int cellHeight = rcBody.Height() / CalculateBodyRow(mi);
    int cellWidth = rcBody.Width() / DAYS_FOR_WEEK;
    int leftSpace = (rcBody.Width() % DAYS_FOR_WEEK) / 2;
    
    CRect rcCell;
    rcCell.top = rcBody.top;
    rcCell.bottom = rcCell.top + cellHeight;
    for(int i = 1; i <= mi.GetDaysCount(); i++)
    {
        const CDayInfo& dayInfo = mi.GetDayInfo(i);
        rcCell.left = rcBody.left + leftSpace + (cellWidth * dayInfo.GetDayWeek());
        rcCell.right = rcCell.left + cellWidth - 1;
        DrawBodyCell(pDC, rcCell, dayInfo);
        if(dayInfo.GetDayWeek() == 6)
        {
            rcCell.top = rcCell.bottom + 1;
            rcCell.bottom = rcCell.top + cellHeight;
        }
    }
}

void CWzCalendarCtrl::DrawCalendarBody(CDC *pDC, const CRect& rcBody)
{
    pDC->FillSolidRect(&rcBody, m_clrBodyBkgnd);

    if(m_calendar.IsCalendarReady())
    {
        CMonthInfo& mi = m_calendar.GetMonthInfo(m_curMonth);
        DrawAllBodyCells(pDC, rcBody, mi);
    }
}

void CWzCalendarCtrl::DrawCalendar(CDC *pDC, const CRect& rcCalendar)
{
    CRect rcHeader = rcCalendar;
    CRect rcBody = rcCalendar;
    
    rcHeader.bottom = rcHeader.top + CALENDAR_HEADER_HEIGHT;
    DrawCalendarHeader(pDC, rcHeader);
    
    rcBody.top += CALENDAR_HEADER_HEIGHT;
    DrawCalendarBody(pDC, rcBody);
}


// CWzCalendarCtrl message handlers
void CWzCalendarCtrl::OnPaint()
{
    CPaintDC dc(this); // device context for painting
	
    CRect rcClient;
	GetClientRect(rcClient);

	CMemoryDC memDC(&dc,&rcClient);
    DrawCalendar(&memDC, rcClient);
}

BOOL CWzCalendarCtrl::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
    //return CStatic::OnEraseBkgnd(pDC);
}

void CWzCalendarCtrl::PreSubclassWindow()
{
    DWORD dwStyle = GetStyle();
    SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);

    CStatic::PreSubclassWindow();
}

BOOL CWzCalendarCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Add your specialized code here and/or call the base class

    return CStatic::PreCreateWindow(cs);
}
