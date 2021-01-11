#pragma once

// CSocServer 명령 대상

class CSocServer : public CSocket
{
public:
	CSocServer();
	virtual ~CSocServer();

	// 메인 윈도우 포인터 받기
	HWND m_hWnd;
	CSocCom m_socCom;
	CSocCom *GetAcceptSocCom();
	void Init(HWND hWnd);
	virtual void OnAccept(int nErrorCode);
};


