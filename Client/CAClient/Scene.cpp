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
}

void CNullScene::Draw(HDC hdc)
{
	RECT rt{ 170, 60, m_Image.GetWidth() +170, m_Image.GetHeight()+60};
	m_Image.Draw(hdc, rt);
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
		ChangeScene<CGameScene>();
		break;
	case VK_F4:	// 3
		ChangeScene<CGameRecordScene>();
		break;
	}
}
