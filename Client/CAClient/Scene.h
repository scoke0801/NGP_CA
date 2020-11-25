#pragma once

#include "GameFramework.h"
#include "Communicates.h"

enum class SceneType
{
	TitleScene=0,
	LobbyScene,
	GameScene,
	GameRecordScene
};

class CScene
{
private:
	CFramework* m_pFramework;

protected:
	RECT m_rtClient;
	SceneType m_Type;

protected:
	template<class SceneName>
	void ChangeScene() { m_pFramework->ChangeScene<SceneName>(); }

public:
	CScene();
	virtual ~CScene();

	void Init(RECT rt, CFramework* framework) { m_rtClient = rt, m_pFramework = framework; }

public:
	virtual void Update(float timeElapsed) = 0;
	virtual void Draw(HDC hdc) = 0;

	virtual void Communicate(SOCKET& sock) = 0;

	virtual bool ProcessInput(UCHAR* pKeysBuffer);

	virtual LRESULT ProcessWindowInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { return 0; }
	virtual void ProcessMouseClick(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
	virtual void ProcessMouseInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
	virtual void ProcessKeyboardUpInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
	virtual void ProcessKeyboardDownInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
	virtual void ProcessCHARInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
};


class CNullScene : public CScene
{
private:
	CImage m_Image;
public:
	CNullScene(); 

	virtual void Update(float timeElapsed) {}
	virtual void Draw(HDC hdc);

	virtual void Communicate(SOCKET& sock) {}

	virtual bool ProcessInput(UCHAR* pKeysBuffer) { return false; }

	virtual LRESULT ProcessWindowInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { return 0; }
	virtual void ProcessMouseInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
	virtual void ProcessKeyboardUpInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
	virtual void ProcessKeyboardDownInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessCHARInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
};