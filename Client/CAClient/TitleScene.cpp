#pragma warning(disable :4996)
#include "stdafx.h"
#include "TitleScene.h"
#include "LobbyScene.h"
#include "Sound.h"

TCHAR* StringToTCHAR(string& s)
{
	size_t strlength = 0;
	size_t tcharlength = 0;
	strlength = strlen(s.c_str()) + 1;
	wchar_t* t = new wchar_t[strlength];
	mbstowcs_s(&tcharlength, t, strlength, s.c_str(), _TRUNCATE);
	return (TCHAR*)t;
}

CTitleScene::CTitleScene()
{
	background.Load(_T("assets/login_scene_bg.bmp"));
	boxLogin.Load(_T("assets/login_scene_login.png"));
	popup.Load(_T("assets/login_scene_alert.png"));

	m_SoundManager = new CSoundManager();
	m_SoundManager->AddStream("assets/sound/login_scene.mp3", Sound_Name::BGM_MAIN_GAME);
	m_SoundManager->PlayBgm(Sound_Name::BGM_MAIN_GAME);

	player.id = "";
	player.pw = "";
	player.isNew = FALSE;
	check.result = FALSE;
	selected = "ID"; 
	textmessage = "test~";

	btNewid.Init(_T("assets/login_scene_newidBT.png"), { 355, 610 });
	btLogin.Init(_T("assets/login_scene_loginBT.png"), { 468, 610 });
	btExit.Init(_T("assets/login_scene_exitBT.png"), { 580, 610 });

	LoadAccounts(); 

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

	boxLogin.TransparentBlt(
		hdc, (CLIENT_WIDTH - boxLogin.GetWidth()) / 2, (CLIENT_HEIGHT - boxLogin.GetHeight()) / 2 + 130,
		boxLogin.GetWidth(), boxLogin.GetHeight(),
		0, 0, boxLogin.GetWidth(), boxLogin.GetHeight(),
		RGB(255, 0, 255));

	popup.TransparentBlt(
		hdc, (CLIENT_WIDTH - popup.GetWidth()) / 2, (CLIENT_HEIGHT - popup.GetHeight()) / 2 - 100,
		popup.GetWidth(), popup.GetHeight(),
		0, 0, popup.GetWidth(), popup.GetHeight(),
		RGB(255, 0, 255));

	btNewid.Draw(hdc);
	btLogin.Draw(hdc);
	btExit.Draw(hdc);

	TextOut(hdc, 500, 538, StringToTCHAR(player.id), player.id.length());
	TextOut(hdc, 500, 563, StringToTCHAR(player.pw), player.pw.length());
	TextOut(hdc, 390, 250, StringToTCHAR(textmessage), textmessage.length());

}

void CTitleScene::Communicate(SOCKET& sock)
{
	int retval;
	vector<string> toSendData;
	char buf[BUFSIZE + 1];
	toSendData.push_back(to_string((int)m_Type));
	SendFrameData(sock, toSendData[0], retval);

	toSendData.clear();
	toSendData.push_back(player.id);
	toSendData.push_back(player.pw);
	toSendData.push_back(to_string(player.isNew));

	for (int i = 0; i < toSendData.size(); ++i)
	{
		SendFrameData(sock, toSendData[i], retval);
	}

	if (RecvFrameData(sock, buf, retval))
		check.result = (bool)buf;
}

void CTitleScene::ProcessMouseClick(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	float mx = LOWORD(lParam);
	float my = HIWORD(lParam);

	if (mx > 396 && mx < 490 && my > 536 && my < 558)
		selected = "ID";
	if (mx > 396 && mx < 490 && my > 563 && my < 585)
		selected = "PW";

	if (message == WM_LBUTTONDOWN)
	{
		if (btNewid.IsClicked({ mx, my })) {
			player.isNew = TRUE;
			if (check.result)
				textmessage = "Account registration completed!";
			else
				textmessage = "This ID already exists.";
		}
		if (btLogin.IsClicked({ mx, my })) {
			player.isNew = FALSE;
			if (check.result)
				ChangeScene<CLobbyScene>();
			else
				textmessage = "Wrong Password.";
		}
		if (btExit.IsClicked({ mx, my })) {
			DestroyWindow(hWnd);
		}
	}

}

void CTitleScene::ProcessCHARInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (selected == "ID") {
		if ((wParam == VK_BACK) && player.id.empty() == FALSE) player.id.pop_back();
		else if ((wParam == VK_BACK) && player.id.empty());
		else {
			if (player.id.length() != 18)
				player.id.push_back(wParam);
		}
	}
	if (selected == "PW") {
		if ((wParam == VK_BACK) && player.pw.empty() == FALSE) player.pw.pop_back();
		else if ((wParam == VK_BACK) && player.pw.empty());
		else {
			if (player.pw.length() != 18)
				player.pw.push_back(wParam);
		}
	}

}

void CTitleScene::LoadAccounts()
{
	ifstream in("data/Account.txt"s);

	if (in) {
		string id, pw;

		while (!in.eof())
		{
			in >> id >> pw;
			accounts.insert(pair<string, string>(id, pw));
		}
	}

	in.close();
}

void CTitleScene::RegisterNewID()
{
	ofstream out;
	out.open("data/Account.txt"s, ios::app);

	TitleSceneSendData account = { player.id ,player.pw, TRUE };
	auto isAdded = accounts.insert(pair<string, string>(player.id, player.pw));
	if (isAdded.second == TRUE) {
		out << player.id << " " << player.pw << endl;
		textmessage = "Account registration completed!";
	}
	else {
		textmessage = "This ID already exists.";
	}

	out.close();
}
