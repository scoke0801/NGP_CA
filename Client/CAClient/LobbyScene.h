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
	virtual void ProcessMouseInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){}
	virtual void ProcessKeyboardInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
	virtual void ProcessKeyboardDownInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	bool Player2_Exist;
	bool Player3_Exist;
	bool is_All_Ready;

	bool Player2_Ready = false;
	bool Player3_Ready = false;
	bool isGameStart;

	string chatData;

private:
	CImage m_Bg_Image;
	CImage m_Player2_Images[2];
	CImage m_Player3_Images[2];

};

