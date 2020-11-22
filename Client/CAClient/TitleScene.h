#pragma once
#include "Scene.h"
#include "Button.h"

class CSoundManager;


typedef struct AccountData {
	string id;
	string pw;
	bool isNew;
};

class CTitleScene : public CScene
{
public:
	CTitleScene();
	~CTitleScene();

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

	void LoadAccounts();
	void RegisterNewID();

private:
	CImage background;
	CImage boxLogin;
	CImage popup;

	Button btNewid;
	Button btLogin;
	Button btExit;

	CSoundManager* m_SoundManager;

	//set<AccountData*> accounts;
	map<string, string> accounts;

	AccountData player;
	string selected;
	string message;
};

