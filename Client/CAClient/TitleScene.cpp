#include "stdafx.h"
#include "TitleScene.h"

CTitleScene::CTitleScene()
{
	background.Load(_T("assets/login_scene_bg.bmp"));
	login.Load(_T("assets/login_scene_login.png"));
	popup.Load(_T("assets/login_scene_alert.png"));

	id[0] = '\0';
	pw[0] = '\0';
	selected = "ID";

	m_Type = SceneType::TitleScene;
}

CTitleScene::~CTitleScene()
{

}

void CTitleScene::Update(float timeElapsed)
{

}

void CTitleScene::Draw(HDC hdc)
{
	SetBkMode(hdc, TRANSPARENT);

	background.StretchBlt(hdc, 0, 0, m_rtClient.right, m_rtClient.bottom,
		0, 0, background.GetWidth(), background.GetHeight());

	login.TransparentBlt(
		hdc, (CLIENT_WIDTH - login.GetWidth()) / 2, (CLIENT_HEIGHT - login.GetHeight()) / 2 + 150,
		login.GetWidth(), login.GetHeight(),
		0, 0, login.GetWidth(), login.GetHeight(),
		RGB(255, 0, 255));

	popup.TransparentBlt(
		hdc, (CLIENT_WIDTH - popup.GetWidth()) / 2, (CLIENT_HEIGHT - popup.GetHeight()) / 2 - 100,
		popup.GetWidth(), popup.GetHeight(),
		0, 0, popup.GetWidth(), popup.GetHeight(),
		RGB(255, 0, 255));



	TextOut(hdc, 500, 538, id, lstrlen(id));
	TextOut(hdc, 500, 563, pw, lstrlen(pw));
}

void CTitleScene::Communicate(SOCKET& sock)
{

}

void CTitleScene::ProcessMouseInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int mx = LOWORD(lParam);
	int my = HIWORD(lParam);

	// 버튼 자르고 교체
	if (mx > 396 && mx < 490 && my > 536 && my < 558)
		selected = "ID";
	if (mx > 396 && mx < 490 && my > 563 && my < 585)
		selected = "PW";
}

void CTitleScene::ProcessCHARInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (selected == "ID") {
		if ((wParam == VK_BACK) && id[0] != '\0') id[lstrlen(id) - 1] = '\0';
		else if ((wParam == VK_BACK) && id[0] == '\0');
		else {
			if (lstrlen(id) != 18) 
				id[lstrlen(id)] = wParam;
		}
	}
	if (selected == "PW") {
		if ((wParam == VK_BACK) && pw[0] != '\0') pw[lstrlen(pw) - 1] = '\0';
		else if ((wParam == VK_BACK) && pw[0] == '\0');
		else {
			if (lstrlen(pw) != 18)
				pw[lstrlen(pw)] = wParam;
		}
	}

}