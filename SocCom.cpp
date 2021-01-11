// SocCom.cpp: 구현 파일
//

#include "pch.h"
#include "OmokServer.h"
#include "SocCom.h"

// CSocCom

CSocCom::CSocCom()
{
}

CSocCom::~CSocCom()
{
}


// CSocCom 멤버 함수

// 메인 윈도우와 연결을 위한 함수
void CSocCom::Init(HWND hWnd) {
	//메인 윈도우 포인터 받기
	m_hWnd = hWnd;
}

// 데이터가 도착했다는걸 알리기 위한 함수
void CSocCom::OnReceive(int nErrorCode) {
	SendMessage(m_hWnd, UM_RECEIVE, 0, 0);
	CSocket::OnReceive(nErrorCode);
}
