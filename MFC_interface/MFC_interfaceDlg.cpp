
// COL_interfaceDlg.cpp: 구현 파일
//
#include "stdafx.h"
#include "framework.h"
#include "MFC_interface.h"
#include "MFC_interfaceDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFCinterfaceDlg 대화 상자

CMFCinterfaceDlg::CMFCinterfaceDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_COL_INTERFACE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCinterfaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PORT, m_ComboPort);
	DDX_Control(pDX, IDC_COMBO_BAUD, m_ComboBaud);
	DDX_Control(pDX, IDC_CHECK_SERIAL, m_SerialOpen);
	DDX_Control(pDX, IDC_CHECK_OMEGA, m_OmegaOpen);
	DDX_Control(pDX, IDC_CHECK_UDP, m_UdpOpen);
	DDX_Control(pDX, IDC_CHECK_CAN, m_CanOpen);
	DDX_Control(pDX, IDC_CHECK_TOUCH, m_TouchOpen);
	DDX_Control(pDX, IDC_CHECK_ARTEC, m_ArtecOpen);
	DDX_Control(pDX, IDC_CHECK_VEGA, m_VegaOpen);
	DDX_Control(pDX, IDC_EDIT_STM, m_editStm);
	DDX_Control(pDX, IDC_EDIT_OMEGA, m_editOmega);
	DDX_Control(pDX, IDC_EDIT_OMEGA2, m_editOmega2);
	DDX_Control(pDX, IDC_EDIT_UDP, m_editSocket);
	DDX_Control(pDX, IDC_EDIT_UDP2, m_editSocket2);
	DDX_Control(pDX, IDC_EDIT_FORCE, m_editForce);
	DDX_Control(pDX, IDC_EDIT_TORQUE, m_editTorque);
	DDX_Control(pDX, IDC_EDIT_TOUCH, m_editTouch);
	DDX_Control(pDX, IDC_CHECK_CPLS, m_SimStart);
}

BEGIN_MESSAGE_MAP(CMFCinterfaceDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMFCinterfaceDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_SERIAL, &CMFCinterfaceDlg::OnBnClickedSerialOpen)
	ON_BN_CLICKED(IDC_CHECK_OMEGA, &CMFCinterfaceDlg::OnBnClickedOmegaOpen)
	ON_BN_CLICKED(IDC_CHECK_UDP, &CMFCinterfaceDlg::OnBnClickedUdpOpen)
	ON_BN_CLICKED(IDC_CHECK_CAN, &CMFCinterfaceDlg::OnBnClickedCanOpen)
	ON_BN_CLICKED(IDC_CHECK_TOUCH, &CMFCinterfaceDlg::OnBnClickedTouchOpen)
	ON_BN_CLICKED(IDC_CHECK_ARTEC, &CMFCinterfaceDlg::OnBnClickedArtecOpen)
	ON_BN_CLICKED(IDC_CHECK_VEGA, &CMFCinterfaceDlg::OnBnClickedVegaOpen)
	ON_BN_CLICKED(IDC_BTN_INIT, &CMFCinterfaceDlg::OnBnClickedInit)
	ON_BN_CLICKED(IDC_BTN_CAPTURE, &CMFCinterfaceDlg::OnBnClickedBtnCapture)
	ON_BN_CLICKED(IDC_BTN_CAPTURE2, &CMFCinterfaceDlg::OnBnClickedBtnCapture2)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_CPLS, &CMFCinterfaceDlg::OnBnClickedSimulationStart)
END_MESSAGE_MAP()


// CMFCinterfaceDlg 메시지 처리기

BOOL CMFCinterfaceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	SetWindowText(_T("MFC interface"));

#ifdef SERIAL
	m_ComboPort.SetCurSel(3);
	m_ComboBaud.SetCurSel(1);
	_commWorker.SetPeriod(COMM_TIME);
	_commWorker.SetWork(CreateWork<CCommWork>("CommWork"));
#endif

#ifdef OMEGA
	_omegaWorker.SetPeriod(OMEGA_TIME);
	_omegaWorker.SetWork(CreateWork<COmegaWork>("OmegaWork"));
#endif

#ifdef KUKA_UDP
#ifdef FRI_MODE
	GetDlgItem(IDC_FRI_STATE)->ShowWindow(TRUE);
	_friWorker.SetPeriod(FRI_TIME);
	_friWorker.SetWork(CreateWork<CFriClientWork>("FriClientWork"));
	_udpWorker.SetPeriod(UDP_TIME);
	_udpWorker.SetWork(CreateWork<CUdpClientWork>("UdpClientWork"));
#else
	_udpWorker.SetPeriod(UDP_TIME);
	GetDlgItem(IDC_FRI_STATE)->ShowWindow(FALSE);
	_udpWorker.SetWork(CreateWork<CUdpClientWork>("UdpClientWork"));
#endif
#endif

#ifdef CAN
	_canWorker.SetPeriod(CAN_TIME);
	_canWorker.SetWork(CreateWork<CCanWork>("CanWork"));
#endif

#ifdef TOUCH
	_touchWorker.SetPeriod(TOUCH_TIME);
	_touchWorker.SetWork(CreateWork<CTouchWork>("TouchWork"));
#endif

#ifdef ARTEC
	_artecWorker.SetPeriod(ARTEC_TIME);
	_artecWorker.SetWork(CreateWork<CArtecWork>("ArtecWork"));
#endif

#ifdef VEGA
	_vegaWorker.SetPeriod(VEGA_TIME);
	_vegaWorker.SetWork(CreateWork<CVegaWork>("VegaWork"));
#endif

#ifdef CPLS
	_cplsWorker.SetPeriod(CPLS_TIME);
	_cplsWorker.SetWork(CreateWork<CCoppeliasimWork>("CoppeliasimWork"));
#endif

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCinterfaceDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCinterfaceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCinterfaceDlg::OnBnClickedSerialOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
#ifdef SERIAL
	if (m_SerialOpen.GetCheck()) {
		CString port, baud;
		m_ComboPort.GetLBText(m_ComboPort.GetCurSel(), port);
		m_ComboBaud.GetLBText(m_ComboBaud.GetCurSel(), baud);
		int nTmp = atoi(baud.GetBuffer());

		if (((CCommWork*)_commWorker.GetWork())->OpenPort(port.GetBuffer(), nTmp)) {
			_commWorker.StartWork();
			printf("Serial Open                                                                         \r");
			SetTimer(STM_DISPLAY_TIMER, 20, NULL);
			m_SerialOpen.SetWindowText("Serial\nClose");
		}
		else {
			MessageBox("Can't open port", "Error");
			m_SerialOpen.SetCheck(false);
		}
	}
	else {
		_commWorker.StopWork();
		((CCommWork*)_commWorker.GetWork())->ClosePort();
		printf("Serial Closed                                                                       \r");
		KillTimer(STM_DISPLAY_TIMER);
		m_editStm.SetWindowText("");
		m_SerialOpen.SetWindowText("Serial\nOpen");
	}
#else
	MessageBox("No #define SERIAL", "Error");
	m_SerialOpen.SetCheck(false);
#endif
}

void CMFCinterfaceDlg::OnBnClickedOmegaOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
#ifdef OMEGA
	if (m_OmegaOpen.GetCheck()) {
		if (((COmegaWork*)_omegaWorker.GetWork())->OpenPort()) {
			_omegaWorker.StartWork();
			printf("Omega  Open                                                                         \r");
			SetTimer(OMEGA_DISPLAY_TIMER, 20, NULL);
			m_OmegaOpen.SetWindowText("Omega\nClose");
		}
		else {
			MessageBox("Can't open device", "Error");
			m_OmegaOpen.SetCheck(false);
		}
	}
	else {
		_omegaWorker.StopWork();
		((COmegaWork*)_omegaWorker.GetWork())->ClosePort();
		printf("Omega  Closed                                                                       \r");
		KillTimer(OMEGA_DISPLAY_TIMER);
		m_editOmega.SetWindowText("");
		m_OmegaOpen.SetWindowText("Omega\nOpen");
	}
#else
	MessageBox("No #define OMEGA", "Error");
	m_OmegaOpen.SetCheck(false);
#endif
}

void CMFCinterfaceDlg::OnBnClickedInit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
#ifdef OMEGA
	omega_state = 0;
	SET_SYSTEM_MEMORY("OmegaWork_State", omega_state);

	printf("initializing...\r");
	if (!drdIsInitialized() && drdAutoInit() < 0) {
		MessageBox("auto-initialization failed");
		return;
	}
	drdStart();
	if (drdMoveToPos(0.0, 0.0, 0.0) < 0) {
		MessageBox("failed to move to central position");
		return;
	}
	else {
		omega_state = 1;
		SET_SYSTEM_MEMORY("OmegaWork_State", omega_state);
		drdStop(true);
	}
	dhdEnableForce(DHD_ON);
#else
	MessageBox("No #define OMEGA", "Error");
#endif
}

void CMFCinterfaceDlg::OnBnClickedUdpOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
#ifdef KUKA_UDP
	if (m_UdpOpen.GetCheck()) {
#ifdef FRI_MODE
		if ((((CUdpClientWork*)_udpWorker.GetWork())->OpenPort()) && (((CFriClientWork*)_friWorker.GetWork())->OpenPort())) {
			_friWorker.StartWork();
#else
		if (((CUdpClientWork*)_udpWorker.GetWork())->OpenPort()) {
#endif
			_udpWorker.StartWork();
			printf("UDP  Open                                                                         \r");
			SetTimer(UDP_DISPLAY_TIMER, 20, NULL);
			m_UdpOpen.SetWindowText("UDP\nClose");
		}
		else {
			MessageBox("Can't open device", "Error");
			m_UdpOpen.SetCheck(false);
		}
	}
	else {
#ifdef FRI_MODE
		_friWorker.StopWork();
		((CFriClientWork*)_friWorker.GetWork())->ClosePort();
		_udpWorker.StopWork();
		((CUdpClientWork*)_udpWorker.GetWork())->ClosePort();
#else
		_udpWorker.StopWork();
		((CUdpClientWork*)_udpWorker.GetWork())->ClosePort();
#endif
		printf("UDP Closed                                                                       \r");
		KillTimer(UDP_DISPLAY_TIMER);
		m_editSocket.SetWindowText("");
		m_editSocket2.SetWindowText("");
		m_UdpOpen.SetWindowText("UDP\nOpen");
	}
#else
	MessageBox("No #define KUKA_UDP", "Error");
	m_UdpOpen.SetCheck(false);
#endif
}

void CMFCinterfaceDlg::OnBnClickedCanOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
#ifdef CAN
	if (m_CanOpen.GetCheck()) {
		if (((CCanWork*)_canWorker.GetWork())->OpenPort()) {
			_canWorker.StartWork();
			printf("CAN Open                                                                         \r");
			SetTimer(CAN_DISPLAY_TIMER, 20, NULL);
			m_CanOpen.SetWindowText("CAN\nClose");
		}
		else {
			MessageBox("Can't open port", "Error");
			m_CanOpen.SetCheck(false);
		}
	}
	else {
		_canWorker.StopWork();
		((CCanWork*)_canWorker.GetWork())->ClosePort();
		printf("CAN Closed                                                                       \r");
		KillTimer(CAN_DISPLAY_TIMER);
		m_editForce.SetWindowText("");
		m_editTorque.SetWindowText("");
		m_CanOpen.SetWindowText("CAN\nOpen");
	}
#else
	MessageBox("No #define CAN", "Error");
	m_CanOpen.SetCheck(false);
#endif
}

void CMFCinterfaceDlg::OnBnClickedTouchOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
#ifdef TOUCH
	if (m_TouchOpen.GetCheck()) {
		if (((CTouchWork*)_touchWorker.GetWork())->OpenPort()) {
			_touchWorker.StartWork();
			printf("Touch Open                                                                         \r");
			SetTimer(TOUCH_DISPLAY_TIMER, 20, NULL);
			m_TouchOpen.SetWindowText("Touch\nClose");
		}
		else {
			MessageBox("Can't open port", "Error");
			m_TouchOpen.SetCheck(false);
		}
	}
	else {
		_touchWorker.StopWork();
		((CTouchWork*)_touchWorker.GetWork())->ClosePort();
		printf("Touch Closed                                                                       \r");
		KillTimer(TOUCH_DISPLAY_TIMER);
		m_editTouch.SetWindowText("");
		m_TouchOpen.SetWindowText("Touch\nOpen");
	}
#else
	MessageBox("No #define TOUCH", "Error");
	m_TouchOpen.SetCheck(false);
#endif
}

void CMFCinterfaceDlg::OnBnClickedArtecOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
#ifdef ARTEC
	if (m_ArtecOpen.GetCheck()) {
		if (((CArtecWork*)_artecWorker.GetWork())->OpenPort()) {
			_artecWorker.StartWork();
			printf("Artec Open                                                                         \r");
			SetTimer(ARTEC_DISPLAY_TIMER, 20, NULL);
			m_ArtecOpen.SetWindowText("Artec\nClose");
		}
		else {
			MessageBox("Can't open port", "Error");
			m_ArtecOpen.SetCheck(false);
		}
	}
	else {
		_artecWorker.StopWork();
		((CArtecWork*)_artecWorker.GetWork())->ClosePort();
		printf("Artec Closed                                                                       \r");
		KillTimer(ARTEC_DISPLAY_TIMER);
		//m_editArtec.SetWindowText("");
		m_ArtecOpen.SetWindowText("Artec\nOpen");
	}
#else
	MessageBox("No #define ARTEC", "Error");
	m_ArtecOpen.SetCheck(false);
#endif
}

void CMFCinterfaceDlg::OnBnClickedVegaOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
#ifdef VEGA
	if (m_VegaOpen.GetCheck()) {
		if (((CVegaWork*)_vegaWorker.GetWork())->OpenPort()) {
			_vegaWorker.StartWork();
			printf("Vega Open                                                                         \r");
			SetTimer(VEGA_DISPLAY_TIMER, 20, NULL);
			m_VegaOpen.SetWindowText("Vega\nClose");
	}
		else {
			MessageBox("Can't open port", "Error");
			m_VegaOpen.SetCheck(false);
		}
}
	else {
		_vegaWorker.StopWork();
		((CVegaWork*)_vegaWorker.GetWork())->ClosePort();
		printf("Vega Closed                                                                       \r");
		KillTimer(VEGA_DISPLAY_TIMER);
		//m_editArtec.SetWindowText("");
		m_VegaOpen.SetWindowText("Vega\nOpen");
	}
#else
	MessageBox("No #define VEGA", "Error");
	m_VegaOpen.SetCheck(false);
#endif
}

void CMFCinterfaceDlg::OnBnClickedSimulationStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
#ifdef CPLS
	if (m_SimStart.GetCheck()) {
		if (((CCoppeliasimWork*)_cplsWorker.GetWork())->OpenPort()) {
			_cplsWorker.StartWork();
			printf("Simulation Start                                                                         \r");
			SetTimer(CPLS_DISPLAY_TIMER, 20, NULL);
			m_SimStart.SetWindowText("Sim\nStop");
		}
		else {
			MessageBox("Can't sim start", "Error");
			m_SimStart.SetCheck(false);
		}
	}
	else {
		_cplsWorker.StopWork();
		((CCoppeliasimWork*)_cplsWorker.GetWork())->ClosePort();
		printf("Simulation Closed                                                                       \r");
		KillTimer(CPLS_DISPLAY_TIMER);
		//m_editSimTime.SetWindowText("");
		m_SimStart.SetWindowText("Sim\nStart");
	}
#else
	MessageBox("No #define CPLS", "Error");
	m_SimStart.SetCheck(false);
#endif
}

void CMFCinterfaceDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
#ifdef SERIAL
	if (!m_SerialOpen.GetCheck()) {
		KillTimer(STM_DISPLAY_TIMER);
		_commWorker.StopWork();
		((CCommWork*)_commWorker.GetWork())->ClosePort();
	}
#endif
#ifdef OMEGA
	if (!m_OmegaOpen.GetCheck()) {
		KillTimer(OMEGA_DISPLAY_TIMER);
		_omegaWorker.StopWork();
		((COmegaWork*)_omegaWorker.GetWork())->ClosePort();
	}
#endif
#ifdef KUKA_UDP
	if (!m_UdpOpen.GetCheck()) {
		KillTimer(UDP_DISPLAY_TIMER);
		_udpWorker.StopWork();
#ifdef FRI_MODE
		((CFriClientWork*)_friWorker.GetWork())->ClosePort();
		((CUdpClientWork*)_udpWorker.GetWork())->ClosePort();
#else
		((CUdpClientWork*)_udpWorker.GetWork())->ClosePort();
#endif
#endif
	}
#ifdef CAN
	if (!m_CanOpen.GetCheck()) {
		KillTimer(CAN_DISPLAY_TIMER);
		_canWorker.StopWork();
		((CCanWork*)_canWorker.GetWork())->ClosePort();
	}
#endif
#ifdef TOUCH
	if (!m_TouchOpen.GetCheck()) {
		KillTimer(TOUCH_DISPLAY_TIMER);
		_touchWorker.StopWork();
		((CTouchWork*)_touchWorker.GetWork())->ClosePort();
	}
#endif
#ifdef ARTEC
	if (!m_ArtecOpen.GetCheck()) {
		KillTimer(ARTEC_DISPLAY_TIMER);
		_artecWorker.StopWork();
		((CArtecWork*)_artecWorker.GetWork())->ClosePort();
	}
#endif
#ifdef VEGA
	if (!m_VegaOpen.GetCheck()) {
		KillTimer(VEGA_DISPLAY_TIMER);
		_vegaWorker.StopWork();
		((CVegaWork*)_vegaWorker.GetWork())->ClosePort();
	}
#endif
#ifdef CPLS
	if (!m_SimStart.GetCheck()) {
		KillTimer(CPLS_DISPLAY_TIMER);
		_cplsWorker.StopWork();
		((CCoppeliasimWork*)_cplsWorker.GetWork())->ClosePort();
	}
#endif
	CDialogEx::OnOK();
}


void CMFCinterfaceDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == STM_DISPLAY_TIMER) {
		StmData_t stm_data;
		GET_SYSTEM_MEMORY("CommWork_Stm", stm_data);

		CString str;
		str.Format(" fdb(%4.01f %4.01f %4.01f)cnt \r",
			stm_data.fdb[0], stm_data.fdb[1], stm_data.fdb[2]);
		m_editStm.SetWindowText(str);
	}
	else if (nIDEvent == OMEGA_DISPLAY_TIMER) {
		OmegaData_t omega_data;
		GET_SYSTEM_MEMORY("OmegaWork_Data", omega_data);

		CString str;
		str.Format(" p(%6.02f %6.02f %6.02f)mm \r",
			omega_data.pos[0] * 1000., omega_data.pos[1] * 1000., omega_data.pos[2] * 1000.);
		m_editOmega.SetWindowText(str);
		str.Format(" o(%4d %4d %4d)deg | g(%3d)deg \r",
			(int)omega_data.ori[0], (int)omega_data.ori[1], (int)omega_data.ori[2], (int)omega_data.grp);
		m_editOmega2.SetWindowText(str);
	}
	else if (nIDEvent == UDP_DISPLAY_TIMER) {
		KuKaData_t kuka_data;
		GET_SYSTEM_MEMORY("UdpClientWork_KuKa", kuka_data);					//"FriClientWork_KuKa"

		CString str;
		str.Format("%4.02lf, %4.02lf, %4.02lf, \r",							
			kuka_data.jpos[0], kuka_data.jpos[1], kuka_data.jpos[2]);
		m_editSocket.SetWindowText(str);
		str.Format("%4.02lf, %4.02lf, %4.02lf, %4.02lf\r",
			kuka_data.jpos[3], kuka_data.jpos[4], kuka_data.jpos[5], kuka_data.jpos[6]);
		m_editSocket2.SetWindowText(str);

		//CString str;
		//str.Format("p(%4.02f %4.02f %4.02f)mm \r",							//str.Format(" f(%4.02f %4.02f %4.02f) \r",						
		//	kuka_data.pos[0], kuka_data.pos[1], kuka_data.pos[2]);			//	kuka_data.force[0], kuka_data.force[1], kuka_data.force[2]);
		//m_editSocket.SetWindowText(str);									//m_editSocket.SetWindowText(str);								
		//str.Format("o(%4.02f %4.02f %4.02f)rad \r",							//str.Format(" t(%4.02f %4.02f %4.02f) \r",						
		//	kuka_data.ori[0], kuka_data.ori[1], kuka_data.ori[2]);			//	kuka_data.torq[0], kuka_data.torq[1], kuka_data.torq[2]);	
		//m_editSocket2.SetWindowText(str);									//m_editSocket2.SetWindowText(str);								
	}
	else if (nIDEvent == CAN_DISPLAY_TIMER) {
		CanData_t can_data;
		GET_SYSTEM_MEMORY("CanWork_Data", can_data);

		CString str;
		str.Format(" f(%4.02f %4.02f %4.02f) \r",
			can_data.force[0], can_data.force[1], can_data.force[2]);
		m_editForce.SetWindowText(str);
		str.Format(" t(%4.02f %4.02f %4.02f) \r",
			can_data.torq[0], can_data.torq[1], can_data.torq[2]);
		m_editTorque.SetWindowText(str);
	}
	else if (nIDEvent == TOUCH_DISPLAY_TIMER) {
		TouchData_t touch_data;
		GET_SYSTEM_MEMORY("TouchWork_Data", touch_data);

		CString str;
		str.Format(" p(%4.02f %4.02f %4.02f) \r",
			touch_data.pos[0], touch_data.pos[1], touch_data.pos[2]);
		m_editTouch.SetWindowText(str);
	}
	else if (nIDEvent == ARTEC_DISPLAY_TIMER) {
		ArtecData_t artec_data;
		GET_SYSTEM_MEMORY("ArtecWork_Data", artec_data);

		//CString str;
		//str.Format(" (%4.02f %4.02f %4.02f) \r",
		//	artec_data.data[0], artec_data.data[1], artec_data.data[2]);
		//m_editArtec.SetWindowText(str);
	}
	else if (nIDEvent == VEGA_DISPLAY_TIMER) {
		//ArtecData_t artec_data;
		//GET_SYSTEM_MEMORY("ArtecWork_Data", artec_data);

		//CString str;
		//str.Format(" (%4.02f %4.02f %4.02f) \r",
		//	artec_data.data[0], artec_data.data[1], artec_data.data[2]);
		//m_editArtec.SetWindowText(str);
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CMFCinterfaceDlg::OnBnClickedBtnCapture()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_ArtecOpen.GetCheck()) {
		((CArtecWork*)_artecWorker.GetWork())->captureOneFrame();
	}
	else {
		MessageBox("Not opened", "Error");
	}
}

void CMFCinterfaceDlg::OnBnClickedBtnCapture2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_VegaOpen.GetCheck()) {
		((CVegaWork*)_vegaWorker.GetWork())->captureOneFrame();
	}
	else {
		MessageBox("Not opened", "Error");
	}
}