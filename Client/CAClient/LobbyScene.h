#pragma once
#include "Scene.h"

class CFramework;

class CLobbyScene : public CScene
{
public:
	CLobbyScene();
	~CLobbyScene();

	virtual void SendDataToNextScene(void* pContext) {}
public:
	virtual void Update(float timeElapsed);
	virtual void Draw(HDC hdc);

	virtual void Communicate(SOCKET& sock);

	virtual LRESULT ProcessWindowInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { return 0; }

	virtual void ProcessMouseInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessKeyboardUpInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}

	virtual void ProcessKeyboardDownInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	bool Player2_Exist;
	bool Player3_Exist;
	bool is_All_Ready;

	bool Player2_Ready = false;
	bool Player3_Ready = false;
	bool isGameStart;

	string chatData;

	CFramework *m_Frame;

private:
	CImage m_Bg_Image;
	CImage m_Player2_Images[2];
	CImage m_Player3_Images[2];

	string is_Select;
	string m_Message;

	char buf[BUFSIZ];
	
	TCHAR* StringToTCHAR(string& s);
};

