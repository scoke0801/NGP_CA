#include "stdafx.h"
#include "GameRecordScene.h"

CGameRecordScene::CGameRecordScene()
{
	background.Load(_T("assets/score_scene_bg.png"));

	players[0] = { {L"����"}, 500, 2000 };
	players[1] = { {L"�հ�"}, 400, 2100 };
	players[2] = { {L"����"}, 300, 2200 };
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
	for (GameScore player : players) {
		TextOut(hdc, 100, 100 + 50 * count, player.id, lstrlen(player.id));

		wsprintf(str, TEXT("%d ��"), player.itemScore);
		TextOut(hdc, 200, 100 + 50 * count, str, lstrlen(str));

		wsprintf(str, TEXT("%d ��"), player.survivedScore);
		TextOut(hdc, 350, 100 + 50 * count, str, lstrlen(str));

		count++;
	}
}

void CGameRecordScene::Communicate()
{
}
