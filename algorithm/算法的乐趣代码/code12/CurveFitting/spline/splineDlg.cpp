
// splineDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "spline.h"
#include "splineDlg.h"
#include "ThomasEquation.h"
#include "Spline3.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

double FuncY(double dbX)
{
	return 3.0 / (1.0 + dbX * dbX);
}

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


// CsplineDlg 对话框




CsplineDlg::CsplineDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CsplineDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_pDispWnd = NULL;
}

void CsplineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CsplineDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BTN_SPLINE, &CsplineDlg::OnBnClickedBtnSpline)
    ON_WM_DESTROY()
END_MESSAGE_MAP()


// CsplineDlg 消息处理程序

BOOL CsplineDlg::OnInitDialog()
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

	m_pDispWnd = new CDisplayWnd;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CsplineDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CsplineDlg::OnPaint()
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
HCURSOR CsplineDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CsplineDlg::OnBnClickedBtnSpline()
{
    double sourceX[17] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 7.5, 8.0};
    double sourceY[17];

    for(int i = 0; i < 17; i++)
    {
        sourceY[i] = FuncY(sourceX[i]);
    }

    if(m_pDispWnd != NULL)
    {
        SplineFitting fitting;
        //fitting.CalcSpline(sourceX, sourceY, 9, 1, 0.0, -0.004608);
        fitting.CalcSpline(sourceX, sourceY, 17, 1, 0.0, -0.01136);

        double xi[1024],yi[1024],si[1024];
        int ind = 0;
        for(double i = sourceX[0]; i < sourceX[16]; i += 0.01)
        {
            xi[ind] = i;
            yi[ind] = FuncY(i);
            si[ind] = fitting.GetValue(i);
            ind++;
        }

        //m_pDispWnd->SetInterpolationPoint(sourceX, sourceY, 9);
        m_pDispWnd->SetSourceCurve(xi, yi, ind);
        m_pDispWnd->SetSourceSplineCurve(xi, si, ind);
        m_pDispWnd->SetPointLogicRange(-1.0, 9.0, -1.0, 5.0);
        m_pDispWnd->ShowDisplayWindow(this);
    }
}

void CsplineDlg::OnDestroy()
{

    CDialog::OnDestroy();
}
