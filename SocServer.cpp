// SocServer.cpp: 구현 파일
//

#include "pch.h"
#include "OmokServer.h"
#include "SocServer.h"
#include <afxsock.h>

// CSocServer

CSocServer::CSocServer()
{
}

CSocServer::~CSocServer()
{
}

// CSocServer 멤버 함수

void CSocServer::Init(HWND hWnd) {
	//메인 윈도우 포인터 받기
	m_hWnd = hWnd;
}

void CSocServer::OnAccept(int nErrorCode) {
	// 클라이언트가 접속하면 메인 윈도우에 접속 메시지(UM_ACCEPT)를 보낸다
	Accept(m_socCom);
	SendMessage(m_hWnd, UM_ACCEPT, 0, 0);
	CSocket::OnAccept(nErrorCode);
}

CSocCom* CSocServer::GetAcceptSocCom() {
	// 통신소켓을 반환한다
	// 반환되는 통신 소켓은 클라이언트와 연결된다.
	return &m_socCom;
}
