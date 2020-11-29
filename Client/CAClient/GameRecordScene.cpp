#include "stdafx.h"
#include "GameRecordScene.h"

CGameRecordScene::CGameRecordScene()
{
	background.Load(_T("assets/score_scene_bg.png"));

	player = { "asd", 200, 121 };

	m_Type = SceneType::GameRecordScene;

	communicate = TRUE;
}

CGameRecordScene::~CGameRecordScene()
{

}

void CGameRecordScene::Update(float timeElapsed)
{
}

void CGameRecordScene::Draw(HDC hdc)
{
	SetBkMode(hdc, TRANSPARENT);
	background.StretchBlt(hdc, 0, 0, m_rtClient.right, m_rtClient.bottom,
		0, 0, background.GetWidth(), background.GetHeight());

	int count = 0;
	TCHAR str[20];
	for (auto s : scores) {
		TextOut(hdc, 100, 105 + 60 * count, StringToTCHAR(s.id), s.id.length());

		wsprintf(str, TEXT("%d Á¡"), s.itemScore);
		TextOut(hdc, 200, 105 + 60 * count, str, lstrlen(str));

		wsprintf(str, TEXT("%d Á¡"), s.survivedScore);
		TextOut(hdc, 350, 105 + 60 * count, str, lstrlen(str));

		count++;
	}
}

void CGameRecordScene::Communicate(SOCKET& sock)
{
	if (communicate == FALSE) return;

	int retval;
	char buffer[BUFSIZE + 1];
	string data;

	GameRecordSceneRecvData recvData;

	SendFrameData(sock, to_string((int)m_Type), retval);

	data = "<ID>";
	data += player.id;
	data += "<ITS>";
	data += to_string(player.itemScore);
	data += "<SVS>";
	data += to_string(player.survivedScore);

	cout << "[ID]: " << player.id << " [ITS]: " << player.itemScore << " [SVS]: " << player.survivedScore << endl;

	SendFrameData(sock, data, retval);

	for (int i = 0; i < 10; i++) {
		RecvFrameData(sock, buffer, retval);
		string data = buffer;

		recvData.id = data.substr(data.find("<ID>") + 4, data.find("<ITS>") - (data.find("<ID>") + 4));
		recvData.itemScore = atoi(data.substr(data.find("<ITS>") + 5).c_str());
		recvData.survivedScore = atoi(data.substr(data.find("<SVS>") + 5).c_str());
		scores.push_back(recvData);
	}

	sort(scores.begin(), scores.end(),
		[](const GameRecordSceneRecvData& a, const GameRecordSceneRecvData& b) {
			return (a.itemScore + a.survivedScore) > (b.itemScore + b.survivedScore); });

	communicate = FALSE;
}
