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

	// ������۸� ó���� ���� �����Դϴ�.
	HDC			m_hdc;
	HBITMAP		m_hbmp;

	// �ð� ó���� ���� �����Դϴ�.
	CGameTimer	m_GameTimer;

	_TCHAR		m_pszFrameRate[50];

private:
	void BuildScene();
	void InitBuffers();

public:
	CFramework();
	~CFramework();

	void init(HWND hWnd, HINSTANCE hInst);

public:
	void preUpdate(); 
	void ProcessInput();
	void update(float timeElapsed);
	void draw(HDC hdc);

	LRESULT ProcessWindowInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	
	template <typename SceneName>
	void ChangeScene() 
	{
		CScene *scene = new SceneName;
		scene->Init(m_rtClient, this);

		if (m_pCurScene)
		{
			delete m_pCurScene;
			m_pCurScene = nullptr;
		}

		m_pCurScene = scene;
	}
};

