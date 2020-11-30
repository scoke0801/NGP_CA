#pragma once
#include "Scene.h"
#include "Datas.h"

class CGameRecordScene : public CScene
{
public:
	CGameRecordScene();
	~CGameRecordScene();

	virtual void SendDataToNextScene(void* pContext);

public:
	virtual void Update(float timeElapsed);
	virtual void Draw(HDC hdc);

	virtual void Communicate(SOCKET& sock);

	virtual LRESULT ProcessWindowInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { return 0; }
	virtual void ProcessMouseInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
	virtual void ProcessKeyboardUpInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
	virtual void ProcessKeyboardDownInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}

private:
	CImage background;

	GameRecordSceneSendData sendData;
	vector<GameRecordSceneRecvData> scores;

	bool communicate;
};