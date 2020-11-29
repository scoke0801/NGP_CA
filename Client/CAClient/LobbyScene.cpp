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

	m_Frame = CFramework::GetInstance();

	// 플레이어의 접속숫자 
	m_Player->Num = 0;

	// 채팅 초기화
	for (int i = 0; i < 3; i++)
	{
		m_Player[i].chatData = "";
		m_Player[i].Char_Type = "Bazzi";
		is_Character_Select[i] = "Bazzi";
		is_Select[i] = "Chatting";
	}

}

CLobbyScene::~CLobbyScene()
{
}

void CLobbyScene::SendDataToNextScene(void* pContext)
{
	TitleToLobbySceneData* data = (TitleToLobbySceneData*)pContext;

	m_ClientIdx = data->playerindx;
	m_ClientID = data->id;

	cout << "####################################################" << endl
		<< "[IDX]: " << m_ClientIdx << " [ID]: " << m_ClientID << endl
		<< "####################################################" << endl;
}

void CLobbyScene::Update(float timeElapsed)
{
	if (is_Character_Select[0] == "Bazzi")
	{
		m_Player[0].Char_Type = "Bazzi";
		Character_Change[0] = 0;
	}

	if (is_Character_Select[0] == "dao")
	{
		m_Player[0].Char_Type = "dao";
		Character_Change[0] = 1;
	}

	if (is_Character_Select[1] == "Bazzi")
	{
		m_Player[1].Char_Type = "Bazzi";
		Character_Change[0] = 0;
	}

	if (is_Character_Select[1] == "dao")
	{
		m_Player[1].Char_Type = "dao";
		Character_Change[0] = 1;
	}
	if (is_Character_Select[2] == "Bazzi")
	{
		m_Player[2].Char_Type = "Bazzi";
		Character_Change[0] = 0;
	}

	if (is_Character_Select[2] == "dao")
	{
		m_Player[2].Char_Type = "dao";
		Character_Change[0] = 1;
	}

	if (m_Player->Num == 2)
	{
		Player2_Exist = true;
	}
	if (m_Player->Num == 3)
	{
		Player2_Exist = true;
		Player3_Exist = true;
	}
	else if (m_Player->Num > 3)
	{
		Player2_Exist = true;
		Player3_Exist = true;
	}

	if (Player2_Ready && Player3_Ready)
	{
		is_All_Ready = true;
	}
	else
	{
		is_All_Ready = false;
	}

	if (isGameStart)
	{
		ChangeScene<CGameScene>();
	}
}

void CLobbyScene::Draw(HDC hdc)
{
	SetBkMode(hdc, TRANSPARENT);
	
	for (int i = 0; i < m_Player->Num; i++)
		cout << i+1 <<"번의 타입은"<<m_Player[i].Char_Type << endl;

	m_Bg_Image[Character_Change[0]].StretchBlt(hdc, 0, 0, m_rtClient.right, m_rtClient.bottom,
			0, 0, m_Bg_Image[Character_Change[0]].GetWidth(), m_Bg_Image[Character_Change[0]].GetHeight());
	

	TextOut(hdc, 250, 685, StringToTCHAR(m_Player[0].chatData), m_Player[0].chatData.length());

	if (m_Player->Num ==2)
	{
		Player2_Exist = true;
		TextOut(hdc, 250, 685, StringToTCHAR(m_Player[1].chatData), m_Player[1].chatData.length());
	}

	if (m_Player->Num == 3)
	{
		Player2_Exist = true;
		Player3_Exist = true;
		TextOut(hdc, 250, 685, StringToTCHAR(m_Player[2].chatData), m_Player[2].chatData.length());
	}
	else if(m_Player->Num > 3)
	{
		Player2_Exist = true;
		Player3_Exist = true;
	}
	
	if (Player2_Exist)
	{
		m_Player2_Images[0].TransparentBlt(
			hdc, 172, 125, 130, 185,
			5, 0, 100, 140, RGB(0, 0, 0)
		);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		TextOut(hdc, 180, 255, TEXT("아이디1"), 4);

	}

	if (Player3_Exist)
	{
		m_Player3_Images[0].TransparentBlt(
			hdc, 300, 115, 130, 185,
			0, 0, 100, 140, RGB(0, 0, 0)
		);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		TextOut(hdc, 360, 255, TEXT("아이디2"), 4);
	}
	
}

void CLobbyScene::Communicate(SOCKET& sock)
{
	int retVal;
	vector<string> toSendData;
	int receivedSize = 0;
	
	// 1 씬타입 보내기
	SendFrameData(sock, to_string((int)m_Type), retVal);

	// 4 플레이어의 접속수 받기
	RecvFrameData(sock, buf, retVal);

	m_Player->Num = atoi(buf);
	
	cout << endl;
	cout << "접속자수" << m_Player->Num << endl;

	// 5 캐릭터 종류 보내기
	for (int i = 0; i < m_Player->Num; i++)
	{
		SendFrameData(sock, m_Player[i].Char_Type, retVal);

		// 6 캐릭터 종류 받기
		RecvFrameData(sock, buf, retVal);
		m_Player[i].Char_Type = buf[i];

	}

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

	//cout << "채팅데이터" << toSendData[2];

}

void CLobbyScene::ProcessMouseInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	float mx = LOWORD(lParam);
	float my = HIWORD(lParam);

	if (mx > 631 && mx < 820 && my > 187 && my < 371)
	{
		if (m_Player->Num == 1)
		{
			is_Character_Select[0] = "Bazzi";
		}
		else if (m_Player->Num == 2)
		{
			is_Character_Select[1] = "Bazzi";
		}
		else if (m_Player->Num == 3)
		{
			is_Character_Select[2] = "Bazzi";
		}
	}
		

	else if (mx > 821 && mx < 1000 && my > 187 && my < 371)
	{
		if (m_Player->Num == 1)
		{
			is_Character_Select[0] = "dao";
		}
		else if (m_Player->Num == 2)
		{
			is_Character_Select[1] = "dao";
		}
		else if (m_Player->Num == 3)
		{
			is_Character_Select[2] = "dao";
		}
	}

}

void CLobbyScene::ProcessMouseClick(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	float mx = LOWORD(lParam);
	float my = HIWORD(lParam);

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
		if (is_All_Ready)
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

