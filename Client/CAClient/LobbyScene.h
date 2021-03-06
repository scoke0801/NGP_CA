#pragma once
#include "Scene.h"
#include "Datas.h"
#include "Player.h"

class CFramework;
class CSoundManager;

class CLobbyScene : public CScene
{
public:
	CLobbyScene();
	~CLobbyScene();

public:
	virtual void Update(float timeElapsed);
	virtual void Draw(HDC hdc);

	virtual void SendDataToNextScene(void* pContext);

	virtual void Communicate(SOCKET& sock);
	
	virtual LRESULT ProcessWindowInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { return 0; }

	virtual void ProcessMouseClick(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessMouseInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessKeyboardUpInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}

	virtual void ProcessCHARInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessKeyboardDownInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	bool Player2_Exist;
	bool Player3_Exist;
	
	bool Player2_Ready = false;
	bool Player3_Ready = false;
	bool isGameStart;
private:
	int m_ClientIdx = 0;

	CImage m_Bg_Image[2];
	CImage m_Player2_Images[4];
	CImage m_Player3_Images[4];

	LobbySceneSendData Send_Player_Data[4];
	LobbySceneSendData m_Player;
	LobbySceneRecvData Recv_Player_Data[4];

	CSoundManager* m_SoundManager;

	string is_Select[3];
	
	char buf[BUFSIZ];
	
	TCHAR* StringToTCHAR(string& s);
};

