#pragma warning(disable :4996)
#include "stdafx.h"
#include "TitleScene.h"
#include "LobbyScene.h"
#include "Sound.h"

CTitleScene::CTitleScene()
{
	communicate = FALSE;
	background.Load(_T("assets/login_scene_bg.bmp"));
	boxLogin.Load(_T("assets/login_scene_login.png"));
	popup.Load(_T("assets/login_scene_alert.png"));

	m_SoundManager = new CSoundManager();
	m_SoundManager->AddStream("assets/sound/login_scene.mp3", Sound_Name::BGM_MAIN_GAME);
	m_SoundManager->PlayBgm(Sound_Name::BGM_MAIN_GAME);
	
	sendData.id = "";
	sendData.pw = "";
	sendData.isNew = FALSE;
	recvData.result = FALSE;
	selected = "ID"; 
	recvData.text = "Welcome!";

	btNewid.Init(_T("assets/login_scene_newidBT.png"), { 355, 610 });
	btLogin.Init(_T("assets/login_scene_loginBT.png"), { 468, 610 });
	btExit.Init(_T("assets/login_scene_exitBT.png"), { 580, 610 });

	m_Type = SceneType::TitleScene;
}

CTitleScene::~CTitleScene()
{
	m_SoundManager->Stop();
}

void CTitleScene::Update(float timeElapsed)
{
	if (selected == "Login" && recvData.result == TRUE) {
	//수신받은 결과가 TRUE 이고 로그인 버튼이 눌렸다면 씬 체인지 
		nextscenedata.playerindx = recvData.playerIndex;
		nextscenedata.id = sendData.id;
		ChangeScene<CLobbyScene>((void*)&nextscenedata);
	}
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

	TextOut(hdc, 500, 538, StringToTCHAR(sendData.id), sendData.id.length());
	TextOut(hdc, 500, 563, StringToTCHAR(sendData.pw), sendData.pw.length());
	TextOut(hdc, 390, 250, StringToTCHAR(recvData.text), recvData.text.length());

}

void CTitleScene::Communicate(SOCKET& sock)
{
	if (communicate == FALSE) return;

	if (sendData.id == "" || sendData.pw == "") {
		recvData.text = "ID/PW is empty...";
		return;
	}

	int retval;
	char buffer[BUFSIZE + 1];
	string data;	//데이터 복사를 위한 공간

	//씬 번호 송신
	SendFrameData(sock, to_string((int)m_Type), retval);

	//계정 정보와 신규 등록 여부 송신
	data = "<ID>";
	data += sendData.id;
	data += "<PW>";
	data += sendData.pw;
	data += "<isNew>";
	data += sendData.isNew;
	SendFrameData(sock, data, retval);

	//서버 처리 결과 수신
	RecvFrameData(sock, buffer, retval);

	data.clear();
	data = buffer;

	recvData.playerIndex = data[data.find("<INDEX>") + 7] - '0';
	recvData.text = data.substr(data.find("<TEXT>") + 6, data.find("<result>") - (data.find("<TEXT>") + 6));
	recvData.result = (bool)data[data.find("<result>") + 8];

	//확인용 출력
	cout << "[IDX]: " << recvData.playerIndex << " [TEXT]: " << recvData.text << " [result]:" << recvData.result << endl;

	communicate = FALSE;
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
			selected = "NewID";
			sendData.isNew = TRUE;
			communicate = TRUE;
		}
		if (btLogin.IsClicked({ mx, my })) {
			selected = "Login";
			sendData.isNew = FALSE;
			communicate = TRUE;
		}
		if (btExit.IsClicked({ mx, my })) {
			selected = "Exit";
			DestroyWindow(hWnd);
		}
	}

}

void CTitleScene::ProcessCHARInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (selected == "ID") {
		if ((wParam == VK_BACK) && sendData.id.empty() == FALSE) sendData.id.pop_back();
		else if ((wParam == VK_BACK) && sendData.id.empty());
		else {
			if (sendData.id.length() != 18)
				sendData.id.push_back(tolower(wParam));
		}
	}
	if (selected == "PW") {
		if ((wParam == VK_BACK) && sendData.pw.empty() == FALSE) sendData.pw.pop_back();
		else if ((wParam == VK_BACK) && sendData.pw.empty());
		else {
			if (sendData.pw.length() != 18)
				sendData.pw.push_back(tolower(wParam));
		}
	}

}