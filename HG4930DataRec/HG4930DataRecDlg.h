
// HG4930DataRecDlg.h: 头文件
//
#pragma once

#include"SerialPort.h"
#include"afxwin.h"


using namespace std;
using namespace itas109;

extern CSerialPort m_Com;
// CHG4930DataRecDlg 对话框
class CHG4930DataRecDlg : public CDialogEx
{
// 构造
public:
	CHG4930DataRecDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HG4930DATAREC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD dwData);
	//afx_msg LONG OnCommunication(WPARAM ch, LPARAM port);    // 串口接收处理数据
	LRESULT afx_msg OnReceiveStr(WPARAM strRec, LPARAM commInfo);
	DECLARE_MESSAGE_MAP()
public:
	// 串口号选择
	CComboBox m_PortNum;
	// 波特率下拉选项
	CComboBox m_BaudRate;
	// 数据位
	CComboBox m_DataBit;
	// 校验位
	CComboBox m_CaliBit;
	CComboBox m_StopBit;
	// 数据流控制
	CComboBox m_StreamBit;
	// 数据传输的帧率
	CEdit m_FrameRate;
	// 接收数据帧计数
	CEdit m_DataCount;
	// 保存的数据文件路径
	CEdit m_FilePath;
	// 原始数据窗口，调试用
	CEdit m_RawData;
	CEdit m_CtrlMeassage;
	// X轴角速度
	CEdit m_AguRateX;
	// Y轴加速度
	CEdit m_AguRateY;
	// Z轴角速度
	CEdit m_AguRateZ;
	// X轴加速度
	CEdit m_AcceX;
	// Y轴加速度
	CEdit m_AccY;
	// Z
	CEdit m_AccZ;
	// X轴角速度增量
	CEdit m_AngDeltaX;
	// 角速度增量Y
	CEdit m_AngDeltaY;
	// Z轴角增量
	CEdit m_AngDeltaZ;
	// X轴速度增量
	CEdit m_VelDeltaX;
	// Y轴速度增量
	CEdit m_VelDeltaY;
	// Z轴加速度
	CEdit m_VelDeltaZ;
	/*自己额外定义的函数 及变量*/
	// 串口配置
	//CUIntArray m_Ports, m_PortsU, m_PortsE;
	//void AddCom();
	//void RefreshCom();
	// 串口初始化	
	afx_msg void OnBnClickedBtnOpen();	
	// 打开串口的按钮
	CButton m_OpenCom;
	afx_msg void OnBnClickedSend();	
	void ResetDisplay();
};
