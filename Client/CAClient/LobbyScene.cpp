#include "stdafx.h"
#include "LobbyScene.h"
#include "GameScene.h"
#include "GameFramework.h"

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

	// 타입 초기화
	m_Type = SceneType::LobbyScene;

	for (int i = 0; i < 3; i++)
	{
		R_Player[i].chartype = 0;
		m_Player[i].Num = 0;
		m_Player[i].chatData = "";
		is_Select[i] = "Chatting";
	}
}

CLobbyScene::~CLobbyScene()
{ 
}

void CLobbyScene::Update(float timeElapsed)
{
	if (m_Play.Num == 2)
	{
		Player2_Exist = true;
	}
	if (m_Play.Num == 3)
	{
		Player2_Exist = true;
		Player3_Exist = true;
	}

	if (isGameStart) 
	{
		LobbyToGameSceneData Data;

		Data.playerNum = m_Play.Num;

		Data.ClientIdx = m_Play.playerIndex;

		for (int i = 0; i < Data.playerNum; i++)
		{
			Data.id_[i] = m_Player[i].id;
			Data.chName[i] = R_Player[i].chartype;
			Data.idx_[i] = R_Player[i].index;

		} 
		ChangeScene<CGameScene>((void*)&Data); 
	}
}

void CLobbyScene::Draw(HDC hdc)
{
	SetBkMode(hdc, TRANSPARENT);
	

	m_Bg_Image[R_Player[0].chartype].StretchBlt(hdc, 0, 0, m_rtClient.right, m_rtClient.bottom,
			0, 0, m_Bg_Image[R_Player[0].chartype].GetWidth(), m_Bg_Image[R_Player[0].chartype].GetHeight());
	
	TextOut(hdc, 250, 685, StringToTCHAR(m_Player[0].chatData), m_Player[0].chatData.length());

	/*if (m_Player->Num ==2)
	{
		TextOut(hdc, 250, 685, StringToTCHAR(m_Player[1].chatData), m_Player[1].chatData.length());
	}

	if (m_Player->Num == 3)
	{
		TextOut(hdc, 250, 685, StringToTCHAR(m_Player[2].chatData), m_Player[2].chatData.length());
	}*/

	if (Player2_Exist)
	{
		m_Player2_Images[R_Player[1].chartype].TransparentBlt(
			hdc, 172, 125, 130, 185,
			5, 0, 100, 140, RGB(0, 0, 0)
		);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		TextOut(hdc, 180, 255, TEXT("아이디1"), 4);
	}

	if (Player3_Exist)
	{
		m_Player3_Images[R_Player[2].chartype].TransparentBlt(
			hdc, 300, 115, 130, 185,
			0, 0, 100, 140, RGB(0, 0, 0)
		);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		TextOut(hdc, 360, 255, TEXT("아이디2"), 4);
	}
}

void CLobbyScene::SendDataToNextScene(void* pContext)
{
	TitleToLobbySceneData* data = (TitleToLobbySceneData*)pContext;
	m_Play.id = data->id;
	m_Play.playerIndex = data->playerindx;
}

void CLobbyScene::Communicate(SOCKET& sock)
{
	int retVal;
	string toSendData = to_string((int)m_Type);
	
	// 씬타입 보내기
	SendFrameData(sock, toSendData, retVal);

	toSendData.clear();
	/////////////////////////

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
	m_Play.Num = buf[2] - 48;

	if (buf[2] - 48 == 2)
	{
		m_Play.Num = 1;
		m_Play.Num = buf[2] - 48;
	}
	if (buf[2] - 48 == 3)
	{
		m_Play.Num = 1;
		m_Play.Num = 2;
		m_Play.Num = buf[2] - 48;
	}

	toSendData = " ";
	toSendData += to_string(m_Play.char_type);

	SendFrameData(sock, toSendData, retVal);
	
	RecvFrameData(sock, buf, retVal);

	R_Player[0].chartype = buf[0] - 48;
	R_Player[1].chartype = buf[1] - 48;
	R_Player[2].chartype = buf[2] - 48;

	toSendData.clear();

	toSendData = "\n";
	toSendData += m_Play.id;

	SendFrameData(sock, toSendData, retVal);

	RecvFrameData(sock, buf, retVal);

	char* token = strtok(buf, "\n");
	strstr(token, "ID");
	int tokenLen = strlen(token);

	strncpy(temp, token + tokenLen, strlen(token) - tokenLen);
	
	int d = atoi(temp);

	for (int i = 0; i <= d; i++)
	{
		token = strtok(NULL, "\n");

		strcpy(temp, token);

		string b = temp;

		m_Player[i].id = b;
	}




	toSendData.clear();
	toSendData += to_string(m_Play.playerIndex);

	SendFrameData(sock, toSendData, retVal);

	RecvFrameData(sock, buf, retVal);

	R_Player[0].index= buf[0] - 48;
	R_Player[1].index = buf[1] - 48;
	R_Player[2].index = buf[2] - 48;

	
	//R_Player[0]

	
	//R_Player[0].id = buf;

	//cout << R_Player[0].id;

	// 채팅데이터 추가
	/*for (int i = 0; i < 3; i++)
	{
		toSendData.emplace_back(m_Player_c[i].chatData);
	}*/

	// 채팅 데이터 보내기
	/*for (int i = 1; i < toSendData.size(); ++i)
	{*/
		//SendFrameData(sock, toSendData[0], retVal);
	/*}*/
}

void CLobbyScene::ProcessMouseInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	float mx = LOWORD(lParam);
	float my = HIWORD(lParam);

	if (mx > 671 && mx < 909 && my > 648 && my < 710)
	{
		if (m_Play.playerIndex == 0)
		{
			if (is_All_Ready)
			{
				isGameStart = true;
			}
			else
			{
				isGameStart = false;
			}
		}
		if (m_Play.playerIndex == 1)
		{
			Player2_Ready = true;
		}
		else
		{
			Player2_Ready = false;
		}
		if (m_Play.playerIndex == 2)
		{
			Player3_Ready = true;
		}
		else
		{
			Player3_Ready = false;
		}

	}

	if (mx > 631 && mx < 820 && my > 187 && my < 371)
	{
		if (m_Play.playerIndex == 0)
		{
			m_Play.char_type = 0;
			
		}
		if (m_Play.playerIndex == 1)
		{
			m_Play.char_type = 0;
			
		}
		if (m_Play.playerIndex == 2)
		{
			m_Play.char_type = 0;
			
		}
	}
	if (mx > 821 && mx < 1000 && my > 187 && my < 371)
	{
		if (m_Play.playerIndex == 0)
		{
			m_Play.char_type = 1;
			
		}
		if (m_Play.playerIndex == 1)
		{
			m_Play.char_type = 1;
			
		}
		if (m_Play.playerIndex == 2)
		{
			m_Play.char_type = 1;
			
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
		for (int i = 0; i < m_Player->Num; i++)
		{
			is_Select[i] = "Chatting";
		}
	else
		for (int i = 0; i < m_Player->Num; i++)
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
		cout << m_Player[0].chatData << endl;
		break;

	}
}

void CLobbyScene::ProcessCHARInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	for (int i = 0; i < m_Player->Num; i++)
	{
		if (is_Select[i] == "Chatting") {
			if ((wParam == VK_BACK) && m_Player[i].chatData.empty() == FALSE) m_Player[i].chatData.pop_back();
			else if ((wParam == VK_BACK) && m_Player[i].chatData.empty());
			else {
				if (m_Player[i].chatData.length() != 35)
					m_Player[i].chatData.push_back(wParam);
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

