#pragma once
#include "Scene.h"

class CTitleScene : public CScene
{
public:
	CTitleScene();
	~CTitleScene();

public:
	virtual void Update(float timeElapsed);
	virtual void Draw(HDC hdc);

	virtual void Communicate();

	virtual LRESULT ProcessWindowInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { return 0; }
	virtual void ProcessMouseInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
	virtual void ProcessKeyboardInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}

private:
	CImage m_Image;
};

