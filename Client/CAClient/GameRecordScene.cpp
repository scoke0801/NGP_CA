#include "stdafx.h"
#include "GameRecordScene.h"

CGameRecordScene::CGameRecordScene()
{
	background.Load(_T("assets/score_scene_bg.png"));

	sendData = { "asd", 200, 121 };

	m_Type = SceneType::GameRecordScene;

	communicate = TRUE;
}

CGameRecordScene::~CGameRecordScene()
{

}

void CGameRecordScene::SendDataToNextScene(void* pContext)
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

		wsprintf(str, TEXT("%d 점"), s.itemScore);
		TextOut(hdc, 200, 105 + 60 * count, str, lstrlen(str));

		wsprintf(str, TEXT("%d 점"), s.survivedScore);
		TextOut(hdc, 350, 105 + 60 * count, str, lstrlen(str));

		count++;
	}
}

void CGameRecordScene::Communicate(SOCKET& sock)
{
	if (communicate == FALSE) return;

	int retval;
	char buffer[BUFSIZE + 1];

	GameRecordSceneRecvData recvData;	//수신받을 데이터
	string data;						//데이터 복사를 위한 공간

	//씬 번호 송신
	SendFrameData(sock, to_string((int)m_Type), retval);

	//플레이어 ID와 점수 송신
	data = "<ID>";
	data += sendData.id;
	data += "<ITS>";
	data += to_string(sendData.itemScore);
	data += "<SVS>";
	data += to_string(sendData.survivedScore);
	SendFrameData(sock, data, retval);
	
	//확인용 출력
	cout << "[ID]: " << sendData.id << " [ITS]: " << sendData.itemScore << " [SVS]: " << sendData.survivedScore << endl;


	//상위 점수 10개 목록 수신
	for (int i = 0; i < 10; i++) {
		RecvFrameData(sock, buffer, retval);
		string data = buffer;

		recvData.id = data.substr(data.find("<ID>") + 4, data.find("<ITS>") - (data.find("<ID>") + 4));
		recvData.itemScore = atoi(data.substr(data.find("<ITS>") + 5).c_str());
		recvData.survivedScore = atoi(data.substr(data.find("<SVS>") + 5).c_str());
		scores.push_back(recvData);
	}

	communicate = FALSE;
}
