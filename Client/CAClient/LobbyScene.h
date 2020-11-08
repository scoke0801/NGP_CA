#pragma once
#include "Scene.h"

class CLobbyScene : public CScene
{
public:
	CLobbyScene();
	~CLobbyScene();

public:
	virtual void Update(float timeElapsed);
	virtual void Draw(HDC hdc);

	virtual void Communicate();

	virtual LRESULT ProcessWindowInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { return 0; }
	virtual void ProcessMouseInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
	virtual void ProcessKeyboardUpInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
	virtual void ProcessKeyboardDownInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
};

