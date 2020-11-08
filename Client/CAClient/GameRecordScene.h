#pragma once
#include "Scene.h"

typedef struct GameRecordData {
	TCHAR id[20];
	int itemScore;
	int survivedScore;
}GameScore;

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
	virtual void ProcessKeyboardUpInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
	virtual void ProcessKeyboardDownInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}

private:
	CImage background;
	GameScore players[3];
	

};