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
//�������� ���������� �ִ� ��� ������ �� �������� �����մϴ�.
#define MAX_LOOP_TIME 50

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
CFramework* CFramework::self = nullptr;
CFramework::CFramework() 
{  
}

CFramework::~CFramework()
{
	if (m_Sock)
	{ 
		closesocket(m_Sock);
	}
	if (m_pCurScene)
	{
		delete m_pCurScene;
		m_pCurScene = nullptr;
	}  
	 
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
	LoadString(m_hInst, IDS_APP_TITLE, m_captionTitle, TITLE_LENGTH);
#if defined(SHOW_CAPTIONFPS)
	lstrcat(m_captionTitle, TEXT(" ("));
#endif
	m_titleLength = lstrlen(m_captionTitle);
	SetWindowText(m_hWnd, m_captionTitle); 
}
 
bool CFramework::PrepareCommunicate()
{
	//CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

	int retval = 0;
	// ���� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &m_WSA) != 0) return false;

	// set serveraddr
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);

	m_IsServerConnected = true;

	// socket()
	m_Sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_Sock == INVALID_SOCKET) 
	{
		m_IsServerConnected = false;
		// for testing 
		// error_quit("socket()");
		return false;
	}
	int opt_val = TRUE;
	setsockopt(m_Sock, IPPROTO_TCP, TCP_NODELAY, (char*)&opt_val, sizeof(opt_val));
	//int optval = 0;
	//setsockopt(m_Sock, SOL_SOCKET, SO_SNDBUF, (char*)&optval, sizeof(optval));
	// connect()
	retval = connect(m_Sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
	{
		m_IsServerConnected = false;
		// for testing 
		// error_quit("connect()");
		return false;
	}
	

	return true;
}

void CFramework::Communicate()
{
	if(m_pCurScene)  m_pCurScene->Communicate(m_Sock);
}

void CFramework::BuildScene()
{
	m_pCurScene = nullptr;

	ChangeScene<CNullScene>();
	//ChangeScene<CTitleScene>(); 
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
	//if (!m_IsServerConnected)
	//{}
	m_timeElapsed = std::chrono::system_clock::now() - m_currentTime;//����ð��� �����ð��� ���ؼ�
	m_dLag = 0.0;

	ProcessInput();

	if (m_timeElapsed.count() > FPS)				//������ �ð��� �귶�ٸ�
	{
		m_currentTime = std::chrono::system_clock::now();//����ð� ����

		if (m_timeElapsed.count() > 0.0) m_fps = 1.0 / m_timeElapsed.count();

		//���� �ð��� �ʾ��� ��� �̸� �������� �� ���� ������Ʈ ��ŵ�ϴ�.
		m_dLag += m_timeElapsed.count();
		for (int i = 0; m_dLag > FPS && i < MAX_LOOP_TIME; ++i)
		{
			//Communicate();
			update(FPS);
			m_dLag -= FPS;
		}
	}
	// �ִ� FPS �̸��� �ð��� ����ϸ� ���� ����(Frame Per Second)
	else
		return;

	// ������Ʈ�� ����Ǹ� �������� �����մϴ�.
	InitBuffers();
	InvalidateRect(m_hWnd, &m_rtClient, FALSE);

#if defined(SHOW_CAPTIONFPS)

	m_updateElapsed = std::chrono::system_clock::now() - m_lastUpdateTime;
	if (m_updateElapsed.count() > MAX_UPDATE_FPS)
		m_lastUpdateTime = std::chrono::system_clock::now();
	else
		return;

	_itow_s(m_fps + 0.1f, m_captionTitle + m_titleLength, TITLE_LENGTH - m_titleLength, 10);
	wcscat_s(m_captionTitle + m_titleLength, TITLE_LENGTH - m_titleLength, TEXT(" FPS)"));
	SetWindowText(m_hWnd, m_captionTitle);

#endif
}

void CFramework::ProcessInput()
{
	static UCHAR pKeysBuffer[256];
	bool bProcessedByScene = false;
	if (m_pCurScene == nullptr) return;
	if (GetKeyboardState(pKeysBuffer))//&& m_pCurScene)
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
		m_pCurScene->ProcessMouseClick(hWnd, message, wParam, lParam);
		break;
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
	
	if (!CFramework::GetInstance()->PrepareCommunicate()) return 0;
	
	std::chrono::system_clock::time_point currentTime;
	std::chrono::duration<double> timeElapsed; // �ð��� �󸶳� ������
	double dLag = 0.0f;
	double fps  = 0.0f;

	std::chrono::system_clock::time_point lastUpdateTime;
	std::chrono::duration<double> updateElapsed;

	while (1)
	{
		timeElapsed = std::chrono::system_clock::now() - currentTime;//����ð��� �����ð��� ���ؼ�
		dLag = 0.0;

		CFramework::GetInstance()->ProcessInput();

		if ( timeElapsed.count() > FPS)				//������ �ð��� �귶�ٸ�
		{
			currentTime = std::chrono::system_clock::now();//����ð� ����

			if (timeElapsed.count() > 0.0) fps = 1.0 / timeElapsed.count();

			//���� �ð��� �ʾ��� ��� �̸� �������� �� ���� ������Ʈ ��ŵ�ϴ�.
			dLag += timeElapsed.count();
			for (int i = 0; dLag > FPS && i < MAX_LOOP_TIME; ++i)
			{
				//CFramework::GetInstance()->preUpdate();
				CFramework::GetInstance()->Communicate();
				dLag -= FPS;
			}
		}
		// �ִ� FPS �̸��� �ð��� ����ϸ� ���� ����(Frame Per Second)
		else
			continue;

#if defined(SHOW_CAPTIONFPS)

		updateElapsed = std::chrono::system_clock::now() - CFramework::GetInstance()->m_lastUpdateTime;
		if (updateElapsed.count() > MAX_UPDATE_FPS)
			lastUpdateTime = std::chrono::system_clock::now();
		else
			continue;

		_itow_s( fps + 0.1f,
			CFramework::GetInstance()->m_captionTitle + CFramework::GetInstance()->m_titleLength,
			TITLE_LENGTH - CFramework::GetInstance()->m_titleLength, 10);
		wcscat_s(CFramework::GetInstance()->m_captionTitle + CFramework::GetInstance()->m_titleLength, TITLE_LENGTH - CFramework::GetInstance()->m_titleLength, TEXT(" FPS)"));
		SetWindowText(CFramework::GetInstance()->m_hWnd, CFramework::GetInstance()->m_captionTitle);
#endif
	}

	return 0;
}

//DWORD __stdcall ClientMain(LPVOID arg)
//{
//	if (!CFramework::GetInstance()->PrepareCommunicate()) return 0; 
//	while (1)
//	{
//		 
//		CFramework::GetInstance()->ProcessInput();
//
//		CFramework::GetInstance()->Communicate();
//			 
//	}
//
//	return 0;
//}