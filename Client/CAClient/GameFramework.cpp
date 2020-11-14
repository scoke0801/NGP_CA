#include "stdafx.h"
#include "Resource.h"
#include "GameFramework.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "GameRecordScene.h"
#include "LobbyScene.h"
#include "Communicates.h"

#define MAX_GAME_LOOP 30
#define FPS 1 / 60.0f

#define TITLE_LENGTH 50

// ĸ�� FPS ��� ���� -------------------
// �׻� ĸ�ǿ� FPS�� ���		(0 : ��Ȱ�� | 1 : Ȱ��)
#define USE_CAPTIONFPS_ALWAYS	 0

#if USE_CAPTIONFPS_ALWAYS
	#define SHOW_CAPTIONFPS 
#elif _DEBUG	// Debug������ �׻� ����
	#define SHOW_CAPTIONFPS 
#endif

#if defined(SHOW_CAPTIONFPS)
	#define MAX_UPDATE_FPS 1.0f / 5.0f
#endif

CFramework::CFramework()
{
}

CFramework::~CFramework()
{
	if (m_pCurScene)
	{
		delete m_pCurScene;
		m_pCurScene = nullptr;
	}

	if (m_Sock) closesocket(m_Sock);
	WSACleanup();
}

void CFramework::init(HWND hWnd, HINSTANCE hInst)
{
	m_hWnd = hWnd;
	m_hInst = hInst;
	
	::GetClientRect(hWnd, &m_rtClient);

	BuildScene();
	InitBuffers();

	m_IsServerConnected = false;
	 
	_tcscpy_s(m_pszFrameRate, _T("Crazy Arcade( "));
	m_GameTimer.Reset(); 
}

void CFramework::PrepareCommunicate()
{
	//CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

	int retval = 0;
	// ���� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &m_WSA) != 0) return;

	// set serveraddr
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);

	// socket()
	m_Sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_Sock == INVALID_SOCKET) 
	{
		m_IsServerConnected = false;
		// for testing 
		// error_quit("socket()");
	}

	// connect()
	retval = connect(m_Sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
	{
		m_IsServerConnected = false;
		// for testing 
		// error_quit("connect()");
	}
}

void CFramework::Communicate()
{
	if(m_pCurScene) m_pCurScene->Communicate();
}

void CFramework::BuildScene()
{
	m_pCurScene = nullptr;

	ChangeScene<CNullScene>();
	//ChangeScene<CTitleScene>();
	//ChangeScene<CGameScene>();
	//ChangeScene<CLobbyScene>();
}

void CFramework::InitBuffers()
{
	if (m_hdc) // ������ ����̽� ���׽�Ʈ�� �����Ǿ� �־��ٸ� �����մϴ�.
	{
		::SelectObject(m_hdc, nullptr);
		::DeleteObject(m_hdc);
	}
	if (m_hbmp) //������ ���۰� �����Ǿ� �־��ٸ� �����մϴ�.
	{
		::DeleteObject(m_hbmp);
	}

	HDC hdc = ::GetDC(m_hWnd);

	m_hdc = CreateCompatibleDC(hdc);
	m_hbmp = CreateCompatibleBitmap(hdc, m_rtClient.right, m_rtClient.bottom);
	::SelectObject(m_hdc, m_hbmp);
	::FillRect(m_hdc, &m_rtClient, (HBRUSH)WHITE_BRUSH);

	::ReleaseDC(m_hWnd, hdc);
}

void CFramework::preUpdate()
{
	m_GameTimer.Tick(0.0f);
	ProcessInput();

	update(m_GameTimer.GetTimeElapsed());
	if (m_IsServerConnected) m_pCurScene->Communicate();

	// ������Ʈ�� ����Ǹ� �������� �����մϴ�.
	InitBuffers();
	InvalidateRect(m_hWnd, &m_rtClient, FALSE);

	m_GameTimer.GetFrameRate(m_pszFrameRate + 14, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}

void CFramework::ProcessInput()
{
	static UCHAR pKeysBuffer[256];
	bool bProcessedByScene = false;
	if (GetKeyboardState(pKeysBuffer) && m_pCurScene)
		bProcessedByScene = m_pCurScene->ProcessInput(pKeysBuffer);

	if (!bProcessedByScene)
	{
	}
	
}

void CFramework::update(float timeElapsed)
{
	m_pCurScene->Update(timeElapsed);
}

void CFramework::draw(HDC hdc)
{
	m_pCurScene->Draw(m_hdc);
	BitBlt(hdc, 0, 0, m_rtClient.right, m_rtClient.bottom,
		m_hdc, 0,0, SRCCOPY);
}

LRESULT CFramework::ProcessWindowInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_NCLBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
		m_pCurScene->ProcessMouseInput(hWnd, message, wParam, lParam);
		break;

	case WM_KEYDOWN:
		m_pCurScene->ProcessKeyboardDownInput(hWnd, message, wParam, lParam);
		break;
	case WM_KEYUP:
		m_pCurScene->ProcessKeyboardUpInput(hWnd, message, wParam, lParam);
		break;

	case WM_CHAR:
		m_pCurScene->ProcessCHARInput(hWnd, message, wParam, lParam);
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		this->draw(hdc);

		EndPaint(hWnd, &ps);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

DWORD __stdcall ClientMain(LPVOID arg)
{
	CFramework* framework = reinterpret_cast<CFramework*>(arg);

	framework->PrepareCommunicate();
	
	while (1)
	{
		framework->Communicate();
	}
	return 0;
}
