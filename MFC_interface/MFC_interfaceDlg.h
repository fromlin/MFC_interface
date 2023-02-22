
// COL_interfaceDlg.h: 헤더 파일
//

#pragma once
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

/* Predefined */
//#define SERIAL
//#define OMEGA
#define KUKA_UDP
//#define CAN
//#define TOUCH
#define ARTEC
#define VEGA
#define CPLS

#ifdef KUKA_UDP
#ifdef  _M_IX86
#define FRI_MODE
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "Comm.h"
#include "DeviceListReader.h"
#include "ThreadWorker.h"
#include "SharedMemory.h"
#include "SystemMemory.h"
#include "DataType.h"

#include "dhdc.h"
#include "drdc.h"

#ifdef SERIAL
#include "CommWork.h"
#endif
#ifdef OMEGA
#include "OmegaWork.h"
#endif
#ifdef KUKA_UDP
#ifdef FRI_MODE
#include "FriClientWork.h"
#include "UdpClientWork.h"
#else
#include "UdpClientWork.h"
#endif
#endif
#ifdef CAN
#include "CanWork.h"
#endif
#ifdef TOUCH
#include "TouchWork.h"
#endif
#ifdef ARTEC
#include "ArtecWork.h"
#endif
#ifdef VEGA
#include "VegaWork.h"
#endif
#ifdef CPLS
#include "CoppeliasimWork.h"
#endif


#define STM_DISPLAY_TIMER		0
#define OMEGA_DISPLAY_TIMER		1
#define UDP_DISPLAY_TIMER		2
#define CAN_DISPLAY_TIMER		3
#define TOUCH_DISPLAY_TIMER		4
#define ARTEC_DISPLAY_TIMER		5
#define VEGA_DISPLAY_TIMER		6
#define CPLS_DISPLAY_TIMER		7

#define COMM_TIME				0.005		// 200[Hz]
#define OMEGA_TIME				0.00025     //  4k[Hz]
#ifdef FRI_MODE
#define FRI_TIME				0.0005		//  2k[Hz]
#define UDP_TIME				0.001		//  1k[Hz]
#else
#define UDP_TIME				0.005		// 200[Hz]
#endif
#define CAN_TIME				0.0005		//  2k[Hz]
#define TOUCH_TIME				0.004		// 250[Hz]
#define ARTEC_TIME				0.01		// 100[Hz]
#define VEGA_TIME				0.01		// 100[Hz]
#define CPLS_TIME				0.01		// 100[Hz]

// CMFCinterfaceDlg 대화 상자
class CMFCinterfaceDlg : public CDialogEx
{
	// 생성입니다.
public:
	CMFCinterfaceDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.


	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COL_INTERFACE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;
	CComm m_comm;
#ifdef SERIAL
	CThreadedWorker _commWorker;
#endif
#ifdef OMEGA
	CThreadedWorker _omegaWorker;
#endif
#ifdef KUKA_UDP
#ifdef FRI_MODE
	CThreadedWorker _friWorker;
#endif
	CThreadedWorker _udpWorker;
#endif
#ifdef CAN
	CThreadedWorker _canWorker;
#endif
#ifdef TOUCH
	CThreadedWorker _touchWorker;
#endif
#ifdef ARTEC
	CThreadedWorker _artecWorker;
#endif
#ifdef VEGA
	CThreadedWorker _vegaWorker;
#endif
#ifdef CPLS
	CThreadedWorker _cplsWorker;
#endif

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSerialOpen();
	afx_msg void OnBnClickedOmegaOpen();
	afx_msg void OnBnClickedInit();
	afx_msg void OnBnClickedUdpOpen();
	afx_msg void OnBnClickedCanOpen();
	afx_msg void OnBnClickedTouchOpen();
	afx_msg void OnBnClickedArtecOpen();
	afx_msg void OnBnClickedVegaOpen();
	afx_msg void OnBnClickedSimulationStart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	CComboBox m_ComboPort;
	CComboBox m_ComboBaud;

	CButton m_SerialOpen;
	CButton m_OmegaOpen;
	CButton m_UdpOpen;
	CButton m_CanOpen;
	CButton m_TouchOpen;
	CButton m_ArtecOpen;
	CButton m_VegaOpen;
	CButton m_SimStart;

	CEdit m_editStm;
	CEdit m_editOmega;
	CEdit m_editOmega2;
	CEdit m_editSocket;
	CEdit m_editSocket2;
	CEdit m_editForce;
	CEdit m_editTorque;
	CEdit m_editTouch;

	bool omega_state;
	afx_msg void OnBnClickedBtnCapture();
	afx_msg void OnBnClickedBtnCapture2();
};