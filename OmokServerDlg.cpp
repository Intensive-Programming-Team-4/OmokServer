
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
public:
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
	//  DDX_Text(pDX, IDC_STATIC_TIMER, m_timer);
	DDX_Control(pDX, IDC_STATIC_TIMER, m_timer);
	DDX_Control(pDX, IDC_BUTTON_GIVEUP, m_giveup);
	DDX_Control(pDX, IDC_STATIC_BLACKSCORE, m_blackScore);
	DDX_Control(pDX, IDC_STATIC_WHITESCORE, m_whiteScore);
}

BEGIN_MESSAGE_MAP(COmokServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_SEND, &COmokServerDlg::OnBnClickedButtonSend)
	ON_MESSAGE(UM_ACCEPT, (LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM))OnAccept)
	ON_MESSAGE(UM_RECEIVE, (LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM))OnReceive)
	ON_BN_CLICKED(IDC_BUTTON_START, &COmokServerDlg::OnBnClickedButtonStart)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_GIVEUP, &COmokServerDlg::OnBnClickedButtonGiveup)
	ON_BN_CLICKED(IDC_BUTTON_UNDO, &COmokServerDlg::OnBnClickedButtonUndo)
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

	// 게임 초기화
	InitGame();

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
	DrawDol();
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR COmokServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void COmokServerDlg::InitGame()
{
	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 15; j++) {
			m_bGame[i][j] = FALSE;
			m_bStone[i][j] = FALSE;
		}
	}
	m_bStartCnt = FALSE;

	m_bStart = FALSE;
	m_bMe = FALSE;
	m_bSvrEnd = FALSE;
	m_bCntEnd = FALSE;
	m_iOrder = 1;
	SetTimer(1, 1000, NULL);
	vBlack.clear();
	vWhite.clear();
	change = FALSE;
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


	// 접속했으니 점속 값 변경
	m_bConnect = TRUE;
	SendGame(SOC_TEXT, "접속성공");

	m_strConnect = _T("접속성공");
	m_strStatus = _T("준비버튼을 눌러주세요.");
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
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
		// 게임이 시작됨
		if (m_bStart) {
			m_bStartCnt = TRUE;
			MessageBox(_T("게임이 시작되었습니다."));
			m_strMe = _T("당신의 차례입니다.");
			m_strStatus = _T("원하는 곳을 선택하세요.");
			m_bMe = TRUE;
			UpdateData(FALSE);
		}
	}

	// 게임 준비
	else if (iType == SOC_GAMEREADY) {
		m_bStartCnt = TRUE;
		if (m_bStart) {
			SendGame(SOC_GAMESTART, "");
			MessageBox(_T("게임이 시작되었습니다."));
			m_strMe = _T("당신의 차례입니다.");
			m_strStatus = _T("원하는 곳을 선택하세요.");
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
		CString i, j;

		int iRow = atoi(str.Left(2));
		int iCol = atoi(str.Right(2));

		if (iRow == -1 && iCol == -1) {
			// 타이머의 시간초과로 넘어온 경우
		}
		else {
			// 바둑알 놓기
			CClientDC dc(this);
			CBrush* p_old_brush;

			// 클라이언트 (백돌)
//			p_old_brush = (CBrush*)dc.SelectStockObject(WHITE_BRUSH);
			p gameP;
			gameP.row = iRow;
			gameP.col = iCol;

			iCol = (iCol + 1) * 35;
			iRow = (iRow + 1) * 35;

			gameP.x1 = iCol - 35 / 2;
			gameP.y1 = iRow - 35 / 2;
			gameP.x2 = iCol + 35 / 2;
			gameP.y2 = iRow + 35 / 2;
			vWhite.push_back(gameP);

//			dc.Ellipse(iCol - 35 / 2, iRow - 35 / 2, iCol + 35 / 2, iRow + 35 / 2);
//			dc.SelectObject(p_old_brush);

			// 차례 변경
			m_bMe = TRUE;
			m_strMe = _T("당신의 차례입니다.");
			m_strStatus = _T("원하는 곳을 선택 하세요.");
			change = FALSE;
			UpdateData(FALSE);
		}
	}

	// 게임에서 패배할 시 혹은 기권할 시
	else if (iType == SOC_GAMEEND) {
		m_bCntEnd = TRUE;
		CWnd::MessageBox("백이 승리했습니다. 새 게임을 시작합니다.", "백돌 승리", MB_OK);
		Sleep(1000);
		InitGame();
		Invalidate(TRUE);
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);

		CString score;
		score.Format(_T("%d"), ++whitescore);
		m_whiteScore.SetWindowText(score);
	}
	else if (iType == SOC_GIVEUP) {
		m_bSvrEnd = TRUE;
		CWnd::MessageBox("흑이 승리했습니다. 새 게임을 시작합니다.", "흑돌 승리", MB_OK);
		Sleep(1000);
		InitGame();
		Invalidate(TRUE);
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);

		CString score;
		score.Format(_T("%d"), ++blackscore);
		m_blackScore.SetWindowText(score);
	}
	else if (iType == SOC_UNDO) {
		m_bGame[vWhite.back().row][vWhite.back().col] = FALSE;
		m_bStone[vWhite.back().row][vWhite.back().col] = FALSE;
		vWhite.pop_back();
	}
	Invalidate(FALSE);

	return TRUE;
}


// 사각형 그리기 (250 *250 시작은 (35, 35))
void COmokServerDlg::DrawRec() {

	CClientDC dc(this);
	CBrush br;
	br.CreateSolidBrush(RGB(218, 164, 43));

	CBrush* bbr = dc.SelectObject(&br);
	dc.Rectangle(10, 10, 60 + 525, 60 + 525);
	dc.SelectObject(bbr);

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
	if (point.x > 560 || point.y > 560)	return;
	if (point.x < 35 || point.y < 35)	return;
	if (!m_bConnect)	return;

	if (m_bStart && m_bStartCnt && m_bMe) {
		// 바둑알 놓기
		CClientDC dc(this);
		CBrush* p_old_brush;

		// 흑돌(서버)
//		p_old_brush = (CBrush*)dc.SelectStockObject(BLACK_BRUSH);

		point.x = ((point.x + 35 / 2) / 35) * 35;//격 맞춤
		point.y = ((point.y + 35 / 2) / 35) * 35;

		int nCol = point.x / 35 - 1;
		int nRow = point.y / 35 - 1;

		// 바둑알이 놓인 곳이 아니면
		if (!m_bGame[nRow][nCol]) {

			int Win = 0;

			m_bGame[nRow][nCol] = TRUE;
			m_bStone[nRow][nCol] = TRUE;

			p gameP;
			gameP.x1 = point.x - 35 / 2;
			gameP.y1 = point.y - 35 / 2;
			gameP.x2 = point.x + 35 / 2;
			gameP.y2 = point.y + 35 / 2;
			gameP.row = nRow;
			gameP.col = nCol;
			vBlack.push_back(gameP);

			CString str;
			str.Format(_T("%02d,%02d"), nRow, nCol);
			SendGame(SOC_CHECK, str);

			//무르기 저장
//			int BackPoint_x, BackPoint_y;
//			BackPoint_x = nCol;
//			BackPoint_y = nRow;


			register int i;
			int nCount = 0;

			// 가로 방향 승리 판정
			for (i = 0; i < 19; i++) {
				if (m_bStone[nRow][i] == 1)
					nCount++;
				else
					nCount = 0;

				if (nCount == 5) { Win = 1; }
			}

			// 세로 방향 승리 판정
			nCount = 0;
			for (i = 0; i < 19; i++)
			{
				if (m_bStone[i][nCol] == 1) { nCount++; }
				else { nCount = 0; }

				if (nCount == 5) { Win = 1; }
			}

			////// 대각선 방향 승리 판정

			int sRow, sCol; // 왼쪽 위의 시작 위치
			int eRow, eCol; // 오른쪽 아래의 끝 위치
			int RowMover, ColMover;

			RowMover = nRow;
			ColMover = nCol;
			while (RowMover != 0 && ColMover != 0)
			{
				RowMover--;
				ColMover--;
			}

			// 대각 방향 최-좌상단 좌표 획득
			sRow = RowMover;
			sCol = ColMover;

			RowMover = nRow;
			ColMover = nCol;
			while (RowMover != 16 && ColMover != 16) {
				RowMover++;
				ColMover++;
			}

			// 대각 방향 최-우하단 좌표 획득
			eRow = RowMover;
			eCol = ColMover;

			// 왼쪽 위에서 오른쪽 아래로 향하는 대각선 방향의 오목을 검색
			nCount = 0;
			while (sCol <= eCol && sRow <= eRow)
			{
				if (m_bStone[sRow][sCol] == 1)
				{
					nCount++;
				}
				else
				{
					nCount = 0;
				}

				if (nCount == 5) { Win = 1; }
				sRow++;
				sCol++;
			}

			RowMover = nRow;
			ColMover = nCol;
			while (RowMover != 0 && ColMover != 16) {
				RowMover--;
				ColMover++;
			}

			// 대각 방향 최-우상단 좌표 획득
			sRow = RowMover;
			sCol = ColMover;

			RowMover = nRow;
			ColMover = nCol;
			while (RowMover != 16 && ColMover != 0) {
				RowMover++;
				ColMover--;
			}

			// 대각 방향 최-좌하단 좌표 획득
			eRow = RowMover;
			eCol = ColMover;

			// 왼쪽 아래에서 오른쪽 위로 향하는 대각선 방향의 오목을 검색
			nCount = 0;
			while (sCol >= eCol && sRow <= eRow)
			{
				if (m_bStone[sRow][sCol] == 1)
				{
					nCount++;
				}
				else
				{
					nCount = 0;
				}

				if (nCount == 5) { Win = 1; }
				sRow++;
				sCol--;
			}


			if (Win == 1)
			{
				m_bSvrEnd = TRUE;
				SendGame(SOC_GAMEEND, "");
				CWnd::MessageBox("흑이 승리했습니다. 새 게임을 시작합니다.", "흑돌 승리", MB_OK);
				Sleep(1000);
				InitGame();
				Invalidate(TRUE);
				GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);

				CString score;
				score.Format(_T("%d"), ++blackscore);
				m_blackScore.SetWindowText(score);
			}

			// 차례 변경
			m_bMe = FALSE;
			m_strMe = _T("상대방의 차례 입니다.");
			m_strStatus = _T("대기하세요.");
			UpdateData(FALSE);
		}
	}
	Invalidate(FALSE);
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


// 게임 준비를 눌렀을 경우
void COmokServerDlg::OnBnClickedButtonStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SendGame(SOC_GAMEREADY, "");

	m_bStart = TRUE;
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
}

void COmokServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CString time;
	
	switch (nIDEvent) {
	case 1:
		if (m_bStart && m_bMe) {
			if (sec > 0) {
				time.Format(_T("%d"), sec--);
			}
			else {
				m_bMe = FALSE;

				CString str;
				str.Format(_T("%02d,%02d"), -1, -1);
				SendGame(SOC_CHECK, str);
			}
		}
		else {
			sec = 30;
			time.Format(_T("%d"), sec);
		}
		m_timer.SetWindowText(time);
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


void COmokServerDlg::OnBnClickedButtonGiveup()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_bStart) {
		m_bCntEnd = TRUE;
		SendGame(SOC_GIVEUP, "");
		CWnd::MessageBox("백이 승리했습니다. 새 게임을 시작합니다.", "백돌 승리", MB_OK);
		Sleep(1000);
		InitGame();
		Invalidate(TRUE);
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);

		CString score;
		score.Format(_T("%d"), ++whitescore);
		m_whiteScore.SetWindowText(score);
	}
}

void COmokServerDlg::OnBnClickedButtonUndo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (vBlack.size() == 0)
		return;
	if (m_bStart && m_bMe && !change) {
		vBlack.pop_back();
		change = TRUE;
		SendGame(SOC_UNDO, "");
		Invalidate(TRUE);
	}
}


void COmokServerDlg::DrawDol()
{
	// TODO: 여기에 구현 코드 추가.
	CClientDC dc(this);
	CBrush* p_old_brush;
	for (iter = vBlack.begin(); iter != vBlack.end(); iter++) {
		p_old_brush = (CBrush*)dc.SelectStockObject(BLACK_BRUSH);
		dc.Ellipse((*iter).x1, (*iter).y1, (*iter).x2, (*iter).y2);
		dc.SelectObject(p_old_brush);
	}
	for (iter = vWhite.begin(); iter != vWhite.end(); iter++) {
		p_old_brush = (CBrush*)dc.SelectStockObject(WHITE_BRUSH);
		dc.Ellipse((*iter).x1, (*iter).y1, (*iter).x2, (*iter).y2);
		dc.SelectObject(p_old_brush);
	}
}
