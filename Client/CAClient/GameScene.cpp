#include "stdafx.h"
#include "GameScene.h"
#include "Player.h"

CGameScene::CGameScene()
{
	m_Players.push_back(new CPlayer());
}

CGameScene::~CGameScene()
{
}

void CGameScene::Update(float timeElapsed)
{
	for (auto player : m_Players)
	{
		player->Update(timeElapsed);
	}
	
}

void CGameScene::Draw(HDC hdc)
{
	for (auto player : m_Players)
	{
		player->Draw(hdc);
	}
}

void CGameScene::Communicate()
{
}
