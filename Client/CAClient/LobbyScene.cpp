#include "stdafx.h"
#include "LobbyScene.h"
#include "GameScene.h"
#include "GameFramework.h"

//#ifdef _DEBUG
//#ifdef UNICODE
//#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
//#else
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//#endif
//#endif

CLobbyScene::CLobbyScene()
{
	m_Bg_Image.Load(_T("assets/lobby_scene_bg.png"));

	m_Player2_Images[0].Load(_T("assets/lobby_scene_player2.png"));
	m_Player2_Images[1].Load(_T("assets/lobby_scene_player2_ready.png"));

	m_Player3_Images[0].Load(_T("assets/lobby_scene_player3.png"));
	m_Player3_Images[1].Load(_T("assets/lobby_scene_player3_ready.png"));

	// 타입 초기화
	m_Type = SceneType::LobbyScene;

	m_Frame = CFramework::GetInstance();

	// 플레이어의 접속숫자 
	m_Player_n.Num = 0;


	// 채팅 초기화
	for (int i = 0; i < 3; i++)
	{
		m_Player_c[i].chatData = "";
	}

	is_Select = "Chatting";
	
}

CLobbyScene::~CLobbyScene()
{
}

void CLobbyScene::Update(float timeElapsed)
{
	if (m_Player_n.Num == 2)
	{
		Player2_Exist = true;
	}
	if (m_Player_n.Num == 3)
	{
		Player2_Exist = true;
		Player3_Exist = true;
	}
	else if (m_Player_n.Num > 3)
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

	m_Bg_Image.StretchBlt(hdc, 0, 0, m_rtClient.right, m_rtClient.bottom,
		0, 0, m_Bg_Image.GetWidth(), m_Bg_Image.GetHeight());

	TextOut(hdc, 250, 685, StringToTCHAR(m_Player_c[0].chatData), m_Player_c[0].chatData.length());

	if (m_Player_n.Num ==2)
	{
		Player2_Exist = true;
		TextOut(hdc, 250, 685, StringToTCHAR(m_Player_c[1].chatData), m_Player_c[1].chatData.length());
	}

	if (m_Player_n.Num == 3)
	{
		Player2_Exist = true;
		Player3_Exist = true;
		TextOut(hdc, 250, 685, StringToTCHAR(m_Player_c[2].chatData), m_Player_c[2].chatData.length());
	}
	else if(m_Player_n.Num > 3)
	{
		Player2_Exist = true;
		Player3_Exist = true;
	}
	

	if (Player2_Exist)
	{
		m_Player2_Images[Player2_Ready].TransparentBlt(
			hdc, 172, 125, 130, 185,
			5, 0, 100, 140, RGB(0, 0, 0)
		);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		TextOut(hdc, 180, 255, TEXT("아이디1"), 4);
	}

	if (Player3_Exist)
	{
		m_Player3_Images[Player3_Ready].TransparentBlt(
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

	m_Player_n.Num = atoi(buf);

	cout << endl;
	cout << "접속자수" << m_Player_n.Num << endl;

	// 채팅데이터 추가
	for (int i = 0; i < 3; i++)
	{
		toSendData.emplace_back(m_Player_c[i].chatData);
	}

	// 채팅 데이터 보내기
	/*for (int i = 1; i < toSendData.size(); ++i)
	{*/
		SendFrameData(sock, toSendData[0], retVal);
	/*}*/

	cout << "채팅데이터" << toSendData[2];

}

void CLobbyScene::ProcessMouseInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	float mx = LOWORD(lParam);
	float my = HIWORD(lParam);

	if (mx > 230 && mx < 600 && my > 670 && my < 700)
		is_Select = "Chatting";
	else
		is_Select = "Chatting_Stop";
	
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
			cout << m_Player_c[0].chatData << endl;
		break;

	}
}

void CLobbyScene::ProcessCHARInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	for (int i = 0; i < 3; i++)
	{
		if (is_Select == "Chatting") {
			if ((wParam == VK_BACK) && m_Player_c[i].chatData.empty() == FALSE) m_Player_c[i].chatData.pop_back();
			else if ((wParam == VK_BACK) && m_Player_c[i].chatData.empty());
			else {
				if (m_Player_c[i].chatData.length() != 35)
					m_Player_c[i].chatData.push_back(wParam);
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

