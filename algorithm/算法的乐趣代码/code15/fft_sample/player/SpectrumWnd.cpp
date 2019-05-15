// SpectrumWnd.cpp : implementation file
//

#include "stdafx.h"
#include "player.h"
#include "SpectrumWnd.h"


#define UPDATE_TIMER_ID  0x1000

// CSpectrumWnd

IMPLEMENT_DYNAMIC(CSpectrumWnd, CWnd)

CSpectrumWnd::CSpectrumWnd()
{
    m_minLevel = 0;
    m_maxLevel = 128;
    m_hBmpView = NULL; 
    m_newLevelReady = FALSE;
}

CSpectrumWnd::~CSpectrumWnd()
{
}

void CSpectrumWnd::SetLevelLimit(int minl, int maxl)
{
    m_minLevel = minl;
    m_maxLevel = maxl;
}

BOOL CSpectrumWnd::SetBandLevel(int *level, int bands)
{
    if(BAND_COUNT != bands)
    {
        return FALSE;
    }
    m_newLevelReady = FALSE;
    for(int i = 0; i < BAND_COUNT; i++)
    {
        if(level[i] < m_minLevel)
            m_newLevel[i] = m_minLevel;
        else if(level[i] > m_maxLevel)
            m_newLevel[i] = m_maxLevel;
        else
            m_newLevel[i] = level[i];
    }

    m_newLevelReady = TRUE;

    return TRUE;
}

BOOL CSpectrumWnd::CreateSpectrumWnd(CWnd* pParentWnd, const CRect& rect)
{
    for(int i = 0; i < BAND_COUNT; i++)
    {
        m_curLevel[i] = m_minLevel;
        m_topBar[i].level = m_minLevel;
        m_topBar[i].wait = TOP_BAR_WAIT;
        m_topBar[i].step = TOP_BAR_STEP;
    }

    int minWidth = BAND_BMP_WIDTH * BAND_COUNT + BAND_BMP_SPACE * (BAND_COUNT - 1) + BAND_WND_BORDER * 2;
    if(rect.Width() < minWidth)
    {
        return FALSE;
    }
    int lineBytes = (rect.Width() * 3 + 3) / 4 * 4;
    m_hBmpView = ::GlobalAlloc(GHND, sizeof(BITMAP_INFO) + lineBytes * rect.Height());
    if(m_hBmpView == NULL)
    {
        return FALSE;
    }
    if(!InitBitmapHandle(m_hBmpView, rect))
    {
        ::GlobalFree(m_hBmpView);
        m_hBmpView = NULL;
        return FALSE;
    }

    int rw = (rect.Width() - minWidth) / BAND_COUNT;
    int rm = (rect.Width() - minWidth) % BAND_COUNT;
    m_barWidth = BAND_BMP_WIDTH + rw;
    m_leftRightSpace = BAND_WND_BORDER + rm / 2;
    m_barHight = ((rect.Height() - BAND_WND_BORDER * 2) / 2) * 2;
    m_levelStep = m_barHight / LEVEL_STEP_PART;
    if(!CWnd::Create(NULL, _T("SpectrumWnd"), WS_CHILD|WS_VISIBLE, rect, pParentWnd, 0, NULL))
    {
        return FALSE;
    }

    SetTimer(UPDATE_TIMER_ID, 50, NULL);
    return TRUE;
}

BOOL CSpectrumWnd::DrawSpectrumView(HDC hDC, BOOL bForceRedraw)
{
    BOOL bUpdated = FALSE;
    if(!DrawSpectrumBitmap(m_hBmpView, bUpdated))
    {
        return FALSE;
    }
    BITMAP_INFO *BmpData = (BITMAP_INFO *)::GlobalLock(m_hBmpView);
    if(BmpData == NULL)
    {
        return FALSE;
    }

	if(bForceRedraw || bUpdated)
	{
	    ::SetStretchBltMode(hDC, COLORONCOLOR);
        ::StretchDIBits(hDC, 0, 0, BmpData->bmiHeader.biWidth, BmpData->bmiHeader.biHeight,
		                0, 0, BmpData->bmiHeader.biWidth, BmpData->bmiHeader.biHeight,
		                BmpData->rgbData, (LPBITMAPINFO)BmpData, DIB_RGB_COLORS,SRCCOPY);
	}
    ::GlobalUnlock(m_hBmpView);

    return TRUE;
}

BOOL CSpectrumWnd::InitBitmapHandle(HGLOBAL hBmp, const CRect& rect)
{
    BITMAP_INFO *BmpData = (BITMAP_INFO *)::GlobalLock(hBmp);
    if(BmpData == NULL)
    {
        return FALSE;
    }
	BmpData->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    BmpData->bmiHeader.biWidth = rect.Width();
	BmpData->bmiHeader.biHeight = rect.Height();
	BmpData->bmiHeader.biPlanes = 1;
	BmpData->bmiHeader.biBitCount = 24;
	BmpData->bmiHeader.biCompression = BI_RGB;
	BmpData->bmiHeader.biSizeImage = 0;
	BmpData->bmiHeader.biXPelsPerMeter = 0;
	BmpData->bmiHeader.biYPelsPerMeter = 0;
	BmpData->bmiHeader.biClrUsed = 0;
	BmpData->bmiHeader.biClrImportant = 0;

    ::GlobalUnlock(hBmp);
    return TRUE;
}

BOOL CSpectrumWnd::DrawSpectrumBitmap(HGLOBAL hBmp, BOOL& bUpdated)
{
    bUpdated = FALSE;

    BITMAP_INFO *BmpData = (BITMAP_INFO *)::GlobalLock(hBmp);
    if(BmpData == NULL)
    {
        return FALSE;
    }

    for(int i = 0; i < BAND_COUNT; i++)
    {
        bUpdated = DrawBandLevelBitmap(BmpData, i) || bUpdated;
    }

    ::GlobalUnlock(hBmp);
    return TRUE;
}

BOOL CSpectrumWnd::DrawBandLevelBitmap(BITMAP_INFO *BmpData, int band)
{
    BOOL bUpdated = FALSE;

    int startX = m_leftRightSpace + band * (m_barWidth + BAND_BMP_SPACE);
    int startY = BAND_WND_BORDER;
    int endY = startY + m_barHight;
    int levelY = startY + ((m_curLevel[band] - m_minLevel) * m_barHight) / ((m_maxLevel - m_minLevel) * 2) * 2;
    int topY = startY + ((m_topBar[band].level - m_minLevel) * m_barHight) / ((m_maxLevel - m_minLevel) * 2) * 2;
    int lineBytes = (BmpData->bmiHeader.biWidth * 3 + 3) / 4 * 4;

    unsigned char *rgbBegin = (unsigned char *)BmpData->rgbData;

    bUpdated = TRUE;
    for(int y = startY; y < endY; y += 2)
    {
        RGB_COLOR *rgbLine = (RGB_COLOR *)(rgbBegin + y * lineBytes);
        if(y == topY)
        {
            for(int x = startX; x < (startX + m_barWidth); x += 2)
            {
                rgbLine[x].Blue   = TOP_BAR_BLUE;
                rgbLine[x].Green  = TOP_BAR_GREEN;
                rgbLine[x].Red    = TOP_BAR_RED;
            }
        }
        else
        {
            unsigned char red, green, blue;
            if(y <= levelY)
            {
                red = LO_LEVEL_RED + (HI_LEVEL_RED - LO_LEVEL_RED) * (y - startY) / m_barHight;
                green = LO_LEVEL_GREEN + (HI_LEVEL_GREEN - LO_LEVEL_GREEN) * (y - startY) / m_barHight;
                blue = LO_LEVEL_BLUE + (HI_LEVEL_BLUE - LO_LEVEL_BLUE) * (y - startY) / m_barHight;
            }
            else
            {
                red = LEVEL_STATIC_RED;
                green = LEVEL_STATIC_GREEN;
                blue = LEVEL_STATIC_BLUE;
            }
            for(int x = startX; x < (startX + m_barWidth); x++)
            {
                rgbLine[x].Blue	  = blue;
                rgbLine[x].Green  = green;
                rgbLine[x].Red    = red;
            }

        }
    }

    return bUpdated;
}

void CSpectrumWnd::UpdateLevelOnTimer()
{
    if(m_newLevelReady)
    {
        for(int i = 0; i < BAND_COUNT; i++)
        {
            UpdateBandLevel(i, m_newLevel[i]);
        }
        m_newLevelReady = FALSE;
    }

    for(int i = 0; i < BAND_COUNT; i++)
    {
        if(m_curLevel[i] >= m_levelStep)
            m_curLevel[i] -= m_levelStep;
        else
            m_curLevel[i] = 0;

        if(m_topBar[i].wait > 0)
            m_topBar[i].wait--;
        else
        {
            m_topBar[i].level = (m_topBar[i].level > m_topBar[i].step) ? (m_topBar[i].level - m_topBar[i].step) : 0;
            if(m_topBar[i].level <= m_curLevel[i])
                m_topBar[i].level = m_curLevel[i];
            
            if(m_topBar[i].step < 64)
                m_topBar[i].step += (m_topBar[i].step / 2);
        }
    }
}

void CSpectrumWnd::UpdateBandLevel(int band, int newLevel)
{
    if(newLevel > m_curLevel[band])
    {
        m_curLevel[band] = newLevel;
    }

    if(newLevel > m_topBar[band].level)
    {
        m_topBar[band].level = newLevel;
        m_topBar[band].wait = TOP_BAR_WAIT;
        m_topBar[band].step = TOP_BAR_STEP;
    }
}


BEGIN_MESSAGE_MAP(CSpectrumWnd, CWnd)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_WM_TIMER()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
END_MESSAGE_MAP()

// CSpectrumWnd message handlers
int CSpectrumWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    // TODO:  Add your specialized creation code here

    return 0;
}

void CSpectrumWnd::OnDestroy()
{
    KillTimer(UPDATE_TIMER_ID);

    CWnd::OnDestroy();

    if(m_hBmpView)
    {
        ::GlobalFree(m_hBmpView);
        m_hBmpView = NULL;
    }
}

void CSpectrumWnd::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
}

void CSpectrumWnd::OnTimer(UINT_PTR nIDEvent)
{
    CDC *pDC = GetDC();
    if(pDC)
    {
        DrawSpectrumView(pDC->m_hDC, FALSE);
        ReleaseDC(pDC);
    }

    UpdateLevelOnTimer();

    CWnd::OnTimer(nIDEvent);
}

BOOL CSpectrumWnd::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default

    //return CWnd::OnEraseBkgnd(pDC);
    return TRUE;
}

void CSpectrumWnd::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: Add your message handler code here
    // Do not call CWnd::OnPaint() for painting messages
    DrawSpectrumView(dc.m_hDC, TRUE);
}
