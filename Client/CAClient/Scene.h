#pragma once

#include "GameFramework.h"

class CScene
{
private:
	CFramework* m_pFramework;

protected:
	RECT m_rtClient;

protected:
	template<class SceneName>
	void ChangeScene() { m_pFramework->ChangeScene<SceneName>(); }

public:
	CScene();
	virtual ~CScene();

	void Init(RECT rt, CFramework* framework) { m_rtClient = rt, m_pFramework = framework; }

public:
	virtual void Update(float timeElapsed) = 0;
	virtual void Draw(HDC hdc) = 0;

	virtual void Communicate() = 0;

	virtual LRESULT ProcessWindowInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { return 0; }
	virtual void ProcessMouseInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
	virtual void ProcessKeyboardUpInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
	virtual void ProcessKeyboardDownInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
};
