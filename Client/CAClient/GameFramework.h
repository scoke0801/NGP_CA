#pragma once
#include "Timer.h"
class CScene;

class CFramework
{
private:
	HWND m_hWnd;
	HINSTANCE m_hInst;

	RECT m_rtClient;

	CScene* m_pCurScene;

	// 더블버퍼링 처리를 위한 변수입니다.
	HDC			m_hdc;
	HBITMAP		m_hbmp;

	// 시간 처리를 위한 변수입니다.
	CGameTimer	m_GameTimer;

	_TCHAR		m_pszFrameRate[50];

	CRITICAL_SECTION m_cs;

private:	// 서버와 통신하기 위한 데이터 입니다.
	WSADATA m_WSA;
	SOCKET m_Sock;
	SOCKADDR m_ServerAddr;

	bool m_IsServerConnected;

private:
	static CFramework* self;

	void BuildScene();
	void InitBuffers();

	CFramework();
	~CFramework();

	CFramework(const CFramework&) = delete;
	CFramework operator=(const CFramework&) = delete;
public:
	static CFramework* GetInstance()
	{
		static CFramework self;
		return &self;
	}

	void init(HWND hWnd, HINSTANCE hInst);

public:
	bool PrepareCommunicate();
	void Communicate();
	friend DWORD WINAPI ClientMain(LPVOID arg);

public:
	void preUpdate(); 
	void ProcessInput();
	void update(float timeElapsed);
	void draw(HDC hdc);

	LRESULT ProcessWindowInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	
	template <typename SceneName>
	void ChangeScene() 
	{
		//EnterCriticalSection(&m_cs);
		CScene *scene = new SceneName;
		static CScene* prevScene;
		scene->Init(m_rtClient, this);

		if (m_pCurScene)
		{
			prevScene = m_pCurScene;
			//delete m_pCurScene;
			m_pCurScene = nullptr;
		}

		m_pCurScene = scene;
		//LeaveCriticalSection(&m_cs);
	}
};

DWORD WINAPI ClientMain(LPVOID arg);
