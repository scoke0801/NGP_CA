#pragma once
#include "Scene.h"
#include "Datas.h"

class CFramework;

class CLobbyScene : public CScene
{
public:
	CLobbyScene();
	~CLobbyScene();

public:
	virtual void Update(float timeElapsed);
	virtual void Draw(HDC hdc);

	virtual void Communicate(SOCKET& sock);
	

	virtual LRESULT ProcessWindowInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { return 0; }

	virtual void ProcessMouseClick(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessMouseInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessKeyboardUpInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}

	virtual void ProcessCHARInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessKeyboardDownInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	bool Player2_Exist;
	bool Player3_Exist;
	bool is_All_Ready;
	int Character_Change[3];

	bool Player2_Ready = false;
	bool Player3_Ready = false;
	bool isGameStart;
	

	CFramework *m_Frame;

private:
	CImage m_Bg_Image[2];
	CImage m_Player2_Images[4];
	CImage m_Player3_Images[4];

	LobbySceneSendData m_Player[4];
	
	string is_Select[3];
	string is_Character_Select[3];

	char buf[BUFSIZ];
	
	TCHAR* StringToTCHAR(string& s);
};

