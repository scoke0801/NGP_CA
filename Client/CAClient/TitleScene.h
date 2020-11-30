#pragma once
#include "Scene.h"
#include "Datas.h"
#include "Button.h"

class CSoundManager;

class CTitleScene : public CScene
{
public:
	CTitleScene();
	~CTitleScene();

	virtual void SendDataToNextScene(void* pContext) {}

public:
	virtual void Update(float timeElapsed);
	virtual void Draw(HDC hdc);

	virtual void Communicate(SOCKET& sock);

	virtual LRESULT ProcessWindowInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { return 0; }
	virtual void ProcessMouseClick(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessMouseInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {};
	virtual void ProcessKeyboardUpInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
	virtual void ProcessKeyboardDownInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
	virtual void ProcessCHARInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	CSoundManager* m_SoundManager;

	CImage background;
	CImage boxLogin;
	CImage popup;

	Button btNewid;
	Button btLogin;
	Button btExit;

	TitleSceneSendData sendData;
	TitleSceneRecvData recvData;
	TitleToLobbySceneData nextscenedata;

	string selected;
	string textmessage;

	bool communicate;
};

