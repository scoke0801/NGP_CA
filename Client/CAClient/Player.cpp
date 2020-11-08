#include "stdafx.h"
#include "Player.h"

CPlayer::CPlayer()
{
	m_ImageIdx = 0;
	m_Images[0].Load(_T("assets/player/bazzi/left.bmp"));
	m_Images[1].Load(_T("assets/player/bazzi/right.bmp"));
	m_Images[2].Load(_T("assets/player/bazzi/up.bmp"));
	m_Images[3].Load(_T("assets/player/bazzi/down.bmp"));
	m_Images[4].Load(_T("assets/player/bazzi/trap.bmp"));
	m_Images[5].Load(_T("assets/player/bazzi/die.bmp"));
	m_Images[6].Load(_T("assets/player/bazzi/live.bmp"));

	m_Position = { 30, 50 };
	m_Size = { 64, 76 };
	m_AnimationIdx = 0;
}

CPlayer::~CPlayer()
{
}

void CPlayer::Draw(HDC hdc)
{
	m_Images[m_ImageIdx].TransparentBlt(
		hdc, m_Position.x, m_Position.y,
		m_Size.x, m_Size.y,
		m_AnimationIdx * m_Size.x, 0, m_Size.x, m_Size.y,
		RGB(255,0,255));
}

void CPlayer::Update(float timeElapsed)
{
	++m_AnimationIdx;
	if (m_AnimationIdx >= 6)
	{
		m_AnimationIdx = 0;
	}
}
