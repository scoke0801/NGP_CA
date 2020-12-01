#include "stdafx.h"
#include "LobbyScene.h"
#include "GameScene.h"
#include "GameFramework.h" 
#include "Sound.h" 
//#ifdef _DEBUG
//#ifdef UNICODE
//#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
//#else
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//#endif
//#endif

CLobbyScene::CLobbyScene()
{
	m_Bg_Image[0].Load(_T("assets/lobby_scene_bg.png"));
	m_Bg_Image[1].Load(_T("assets/lobby_scene_dao.png"));

	m_Player2_Images[0].Load(_T("assets/lobby_scene_player2.png"));
	m_Player2_Images[1].Load(_T("assets/lobby_scene_player2_dao.png"));
	m_Player2_Images[2].Load(_T("assets/lobby_scene_player2_ready.png"));
	m_Player2_Images[3].Load(_T("assets/lobby_scene_player2_dao_ready.png"));

	m_Player3_Images[0].Load(_T("assets/lobby_scene_player3.png"));
	m_Player3_Images[1].Load(_T("assets/lobby_scene_player3_dao.png"));
	m_Player3_Images[2].Load(_T("assets/lobby_scene_player3_ready.png"));
	m_Player3_Images[3].Load(_T("assets/lobby_scene_player3_dao_ready.png"));

	m_SoundManager = new CSoundManager();
	m_SoundManager->AddStream("assets/sound/Lobby.mp3", Sound_Name::BGM_LOBBY);
	m_SoundManager->PlayBgm(Sound_Name::BGM_LOBBY);

	// 타입 초기화
	m_Type = SceneType::LobbyScene;

	for (int i = 0; i < 3; i++)
	{
		Recv_Player_Data[i].chartype = 0;
		Send_Player_Data[i].Num = 0;
		Send_Player_Data[i].chatData = "";
		is_Select[i] = "Chatting";
	}
}

CLobbyScene::~CLobbyScene()
{
	m_SoundManager->Stop();
}

void CLobbyScene::Update(float timeElapsed)
{
	m_SoundManager->OnUpdate();
	if (m_Player.Num == 2)
	{
		Player2_Exist = true;
	}
	if (m_Player.Num == 3)
	{
		Player2_Exist = true;
		Player3_Exist = true;
	}
	if (m_Player.gamestart == 1)
	{
		isGameStart = true;
	}
	if (isGameStart)
	{
		LobbyToGameSceneData Data;

		Data.playerNum = m_Player.Num;

		Data.ClientIdx = m_Player.playerIndex;

		for (int i = 0; i < Data.playerNum; i++)
		{
			Data.id_[i] = Recv_Player_Data[i].id;
			Data.chName[i] = Recv_Player_Data[i].chartype;
			Data.idx_[i] = Recv_Player_Data[i].index; 
		}
		m_SoundManager->Stop();
		ChangeScene<CGameScene>((void*)&Data);
	}
}

void CLobbyScene::Draw(HDC hdc)
{
	SetBkMode(hdc, TRANSPARENT);
	
	m_Bg_Image[Recv_Player_Data[0].chartype].StretchBlt(hdc, 0, 0, m_rtClient.right, m_rtClient.bottom,
			0, 0, m_Bg_Image[Recv_Player_Data[0].chartype].GetWidth(), m_Bg_Image[Recv_Player_Data[0].chartype].GetHeight());
	
	SetTextColor(hdc, RGB(255, 255, 255));
	SetBkMode(hdc, TRANSPARENT);
	TextOut(hdc, 50 ,255, StringToTCHAR(Recv_Player_Data[0].id) , Recv_Player_Data[0].id.length());
	//m_Player[i].id
	// m_Play.id
	/*if (m_Player->Num ==2)
	{
		TextOut(hdc, 250, 685, StringToTCHAR(m_Player[1].chatData), m_Player[1].chatData.length());
	}

	if (m_Player->Num == 3)
	{
		TextOut(hdc, 250, 685, StringToTCHAR(m_Player[2].chatData), m_Player[2].chatData.length());
	}*/

	if (Player2_Exist && Recv_Player_Data[1].ready ==0)
	{
		m_Player2_Images[Recv_Player_Data[1].chartype].TransparentBlt(
			hdc, 172, 125, 130, 185,
			5, 0, 100, 140, RGB(0, 0, 0)
		);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		TextOut(hdc, 180, 255, StringToTCHAR(Recv_Player_Data[1].id), Recv_Player_Data[1].id.length());
	}
	else if (Player2_Exist && Recv_Player_Data[1].ready==1)
	{
		m_Player2_Images[Recv_Player_Data[1].chartype+2].TransparentBlt(
			hdc, 172, 125, 130, 185,
			5, 0, 100, 140, RGB(0, 0, 0)
		);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		TextOut(hdc, 180, 255, StringToTCHAR(Recv_Player_Data[1].id), Recv_Player_Data[1].id.length());
	}

	if (Player3_Exist && Recv_Player_Data[2].ready == 0)
	{
		m_Player3_Images[Recv_Player_Data[2].chartype].TransparentBlt(
			hdc, 300, 115, 130, 185,
			0, 0, 100, 140, RGB(0, 0, 0)
		);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		TextOut(hdc, 360, 255, StringToTCHAR(Recv_Player_Data[2].id), (Recv_Player_Data[2].id).length());
	}

	else if (Player3_Exist && Recv_Player_Data[2].ready == 1)
	{
		m_Player3_Images[Recv_Player_Data[2].chartype+2].TransparentBlt(
			hdc, 300, 115, 130, 185,
			0, 0, 100, 140, RGB(0, 0, 0)
		);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		TextOut(hdc, 360, 255, StringToTCHAR(Recv_Player_Data[2].id), (Recv_Player_Data[2].id).length());
	}
}

void CLobbyScene::SendDataToNextScene(void* pContext)
{
	// 타이틀씬에서 읽어오는 데이터
	TitleToLobbySceneData* data = (TitleToLobbySceneData*)pContext;
	m_Player.id = data->id;
	m_Player.playerIndex = data->playerindx;
}

void CLobbyScene::Communicate(SOCKET& sock)
{
	int retVal;
	string toSendData = to_string((int)m_Type);
	
	// 씬타입 보내기
	SendFrameData(sock, toSendData, retVal);

	toSendData.clear();

	// 접속한 순서 받기
	RecvFrameData(sock, buf, retVal);

	char temp[256] = {};
	for (int i = 0; i< 3; i++)
	{
		strncpy(temp, buf, i);
		//m_Play.playerIndex = buf[1];
		strncpy(temp, buf + i + 1, strlen(buf) - i);
		//cout << "Index : " << m_Play.playerIndex <<endl;
	}

	// 플레이어의 총 접속수 받기
	m_Player.Num = buf[2] - 48;

	if (buf[2] - 48 == 2)
	{
		m_Player.Num = 1;
		m_Player.Num = buf[2] - 48;
	}
	if (buf[2] - 48 == 3)
	{
		m_Player.Num = 1;
		m_Player.Num = 2;
		m_Player.Num = buf[2] - 48;
	}

	// 플레이어의 캐릭터 타입 보내고 받기
	toSendData = " ";
	toSendData += to_string(m_Player.char_type);

	SendFrameData(sock, toSendData, retVal);
	
	RecvFrameData(sock, buf, retVal);

	Recv_Player_Data[0].chartype = buf[0] - 48;
	Recv_Player_Data[1].chartype = buf[1] - 48;
	Recv_Player_Data[2].chartype = buf[2] - 48;


	// 아이디 보내고 받기
	toSendData.clear();

	toSendData = "\n";
	toSendData += m_Player.id;

	SendFrameData(sock, toSendData, retVal);

	RecvFrameData(sock, buf, retVal);


	char* token = strtok(buf, "\n");
	strstr(token, "ID");
	int tokenLen = strlen("ID");

	//strncpy(temp, token + tokenLen, strlen(token) - tokenLen);
	strcpy(temp, token + tokenLen);
	int d = atoi(temp);

	for (int i = 0; i < d; i++)
	{
		token = strtok(NULL, "\n");
		if (token == nullptr) continue;
		strcpy(temp, token);

		string b = temp;

		Recv_Player_Data[i].id = b;
	}

	// 인덱스 보내고 받기
	toSendData.clear();
	toSendData += to_string(m_Player.playerIndex);

	SendFrameData(sock, toSendData, retVal);

	RecvFrameData(sock, buf, retVal);

	Recv_Player_Data[0].index= buf[0] - 48;
	Recv_Player_Data[1].index = buf[1] - 48;
	Recv_Player_Data[2].index = buf[2] - 48;


	// 준비완료 보내고 받기
	toSendData.clear();
	toSendData += to_string(m_Player.isReady);
	
	SendFrameData(sock, toSendData, retVal);

	RecvFrameData(sock, buf, retVal);
	
	Recv_Player_Data[0].ready = buf[0] - 48;
	Recv_Player_Data[1].ready = buf[1] - 48;
	Recv_Player_Data[2].ready = buf[2] - 48;
	m_Player.gamestart = buf[3] - 48;

	// 채팅데이터 보내기

	//toSendData.clear();

	//toSendData = "\n";
	//toSendData += m_Play.chatData;

	//SendFrameData(sock, toSendData, retVal);

	//RecvFrameData(sock, buf, retVal);

	//char temp2[256] = {};
	//char* token2 = strtok(buf, "\n");
	//strstr(token2, "Chat");
	//int tokenLen2 = strlen(token2);

	//strncpy(temp2, token2 + tokenLen, strlen(token2) - tokenLen);

	//int g = atoi(temp2);

	//for (int i = 0; i <= d; i++)
	//{
	//	token2 = strtok(NULL, "\n");

	//	strcpy(temp2, token2);

	//	string b = temp2;

	//	m_Player[i].chatData = b;
	//}
	
}

void CLobbyScene::ProcessMouseInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	float mx = LOWORD(lParam);
	float my = HIWORD(lParam);

	if (mx > 631 && mx < 820 && my > 187 && my < 371)
	{
		if (Recv_Player_Data[0].index == 0)
		{
			m_Player.char_type = 0;
		}
		if (Recv_Player_Data[1].index == 1)
		{
			m_Player.char_type = 0;

		}
		if (Recv_Player_Data[2].index == 2)
		{
			m_Player.char_type = 0;

		}
	}
	if (mx > 821 && mx < 1000 && my > 187 && my < 371)
	{
		if (Recv_Player_Data[0].index == 0)
		{
			m_Player.char_type = 1;

		}
		if (Recv_Player_Data[1].index == 1)
		{
			m_Player.char_type = 1;

		}
		if (Recv_Player_Data[2].index == 2)
		{
			m_Player.char_type = 1;

		}
	}

	if (mx > 671 && mx < 909 && my > 648 && my < 710)
	{
		if (Recv_Player_Data[0].index == 0)
		{
			m_Player.isReady = 1;
			if (m_Player.gamestart == 1)
			{
				isGameStart = true;
			}
		}
		if (Recv_Player_Data[1].index == 1)
		{
			m_Player.isReady = 1;
		}
		if (Recv_Player_Data[2].index == 2)
		{
			m_Player.isReady = 1;

		}
	}

	if (mx > 671 && mx < 909 && my > 710 && my < 750)
	{
		if (Recv_Player_Data[0].index == 0)
		{
			m_Player.isReady = 0;
		}
		if (Recv_Player_Data[1].index == 1)
		{
			m_Player.isReady = 0;
		}
		if (Recv_Player_Data[2].index == 2)
		{
			m_Player.isReady = 0;

		}
	}
	
}

void CLobbyScene::ProcessMouseClick(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	float mx = LOWORD(lParam);
	float my = HIWORD(lParam);

	/*cout << mx << endl;
	cout << my << endl;*/

	if (mx > 230 && mx < 600 && my > 670 && my < 700)
		for (int i = 0; i < Send_Player_Data->Num; i++)
		{
			is_Select[i] = "Chatting";
		}
	else
		for (int i = 0; i < Send_Player_Data->Num; i++)
		{
			is_Select[i] = "Chatting_Stop";
		}
}

void CLobbyScene::ProcessKeyboardDownInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_F2:	// 0
		Player2_Exist = !Player2_Exist;
		break;

	case VK_F3:
		Player3_Exist = !Player3_Exist;
		break;

	case VK_F4:
		if (Player2_Exist)
		{
			Player2_Ready = !Player2_Ready;
		}
		break;

	case VK_F5:
		if (Player3_Exist)
		{
			Player3_Ready = !Player3_Ready;
		}
		break;
	case VK_F6:
		//if (is_All_Ready)
	{
		isGameStart = true;
	}
	break;
	case VK_RETURN:
		cout << Send_Player_Data[0].chatData << endl;
		break;

	}
}

void CLobbyScene::ProcessCHARInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	for (int i = 0; i < Send_Player_Data->Num; i++)
	{
		if (is_Select[i] == "Chatting") {
			if ((wParam == VK_BACK) && Send_Player_Data[i].chatData.empty() == FALSE) Send_Player_Data[i].chatData.pop_back();
			else if ((wParam == VK_BACK) && Send_Player_Data[i].chatData.empty());
			else {
				if (Send_Player_Data[i].chatData.length() != 35)
					Send_Player_Data[i].chatData.push_back(wParam);
			}
		}
	}
}

TCHAR* CLobbyScene::StringToTCHAR(string& s)
{
	size_t strlength = 0;
	size_t tcharlength = 0;
	strlength = strlen(s.c_str()) + 1;
	wchar_t* t = new wchar_t[strlength];
	mbstowcs_s(&tcharlength, t, strlength, s.c_str(), _TRUNCATE);
	return (TCHAR*)t;
	
}

