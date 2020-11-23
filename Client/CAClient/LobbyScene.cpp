#include "stdafx.h"
#include "LobbyScene.h"
#include "GameScene.h"
#include "GameFramework.h"

#ifdef _DEBUG
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif
#endif

CLobbyScene::CLobbyScene()
{
	m_Bg_Image.Load(_T("assets/lobby_scene_bg.png"));

	m_Player2_Images[0].Load(_T("assets/lobby_scene_player2.png"));
	m_Player2_Images[1].Load(_T("assets/lobby_scene_player2_ready.png"));

	m_Player3_Images[0].Load(_T("assets/lobby_scene_player3.png"));
	m_Player3_Images[1].Load(_T("assets/lobby_scene_player3_ready.png"));

	m_Type = SceneType::LobbyScene;
	 
	m_Type = SceneType::LobbyScene; 

	m_Message = "test~";
	
	m_Frame = CFramework::GetInstance();
}

CLobbyScene::~CLobbyScene()
{
}




void CLobbyScene::Update(float timeElapsed)
{
	if (atoi(buf) == 2)
	{
		Player2_Exist = true;
		
	}

	if (atoi(buf) == 3)
	{
		Player2_Exist = true;
		Player3_Exist = true;
	}
	else if (atoi(buf) > 3)
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
	m_Bg_Image.StretchBlt(hdc, 0, 0, m_rtClient.right, m_rtClient.bottom,
		0, 0, m_Bg_Image.GetWidth(), m_Bg_Image.GetHeight());

	if (atoi(buf) ==2)
	{
		Player2_Exist = true;
		Update(0.5);
	}

	if (atoi(buf) == 3)
	{
		Player2_Exist = true;
		Player3_Exist = true;
	}
	else if(atoi(buf) > 3)
	{
		Player2_Exist = true;
		Player3_Exist = true;
	}

	TextOut(hdc, 500, 538, StringToTCHAR(m_Message), m_Message.length());

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
	//TextOut(hdc, 500, 538, StringToTCHAR(player.id), player.id.length());
}

void CLobbyScene::Communicate(SOCKET& sock)
{
	int retVal;
	vector<string> toSendData;

	toSendData.emplace_back(to_string((bool)is_All_Ready));

	
	int receivedSize = 0;

	if (RecvFrameData(sock, buf, receivedSize))
	{
		cout << "dd";
	}

	if (is_All_Ready)
	{
		SendFrameData(sock, toSendData[0], retVal);
	}
	
}

void CLobbyScene::ProcessMouseInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	float mx = LOWORD(lParam);
	float my = HIWORD(lParam);

	if (mx > 396 && mx < 490 && my > 536 && my < 558)
		is_Select = "Chatting";
	
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
			Player2_Ready=1; 
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

