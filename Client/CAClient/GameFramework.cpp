#include "stdafx.h"
#include "Resource.h"
#include "GameFramework.h"
#include "GameScene.h"
#include "LobbyScene.h"

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
}

void CFramework::init(HWND hWnd, HINSTANCE hInst)
{
	m_hWnd = hWnd;
	m_hInst = hInst;
	
	::GetClientRect(hWnd, &m_rtClient);

	m_prevTime = m_lastUpdateTime = chrono::system_clock::now();

	m_frameCount = 0;
	m_fps = m_timeGap = m_updateGap = 0;

	LoadString(m_hInst, IDS_APP_TITLE, m_captionTitle, TITLE_LENGTH);
#if defined(SHOW_CAPTIONFPS)
	lstrcat(m_captionTitle, TEXT(" ("));
#endif
	m_titleLength = lstrlen(m_captionTitle);
	SetWindowText(m_hWnd, m_captionTitle);

	BuildScene();
	InitBuffers();
}

void CFramework::BuildScene()
{
	m_pCurScene = nullptr;

	ChangeScene<CLobbyScene>();

	//ChangeScene<CGameScene>();
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
	int loopCount = 0;

	// ������ �Լ� ȣ����� �̹� �Լ� ȣ������� �ð� ����� ����մϴ�.
	m_timeElapsed = std::chrono::system_clock::now() - m_prevTime;
	m_prevTime = chrono::system_clock::now();
	m_timeGap += m_timeElapsed.count();

	// ����� �ð��� ���̰� ������ �ð����� �۰� �� ������ ������Ʈ�� �ݺ��մϴ�.
	// ��, �ð��� �����ϰ� �귯 ������Ʈ�� ����ؼ� �ݺ��Ǵ� ���� ���� ����
	// ���ѵ� Ƚ����ŭ�� �ݺ��� �� �ֵ��� ������ �����մϴ�.
	while (m_timeGap > FPS && loopCount++ < MAX_GAME_LOOP)
	{
		update(m_timeGap);

		m_timeGap -= FPS;
	}

	// ������Ʈ�� ����Ǹ� �������� �����մϴ�.
	InitBuffers();
	InvalidateRect(m_hWnd, &m_rtClient, FALSE);

	//Ÿ��Ʋ ��(ĸ��) ����
#if defined(SHOW_CAPTIONFPS)
	m_updateElapsed = std::chrono::system_clock::now() - m_lastUpdateTime;
	m_updateGap = m_updateElapsed.count();

	// ������ �ð����� fps�� ����մϴ�.
	if (m_updateGap > MAX_UPDATE_FPS)
	{
		m_fps = m_frameCount / m_updateGap;
		m_lastUpdateTime = std::chrono::system_clock::now();

		m_updateGap = 0;
		m_frameCount = 0;
	}
	else
		++m_frameCount;
	
	_itow_s(m_fps + 0.1f, m_captionTitle + m_titleLength, TITLE_LENGTH - m_titleLength, 10);
	wcscat_s(m_captionTitle + m_titleLength, TITLE_LENGTH - m_titleLength, TEXT(" FPS)"));
	SetWindowText(m_hWnd, m_captionTitle);
#endif
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
		m_pCurScene->ProcessMouseInput(hWnd, message, wParam, lParam);
	case WM_LBUTTONDOWN:
		m_pCurScene->ProcessMouseInput(hWnd, message, wParam, lParam);
	case WM_NCLBUTTONUP:
	case WM_RBUTTONDOWN:
		m_pCurScene->ProcessMouseInput(hWnd, message, wParam, lParam);
	case WM_RBUTTONUP:
		m_pCurScene->ProcessMouseInput(hWnd, message, wParam, lParam);
		break;

	case WM_KEYDOWN:
		m_pCurScene->ProcessKeyboardDownInput(hWnd, message, wParam, lParam);
		break;
	
	case WM_KEYUP:
		m_pCurScene->ProcessKeyboardUpInput(hWnd, message, wParam, lParam);
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
