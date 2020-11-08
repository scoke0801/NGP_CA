#pragma once
#include "Scene.h"

class CPlayer;

class CGameScene : public CScene
{
private:
	vector<CPlayer*>	m_Players;

public:
	CGameScene();
	~CGameScene();

public:
	virtual void Update(float timeElapsed);
	virtual void Draw(HDC hdc);

	virtual void Communicate();

	virtual LRESULT ProcessWindowInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { return 0; }
	virtual void ProcessMouseInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
	virtual void ProcessKeyboardInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
};

