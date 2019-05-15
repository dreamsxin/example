#pragma once

const int BAND_COUNT = 32;
const int TOP_BAR_WAIT = 3;
const int LEVEL_STEP_PART = 12;
const int TOP_BAR_STEP = 2;

const unsigned char TOP_BAR_RED    = 192;
const unsigned char TOP_BAR_GREEN  = 192;
const unsigned char TOP_BAR_BLUE   = 192;

const unsigned char LEVEL_STATIC_RED = 32;
const unsigned char LEVEL_STATIC_GREEN = 32;
const unsigned char LEVEL_STATIC_BLUE = 64;

const unsigned char HI_LEVEL_RED    = 255;
const unsigned char HI_LEVEL_GREEN  = 0;
const unsigned char HI_LEVEL_BLUE   = 64;

const unsigned char LO_LEVEL_RED      = 32;
const unsigned char LO_LEVEL_GREEN    = 192;
const unsigned char LO_LEVEL_BLUE     = 0;
//const unsigned char LO_LEVEL_RED      = 64;
//const unsigned char LO_LEVEL_GREEN    = 128;
//const unsigned char LO_LEVEL_BLUE     = 255;

const int BAND_BMP_SPACE = 2;
const int BAND_WND_BORDER = 2;
const int BAND_BMP_WIDTH = 6;

typedef struct tagTopBar 
{
	int level; 
	int wait;
    int step;
}TOP_BAR;

typedef struct tagRgbColor 
{
    unsigned char Blue;
    unsigned char Green;
    unsigned char Red;
}RGB_COLOR;

typedef struct tagBitmapInfoT
{
    BITMAPINFOHEADER bmiHeader;
    RGB_COLOR rgbData[1];
}BITMAP_INFO;

// CSpectrumWnd

class CSpectrumWnd : public CWnd
{
	DECLARE_DYNAMIC(CSpectrumWnd)

public:
	CSpectrumWnd();
	virtual ~CSpectrumWnd();

    BOOL CreateSpectrumWnd(CWnd* pParentWnd, const CRect& rect);

    void SetLevelLimit(int minl, int maxl);
    BOOL SetBandLevel(int *level, int bands);

protected:
    int m_minLevel;
    int m_maxLevel;
    int m_levelStep;
    int m_curLevel[BAND_COUNT];
    int m_newLevel[BAND_COUNT];
    TOP_BAR m_topBar[BAND_COUNT];
    int m_barWidth;
    int m_barHight;
    int m_leftRightSpace;
    BOOL m_newLevelReady;
    HGLOBAL m_hBmpView; 

    BOOL DrawSpectrumView(HDC hDC, BOOL bForceRedraw);
    BOOL InitBitmapHandle(HGLOBAL hBmp, const CRect& rect);
    BOOL DrawSpectrumBitmap(HGLOBAL hBmp, BOOL& bUpdated);
    BOOL DrawBandLevelBitmap(BITMAP_INFO *BmpData, int band);
    void UpdateLevelOnTimer();
    void UpdateBandLevel(int band, int newLevel);

	DECLARE_MESSAGE_MAP()
public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
};


