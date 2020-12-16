
// HG4930DataRecDlg.cpp: 实现文件
//
//#include "stdafx.h"
#include "pch.h"
#include "framework.h"
#include "HG4930DataRec.h"
#include "HG4930DataRecDlg.h"
#include "Dbt.h"
#include "math.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

constexpr float pi = 3.14159265358979323846;
constexpr float m_g = 9.805989024;
#define AguLSBW01 pow(2,-20)*600
#define AccLSBW01 pow(2,-14)*600*0.3048
#define AguLSBW02 pow(2,-15)*499*pi/180
#define AccLSBW02 pow(2,-15)*21*m_g

int BaudRate[] = { 9600, 14400, 19200, 38400, 56000, 57600, 115200 };
CSerialPort m_Com;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)	
END_MESSAGE_MAP()


// CHG4930DataRecDlg 对话框



CHG4930DataRecDlg::CHG4930DataRecDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HG4930DATAREC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHG4930DataRecDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_PortNum);
	DDX_Control(pDX, IDC_COMBO2, m_BaudRate);
	DDX_Control(pDX, IDC_COMBO3, m_DataBit);
	DDX_Control(pDX, IDC_COMBO4, m_CaliBit);
	DDX_Control(pDX, IDC_COMBO5, m_StopBit);
	DDX_Control(pDX, IDC_COMBO6, m_StreamBit);
	DDX_Control(pDX, IDC_EDIT1, m_FrameRate);
	DDX_Control(pDX, IDC_EDIT2, m_DataCount);
	DDX_Control(pDX, IDC_EDIT3, m_FilePath);
	DDX_Control(pDX, IDC_EDIT17, m_RawData);
	DDX_Control(pDX, IDC_EDIT16, m_CtrlMeassage);
	DDX_Control(pDX, IDC_EDIT4, m_AguRateX);
	DDX_Control(pDX, IDC_EDIT5, m_AguRateY);
	DDX_Control(pDX, IDC_EDIT6, m_AguRateZ);
	DDX_Control(pDX, IDC_EDIT7, m_AcceX);
	DDX_Control(pDX, IDC_EDIT9, m_AccY);
	DDX_Control(pDX, IDC_EDIT10, m_AccZ);
	DDX_Control(pDX, IDC_EDIT8, m_AngDeltaX);
	DDX_Control(pDX, IDC_EDIT12, m_AngDeltaY);
	DDX_Control(pDX, IDC_EDIT14, m_AngDeltaZ);
	DDX_Control(pDX, IDC_EDIT11, m_VelDeltaX);
	DDX_Control(pDX, IDC_EDIT13, m_VelDeltaY);
	DDX_Control(pDX, IDC_EDIT15, m_VelDeltaZ);
	DDX_Control(pDX, IDOK, m_OpenCom);
}

BEGIN_MESSAGE_MAP(CHG4930DataRecDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_WM_DEVICECHANGE()
	//ON_MESSAGE(WM_COMM_RXCHAR, OnCommunication)
	ON_MESSAGE(WM_COMM_RXSTR, &CHG4930DataRecDlg::OnReceiveStr)   // 串口接收处理函数
	ON_BN_CLICKED(IDOK, &CHG4930DataRecDlg::OnBnClickedBtnOpen)
	ON_BN_CLICKED(IDCANCEL, &CHG4930DataRecDlg::OnBnClickedSend)
END_MESSAGE_MAP()


// CHG4930DataRecDlg 消息处理程序

BOOL CHG4930DataRecDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//ShowWindow(SW_MAXIMIZE);
	//ShowWindow(SW_MINIMIZE);

	// TODO: 在此添加额外的初始化代码
	// 串口配置信息的初始化
	CString temp;
	for (int i = 0; i < sizeof(BaudRate) / sizeof(int); i++) 
	{
		temp.Format(_T("%d"), BaudRate[i]);
		m_BaudRate.AddString((LPCTSTR)temp);
	}
	temp.Format(_T("%d"), 9600);
	m_BaudRate.SetCurSel(m_BaudRate.FindString(0, temp));

	// 获取串口号
	CSerialPortInfo a;
	list<string> m_PortList = CSerialPortInfo::availablePorts();
	list<string>::iterator itor;
	TCHAR m_regKeyValue[255];
	for (itor = m_PortList.begin(); itor != m_PortList.end(); ++itor) 
	{
#ifdef UNICODE
		int iLength;
		const char* _char = (*itor).c_str();
		iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char)+1, NULL, 0);
		MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, m_regKeyValue, iLength );
#else
		strcpy_s(m_regKeyValue, 255, (*itor).c_str());
#endif
		m_PortNum.AddString(m_regKeyValue);
	}
	m_PortNum.SetCurSel(0);

	OnBnClickedBtnOpen();
	m_RawData.SetWindowText(_T("Initialized!"));
	//AddCom();
	//m_PortNum.SetCurSel(0);
	//m_BaudRate.SetCurSel(0);   // 设置BaudRate的初始值参数列表的第一项
	//m_DataBit.SetCurSel(0);
	//m_CaliBit.SetCurSel(0);
	//m_StopBit.SetCurSel(0);
	//m_StreamBit.SetCurSel(0);
	

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CHG4930DataRecDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CHG4930DataRecDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CHG4930DataRecDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
// 自己定义的函数
LRESULT CHG4930DataRecDlg::OnReceiveStr(WPARAM strRec, LPARAM commPort) {
	struct serialPortInfo
	{
		UINT portNr;//串口号
		DWORD bytesRead;//读取的字节数
	}*pCommInfo;
	pCommInfo = (serialPortInfo*)commPort;
	int mBytes = pCommInfo->bytesRead;	
	CString str0((char*)strRec);
	char* RecData = (char*)strRec;
	// 原始数据显示 调试用
	CString str1;
	//str0.Format(_T("%x"),RecData);
	m_RawData.SetSel(-1,-1);
	m_RawData.ReplaceSel(str0);	
	// 数据解析1
	char cmCtrlMeas0 = 0, cmCtrlMeas1 = 0;
	float fmAguRateX=0, fmAguRateY=0, fmAguRateZ=0,  fmAccX=0 , fmAccY=0, fmAccZ=0;
	long float  lfmAngDeltaX=0, lfmAngDeltaY=0, lfmAngDeltaZ=0,
				lfmVelDeltaX=0, lfmVelDeltaY=0, lfmVelDeltaZ=0;
	CString mCtrlMeas,mAguRateX, mAguRateY, mAguRateZ, mAccX, mAccY, mAccZ;
	CString  mAngDeltaX, mAngDeltaY, mAngDeltaZ,mVelDeltaX, mVelDeltaY, mVelDeltaZ;
	// ***数据解析部分****//
	// 获取帧头并显示
	if (mBytes) 
	{
		cmCtrlMeas0 = RecData[0];      //0x0e
		cmCtrlMeas1 = RecData[1];	   // 0x01 |0x02 | 0xCA
		mCtrlMeas.Format(_T("%01x%01x"), cmCtrlMeas0,cmCtrlMeas1);
		m_CtrlMeassage.SetWindowText(mCtrlMeas);
	}
	// 获取数据
	if (cmCtrlMeas0 == 0x0e && mBytes >=14)   // 如果检测到帧头，且收到的数据长度大于14字节(防止出现内存溢出) 
	{		
			fmAguRateX = RecData[2] << 8 | RecData[3];
			fmAguRateY = RecData[4] << 8 | RecData[5];
			fmAguRateZ = RecData[6] << 8 | RecData[7];
			fmAccX = RecData[8] << 8 | RecData[9];
			fmAccY = RecData[10] << 8 | RecData[11];
			fmAccZ = RecData[12] << 8 | RecData[13];
			if (cmCtrlMeas1 == 0x01 || cmCtrlMeas1 == 0x02) 
			{
				fmAguRateX *= AguLSBW01;
				fmAguRateY *= AguLSBW01;
				fmAguRateZ *= AguLSBW01;
				fmAccX *= AccLSBW01;
				fmAccY *= AccLSBW01;
				fmAccZ *= AccLSBW01;
			}
			else if (cmCtrlMeas1 == 0xCA)
			{
				fmAguRateX *= AguLSBW02;
				fmAguRateY *= AguLSBW02;
				fmAguRateZ *= AguLSBW02;
				fmAccX *= AccLSBW02;
				fmAccY *= AccLSBW02;
				fmAccZ *= AccLSBW02;
			}
			// 增量数据
			if (mBytes >= 42 && cmCtrlMeas1 == 0x02)    // 如果是命令0x02,且数据长度大于等于42(防止出现内存溢出)
			{
				lfmAngDeltaX = (RecData[18] << 24 | RecData[19] << 16 | RecData[20] << 8 | RecData[21]) * pow(2, -33);  // 4 字节
				lfmAngDeltaY = (RecData[22] << 24 | RecData[23] << 16 | RecData[24] << 8 | RecData[25]) * pow(2, -33);
				lfmAngDeltaZ = (RecData[26] << 24 | RecData[27] << 16 | RecData[28] << 8 | RecData[29]) * pow(2, -33);
				lfmVelDeltaX = (RecData[30] << 24 | RecData[31] << 16 | RecData[32] << 8 | RecData[33]) * pow(2, -27)*0.3048;
				lfmVelDeltaY = (RecData[34] << 24 | RecData[35] << 16 | RecData[36] << 8 | RecData[37]) * pow(2, -27)*0.3048;
				lfmVelDeltaZ = (RecData[38] << 24 | RecData[39] << 16 | RecData[40] << 8 | RecData[41]) * pow(2, -27)*0.3048;
			}
	}	
	//****************数据保存*************************//
	
	// ***************数据显示*************************//
	if (mBytes) {
		// 数据格式转换
		// 原始数据
		ResetDisplay();
		mAguRateX.Format(_T("%.8f"), fmAguRateX);
		mAguRateY.Format(_T("%.8f"), fmAguRateY);
		mAguRateZ.Format(_T("%.8f"), fmAguRateZ);
		mAccX.Format(_T("%.8f"), fmAccX);
		mAccY.Format(_T("%.8f"), fmAccY);
		mAccZ.Format(_T("%.8f"), fmAccZ);
		// 增量数据
		mAngDeltaX.Format(_T("%.8lf"), lfmAngDeltaX);
		mAngDeltaY.Format(_T("%.8lf"), lfmAngDeltaY);
		mAngDeltaZ.Format(_T("%.8lf"), lfmAngDeltaZ);
		mVelDeltaX.Format(_T("%.8lf"), lfmVelDeltaX);
		mVelDeltaY.Format(_T("%.8lf"), lfmVelDeltaY);
		mVelDeltaZ.Format(_T("%.8lf"), lfmVelDeltaZ);		
		// 在图形界面显示数据
		m_AguRateX.SetWindowText(mAguRateX);
		m_AguRateY.SetWindowText(mAguRateY);
		m_AguRateZ.SetWindowText(mAguRateZ);
		m_AcceX.SetWindowText(mAccX);
		m_AccY.SetWindowText(mAccY);
		m_AccZ.SetWindowText(mAccZ);
		m_AngDeltaX.SetWindowText(mAngDeltaX);
		m_AngDeltaY.SetWindowText(mAngDeltaY);
		m_AngDeltaZ.SetWindowText(mAngDeltaZ);
		m_VelDeltaX.SetWindowText(mVelDeltaX);
		m_VelDeltaY.SetWindowText(mVelDeltaY);
		m_VelDeltaZ.SetWindowText(mVelDeltaZ);
	}	
	else
	{
		ResetDisplay();
	}
	
	return TRUE;
}
void CHG4930DataRecDlg::OnBnClickedBtnOpen()
{
	// TODO: 在此添加控件通知处理程序代码		
	CString temp;	
	m_OpenCom.GetWindowText(temp);   // 获取按钮的文本
	UpdateData(true);
	if (temp == _T("关闭串口"))  // 串口已打开
	{
		m_Com.ClosePort();		
		m_OpenCom.SetWindowText(_T("打开串口"));
		// 修改状态
		m_PortNum.EnableWindow(TRUE);
		m_BaudRate.EnableWindow(TRUE);
		m_DataBit.EnableWindow(TRUE);
		m_CaliBit.EnableWindow(TRUE);
		m_StopBit.EnableWindow(TRUE);
	}
	else  if(m_PortNum.GetCount() > 0 )  // 当前列表的内容个数
	{
		int SelPortNO, SelBaudRate;
		UpdateData(true);
		m_PortNum.GetWindowText(temp);  // CString temp
		temp.Delete(0, 3);
		SelPortNO = _tstoi(temp);

		m_BaudRate.GetWindowText(temp);
		SelBaudRate = _tstoi(temp);

		if (m_Com.InitPort(this->GetSafeHwnd(), SelPortNO, SelBaudRate))
		{
			m_Com.StartMonitoring();	
			//AfxMessageBox(_T("Monitoring!"));
			m_OpenCom.SetWindowText(_T("关闭串口"));
			// 修改状态
			m_PortNum.EnableWindow(FALSE);
			m_BaudRate.EnableWindow(FALSE);
			m_DataBit.EnableWindow(FALSE);
			m_CaliBit.EnableWindow(FALSE);
			m_StopBit.EnableWindow(FALSE);
		}
		else    // 串口打开失败
		{
			MessageBox(_T("没有发现此串口或被占用"), _T("串口打开失败"), MB_ICONWARNING);
		}
	} 	
	//CDialogEx::OnOK();
}

void CHG4930DataRecDlg::OnBnClickedSend()
{
	/*// TODO: 在此添加控件通知处理程序代码
	CString temp;
	UpdateData(true);
	m_OpenCom.GetWindowText(temp);
	if (temp == "打开串口") 
	{
		AfxMessageBox(_T("请先打开串口"));
		return;
	}
	char* m_str = "Youyinghui";
	//m_Com.WriteToPort("abc",3);
	m_Com.WriteToPort(m_str,11);*/

	CDialogEx::OnCancel();
}

void CHG4930DataRecDlg::ResetDisplay() {
	m_AguRateX.SetWindowText(_T("NaN"));
	m_AguRateY.SetWindowText(_T("NaN"));
	m_AguRateZ.SetWindowText(_T("NaN"));
	m_AcceX.SetWindowText(_T("NaN"));
	m_AccY.SetWindowText(_T("NaN"));
	m_AccZ.SetWindowText(_T("NaN"));
	m_AngDeltaX.SetWindowText(_T("NaN"));
	m_AngDeltaY.SetWindowText(_T("NaN"));
	m_AngDeltaZ.SetWindowText(_T("NaN"));
	m_VelDeltaX.SetWindowText(_T("NaN"));
	m_VelDeltaY.SetWindowText(_T("NaN"));
	m_VelDeltaZ.SetWindowText(_T("NaN"));

}
//注释掉的内容
/* 对WPARAM进行操作
	if(mBytes >= 16)
	{
		mCtrlMeas0.Format(_T("%d"),  HIBYTE(HIWORD(strRec)));   //  帧头1/ 字节计数1
		mCtrlMeas1.Format(_T("%d"),LOBYTE(HIWORD(strRec))); //  帧头2      字节计数2
		mAguRateX.Format(_T("%d"), LOWORD(strRec));  //  X轴角速度	   字节计数4
		mAguRateY.Format(_T("%d"), HIWORD(strRec+1));  //  Y轴角速度	   字节计数6
		mAguRateZ.Format(_T("%d"), LOWORD(strRec));  //  Z轴角速度	   字节计数8
		mAccX.Format(_T("%d"),     HIWORD(strRec+2));     //  X轴加速度    字节计数10
		mAccY.Format(_T("%d"),     LOWORD(strRec));     // Y轴加速度	   字节计数12
		mAccZ.Format(_T("%d"),     HIWORD(strRec+3));     //Z轴加速度	   字节计数14
	}
	//_itoa_s(RecData[5] << 8 | RecData[6], cmAccX, 10, 10);

	//mAccX = _itoa(RecData[5]<<8|RecData[6],cmAccX,10);
	//mAccX.Format('%d',(RecData[5]<< 8 |RecData[6]));
	// 数据解析2
	if (mBytes == 10) {
		mCtrlMeas0 = strRec & 0xFF00 << 8;    // 取最高8位（一个字节）的数据往后
		itoa()
	}*/