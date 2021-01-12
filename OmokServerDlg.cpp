﻿
// OmokServerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "OmokServer.h"
#include "OmokServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// COmokServerDlg 대화 상자



COmokServerDlg::COmokServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OMOKSERVER_DIALOG, pParent)
	, m_strConnect(_T("접속 전입니다."))
	, m_strMe(_T("대기중"))
	, m_strStatus(_T("대기중"))
	, m_strSend(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COmokServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Text(pDX, IDC_STATIC_CONNECT, m_strConnect);
	DDX_Text(pDX, IDC_STATIC_ME, m_strMe);
	DDX_Text(pDX, IDC_STATIC_STATUS, m_strStatus);
	DDX_Text(pDX, IDC_EDIT_SEND, m_strSend);
}

BEGIN_MESSAGE_MAP(COmokServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_SEND, &COmokServerDlg::OnBnClickedButtonSend)
	ON_MESSAGE(UM_ACCEPT, (LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM))OnAccept)
	ON_MESSAGE(UM_RECEIVE, (LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM))OnReceive)
END_MESSAGE_MAP()


// COmokServerDlg 메시지 처리기

BOOL COmokServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	// 방화벽 개방
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;

	// 서버 소켓 생성(포트번호 : 5000)
	if (m_socServer.Create(5000) == FALSE) {
		int err = m_socServer.GetLastError();
		CString error = _T("ERROR : Fail to create Server (ERROR CODE : ");
		CString strErr = _T("");
		strErr.Format(_T("%d)"), err);
		AfxMessageBox(error + strErr);
	}
	else {
		//MessageBox(_T("Success to Create Socket Server"));
	}
	// 클라이언트 접속 대기
	m_socServer.Listen();

	// 소켓 클래스와 메인 윈도우 (여기서는 CChatServerDlg)를 연결
	m_socServer.Init(this->m_hWnd);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void COmokServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void COmokServerDlg::OnPaint()
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


	// 바둑판 생성
	DrawRec();
	DrawLine();
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR COmokServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// 데이터 전송
void COmokServerDlg::SendGame(int iType, CString strTmp) {

	UpdateData(TRUE);
	char pTmp[256];
	memset(pTmp, '\0', 256);
	sprintf(pTmp, "%d%s", iType, (LPSTR)(LPCTSTR)strTmp);

	m_socCom->Send(pTmp, 256);
}

// 클라이언트에서 접속 요청이 왔을 떄
LPARAM COmokServerDlg::OnAccept(UINT wParam, LPARAM lParam) {

	// 통신용 소켓을 생성한 뒤
	m_socCom = new CSocCom;

	// 서버소켓과 통신소켓 연결
	m_socCom = m_socServer.GetAcceptSocCom();

	m_socCom->Init(this->m_hWnd);
	m_strConnect = _T("접속성공");
	m_strStatus = _T("게임을 초기화 하십시오.");

	// 접속했으니 점속 값 변경
	m_bConnect = TRUE;
	SendGame(SOC_TEXT, "접속성공");
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
	UpdateData(FALSE);

	return TRUE;
}


// 접속된 곳에서 데이터가 도착했을 때
LPARAM COmokServerDlg::OnReceive(UINT wParam, LPARAM lParam) {

	char pTmp[256];
	CString strTmp, str;
	memset(pTmp, '\0', 256);

	// 데이터를 pTmp에 받는다
	m_socCom->Receive(pTmp, 256);

	// strTmp에 헤더 저장
	strTmp.Format(_T("%c"), pTmp[0]);

	int iType = atoi((char*)(LPCTSTR)strTmp);

	if (iType == SOC_GAMESTART) {
		m_bStartCnt = TRUE;

		// 빙고판을 다 채웠을 경우
		if (m_bStart) {
			m_strMe = _T("당신의 차례입니다.");
			m_strStatus = _T("원하는곳을 선택하세요.");
			m_bMe = TRUE;
			UpdateData(FALSE);
		}
	}

	// 메시지 전송
	else if (iType == SOC_TEXT) {
		str.Format(_T("%s"), (LPCTSTR)(pTmp + 1));
		m_list.AddString(str);
	}

	// 바둑판 클릭
	else if (iType == SOC_CHECK) {
		str.Format(_T("%s"), (LPCTSTR)(pTmp + 1));
	}

	// 클라이언트 유저 승리 시
	else if (iType == SOC_GAMEEND) {

	}

	return TRUE;
}


// 사각형 그리기 (250 *250 시작은 (35, 35))
void COmokServerDlg::DrawRec() {

	CClientDC dc(this);
	CBrush br;
	br.CreateSolidBrush(RGB(218, 164, 43));

	CBrush* lbr = dc.SelectObject(&br);
	dc.Rectangle(35, 35, 35 + 525, 35 + 525);
	dc.SelectObject(lbr);
}


// 선 그리기
void COmokServerDlg::DrawLine() {

	CClientDC dc(this);
	CPen pen;

	pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));

	CPen* lodp = dc.SelectObject(&pen);

	for (int i = 0; i < 16; i++) {
		dc.MoveTo(35, 35 + i * 35);
		dc.LineTo(35 + 35 * 15 , 35 + i * 35);

	}

	for (int i = 0; i < 16; i++) {
		dc.MoveTo(35 + i * 35, 35);
		dc.LineTo(35 + i * 35, 35 * 15 + 35);
	}

	dc.SelectObject(pen);
}

void COmokServerDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	
	// 게임과 관련 없는 곳 클릭 시
	if (point.x > 535 || point.y > 535)	return;
	if (point.x < 10 || point.y < 10)	return;
	//if (!m_bConnect)	return;

	CString msg;

	msg.Format(_T("%2d %02d"), point.x, point.y);

	// 바둑알 놓기
	CClientDC dc(this);
	CBrush* p_old_brush;

	// 흑돌이면
	p_old_brush = (CBrush*)dc.SelectStockObject(BLACK_BRUSH);

	// 백돌이면
	p_old_brush = (CBrush*)dc.SelectStockObject(WHITE_BRUSH);


	point.x = ((point.x + 35 / 2 ) / 35) * 35;//격 맞춤
	point.y = ((point.y + 35 / 2) / 35) * 35;
	dc.Ellipse(point.x - 35 / 2, point.y - 35 / 2, point.x + 35 / 2, point.y + 35 / 2);
	dc.SelectObject(p_old_brush);

	CDialogEx::OnLButtonDown(nFlags, point);
}

void COmokServerDlg::OnBnClickedButtonSend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	char pTmp[256];
	CString strTmp;
	memset(pTmp, '\0', 256);
	memcpy(pTmp, (unsigned char*)(LPCTSTR)m_strSend, 256);
	//	strcpy(pTmp, m_strSend);

	// 전송
	m_socCom->Send(pTmp, 256);
	SendGame(SOC_TEXT, m_strSend);
	// 전송한 데이터도 리스트박스에 보여준다
	strTmp.Format(_T("%s"), (LPCTSTR)pTmp);
	int i = m_list.GetCount();
	m_list.InsertString(i, strTmp);
}

