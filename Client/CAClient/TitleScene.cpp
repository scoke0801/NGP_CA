#include "stdafx.h"
#include "TitleScene.h"


CTitleScene::CTitleScene()
{
	m_Image.Load(_T("assets/login_scene_bg.bmp"));
}

CTitleScene::~CTitleScene()
{
}

void CTitleScene::Update(float timeElapsed)
{
}

void CTitleScene::Draw(HDC hdc)
{
	m_Image.StretchBlt(hdc, 0, 0, m_rtClient.right, m_rtClient.bottom,
		0, 0, m_Image.GetWidth(), m_Image.GetHeight());
}

void CTitleScene::Communicate()
{
}
