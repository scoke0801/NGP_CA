#include "stdafx.h"
#include "Scene.h"
#include "GameFramework.h"

#include "TitleScene.h"
#include "LobbyScene.h"
#include "GameScene.h"
#include "GameRecordScene.h"

CScene::CScene()
{
}

CScene::~CScene()
{
}

bool CScene::ProcessInput(UCHAR* pKeysBuffer)
{
    return false;
}

CNullScene::CNullScene()
{
	m_Image.Load(_T("assets/NullSceneInfoImage.png"));
	m_Type = SceneType::NullScene;
}

void CNullScene::Draw(HDC hdc)
{
	RECT rt{ 170, 60, m_Image.GetWidth() +170, m_Image.GetHeight()+60};
	m_Image.Draw(hdc, rt);
}

void CNullScene::Communicate(SOCKET& sock)
{
	int retVal;
	string toSendData = to_string((int)m_Type);
	SendFrameData(sock, toSendData, retVal);
	
	int receivedSize = 0;
	// +1, null value
	char buffer[BUFSIZE + 1];
	RecvFrameData(sock, buffer, receivedSize);

	char temp[30] = {};
	for (int i = 0; strlen(buffer); ++i)
	{
		if (buffer[i] == ' ')
		{
			strncpy(temp, buffer, i);
			m_Idx = atoi(buffer);
			//cout << "Index : " << m_Idx << " ID : " << m_ID << "\n";
			return;
		}
	}
	//m_Idx = atoi(buffer);
}

void CNullScene::ProcessKeyboardDownInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{ 
	case VK_F1:	// 0
		ChangeScene<CTitleScene>();
		break;
	case VK_F2:	// 1
		ChangeScene<CLobbyScene>();
		break;
	case VK_F3:	// 2
	{
		LobbyToGameSceneData data;
		data.id = m_ID;
		data.idx = m_Idx;
	
		ChangeScene<CGameScene>((void*)&data);
	}
		break;
	case VK_F4:	// 3
		ChangeScene<CGameRecordScene>();
		break;
	}
}
