#pragma once
#include "Scene.h"

class CGameRecordScene : public CScene
{
public:
	CGameRecordScene();
	~CGameRecordScene();

public:
	virtual void Update(float timeElapsed);
	virtual void Draw(HDC hdc);

	virtual void Communicate();

	virtual LRESULT ProcessWindowInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { return 0; }
	virtual void ProcessMouseInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
	virtual void ProcessKeyboardInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
};

