
// calendar2Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "calendar2.h"
#include "calendar2Dlg.h"
#include "ChineseCalendar.h"
#include "support.h"
#include "calendar_func.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// Ccalendar2Dlg 对话框




Ccalendar2Dlg::Ccalendar2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(Ccalendar2Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_CurrentMonth = 0;
}

void Ccalendar2Dlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STC_CALENDAR, m_CalendarCtrl);
}

BEGIN_MESSAGE_MAP(Ccalendar2Dlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BTN_SET_YEAR, &Ccalendar2Dlg::OnBnClickedBtnSetYear)
    ON_BN_CLICKED(IDC_BTN_LAST_MONTH, &Ccalendar2Dlg::OnBnClickedBtnLastMonth)
    ON_BN_CLICKED(IDC_BTN_NEXT_MONTH, &Ccalendar2Dlg::OnBnClickedBtnNextMonth)
    ON_BN_CLICKED(IDC_BTN_INFORMATION, &Ccalendar2Dlg::OnBnClickedBtnInformation)
    ON_BN_CLICKED(IDC_BTN_TEST, &Ccalendar2Dlg::OnBnClickedBtnTest)
    ON_BN_CLICKED(IDC_BTN_SOLARTERM, &Ccalendar2Dlg::OnBnClickedBtnSolarterm)
    ON_BN_CLICKED(IDC_BTN_NEWMOON, &Ccalendar2Dlg::OnBnClickedBtnNewmoon)
END_MESSAGE_MAP()


// Ccalendar2Dlg 消息处理程序

BOOL Ccalendar2Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
    SetDlgItemText(IDC_EDIT_YEAR, _T("2015"));
    m_CalendarCtrl.SetGeriYear(2015);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void Ccalendar2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Ccalendar2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Ccalendar2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Ccalendar2Dlg::OnBnClickedBtnSetYear()
{
    int year = GetDlgItemInt(IDC_EDIT_YEAR);
    if(year < 1600)
    {
        AfxMessageBox(_T("不支持1600年以前的历史农历计算！"));
        return;
    }
    
    m_CalendarCtrl.SetGeriYear(year);
}

void Ccalendar2Dlg::OnBnClickedBtnLastMonth()
{
    m_CalendarCtrl.LastMonth();
}

void Ccalendar2Dlg::OnBnClickedBtnNextMonth()
{
    m_CalendarCtrl.NextMonth();
}

void Ccalendar2Dlg::OnBnClickedBtnInformation()
{
    // TODO: Add your control notification handler code here
}

void Ccalendar2Dlg::OnBnClickedBtnSolarterm()
{
    // TODO: Add your control notification handler code here
}

void Ccalendar2Dlg::OnBnClickedBtnNewmoon()
{
    // TODO: Add your control notification handler code here
}

/*从某一年的某个节气开始，连续计算25个节气，返回各节气的儒略日，本地时间*/
void GetAllSolarTermsJD(int year, double *SolarTerms, int start)
{
    int i = 0;
    int st = start;
    while(i < 25)
    {
        double jd = CalculateSolarTerms(year, st * 15);
        SolarTerms[i++] = JDTDtoLocalTime(jd);
        if(st == WINTER_SOLSTICE)
        {
            year++;
        }
        st = (st + 1) % SOLAR_TERMS_COUNT;
    }
}

void PrintJDTimes(double *jds, int count)
{
    char timeStr[128];

    TRACE("\n");
    for(int i = 0; i < count; i++)
    {
        JDtoString(jds[i], timeStr, 128);
        TRACE("%f, %s\n", jds[i], timeStr);
    }
    TRACE("\n");
}

void Ccalendar2Dlg::OnBnClickedBtnTest()
{
    int this_year = 2011;

    double solarTerms[25];
    GetAllSolarTermsJD(this_year - 1, solarTerms, WINTER_SOLSTICE);
    PrintJDTimes(solarTerms, 25);

}
