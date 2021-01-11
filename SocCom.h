#pragma once
#include <afxsock.h>

// CSocCom 명령 대상

class CSocCom : public CSocket
{
public:
	CSocCom();
	virtual ~CSocCom();
	HWND m_hWnd;
	void Init(HWND hWnd);
	virtual void OnReceive(int nErrorCode);
};


