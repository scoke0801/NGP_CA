#pragma once

class CScene;

class CFramework
{
private:
	HWND m_hWnd;
	HINSTANCE m_hInst;

	RECT m_rtClient;

	CScene* m_pCurScene;

	// ������۸� ó���� ���� �����Դϴ�.
<<<<<<< Updated upstream
	HDC m_hdc;
	HBITMAP m_hbmp;
=======
	HDC			m_hdc;
	HBITMAP		m_hbmp;
	
>>>>>>> Stashed changes

	// �ð� ó���� ���� �����Դϴ�.
	chrono::system_clock::time_point m_prevTime;
	std::chrono::duration<double> m_timeElapsed;
	double m_timeGap;

	// �������� Ÿ��Ʋ�� ǥ���ϱ� ���� �����Դϴ�.
	TCHAR m_captionTitle[50];
	int m_titleLength;
	std::chrono::system_clock::time_point m_lastUpdateTime;
	std::chrono::duration<double> m_updateElapsed; 
	double m_updateGap;
	double m_fps;
	USHORT m_frameCount;

private:
<<<<<<< Updated upstream
=======
	static CFramework* self;
	
>>>>>>> Stashed changes
	void BuildScene();
	void InitBuffers();

public:
	CFramework();
	~CFramework();

	void init(HWND hWnd, HINSTANCE hInst);

	int Thread_Num;

public:
	void preUpdate();
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

