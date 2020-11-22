#include "stdafx.h"
#include "LobbyScene.h"
#include "GameScene.h"

CLobbyScene::CLobbyScene()
{
	

	m_Bg_Image.Load(_T("assets/lobby_scene_bg.png"));

	m_Player2_Images[0].Load(_T("assets/lobby_scene_player2.png"));	
	m_Player2_Images[1].Load(_T("assets/lobby_scene_player2_ready.png"));	

	m_Player3_Images[0].Load(_T("assets/lobby_scene_player3.png"));
	m_Player3_Images[1].Load(_T("assets/lobby_scene_player3_ready.png"));

<<<<<<< Updated upstream
=======
	m_Type = SceneType::LobbyScene;

>>>>>>> Stashed changes
	
}


CLobbyScene::~CLobbyScene()
{
}

void CLobbyScene::Update(float timeElapsed)
{
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

void CLobbyScene::Communicate()
{
	int retVal;
	string toSendData;
	if (is_All_Ready)
	{
		toSendData = "준비완료";
		SendFrameData(sock, toSendData, retVal);
	}

}

void CLobbyScene::ProcessKeyboardDownInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_F2:	// 0
		Player2_Exist =! Player2_Exist;
		break;

	case VK_F3:
		Player3_Exist =! Player3_Exist;
		break;

	case VK_F4:
		if (Player2_Exist)
		{
<<<<<<< Updated upstream
			Player2_Ready=1;
=======
			Player2_Ready = !Player2_Ready;
>>>>>>> Stashed changes
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

