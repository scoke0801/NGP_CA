#include "stdafx.h"
#include "Player.h"
#include "Bomb.h"
#include "Communicates.h"

CPlayer::CPlayer(Vector2D<float> position, CharacterName name, int index)
{
	m_Position = position;

	m_CurrentBombNum = 0;
	m_Power = 3;
	m_MaxBomb = 2;
	m_Speed = 2;
	m_TimeSum = 0.0f;

	m_Size = { OBJECT_SIZE, OBJECT_SIZE };
	m_Vel = { PlAYER_SPEED * m_Speed, PlAYER_SPEED * m_Speed };

	m_Dir = Direction::down;

	m_ID = "Player0";
	m_Idx = index;
	LoadImages(name);
	LoadSounds();

	m_State = PlayerState::wait;
	m_BoomCreateFlag = false;
}

CPlayer::~CPlayer()
{
}

void CPlayer::Draw(HDC hdc)
{
	DrawPlayerInfo(hdc);
	if (!m_Images[(int)m_currentAnimation]) return;
	if (m_State != PlayerState::die)
	{
		m_Images[(int)m_currentAnimation].TransparentBlt(
			hdc, m_Position.x, m_Position.y,
			m_Size.x, m_Size.y,
			//m_AnimationSizes[(int)m_currentAnimation].x,
			//m_AnimationSizes[(int)m_currentAnimation].x,
			m_AnimationIdx * m_AnimationSizes[(int)m_currentAnimation].x, 0,
			m_AnimationSizes[(int)m_currentAnimation].x,
			m_AnimationSizes[(int)m_currentAnimation].y,
			RGB(255, 0, 255));
	}
	else
	{
		m_Images[(int)m_currentAnimation].TransparentBlt(
			hdc, m_Position.x - 25, m_Position.y - 25,
			m_Size.x * 1.8, m_Size.y *1.8,
			//m_AnimationSizes[(int)m_currentAnimation].x,
			//m_AnimationSizes[(int)m_currentAnimation].x,
			m_AnimationIdx * m_AnimationSizes[(int)m_currentAnimation].x, 0,
			m_AnimationSizes[(int)m_currentAnimation].x,
			m_AnimationSizes[(int)m_currentAnimation].y,
			RGB(255, 0, 255));
	}
}

void CPlayer::DrawPlayerInfo(HDC hdc)
{
	if (m_Position.x == -1000.0f) return;
	SetBkMode(hdc, TRANSPARENT);
	m_PortraitImage.TransparentBlt(hdc,
		860, 136 + (m_Idx * 56), 55, 38,
		0, 0, 
		m_PortraitImage.GetWidth(), m_PortraitImage.GetHeight(),
		RGB(255, 0, 255));
	string id = m_ID;
	wchar_t wchar_ID[50] = {};

	textConvert(m_ID.c_str(), wchar_ID);
	TextOut(hdc, 950, 136 + (m_Idx * 56), wchar_ID, lstrlen(wchar_ID));
	//TextOut(hdc, 950, 192, L"PlayerID", lstrlen(L"PlayerID"));
	//TextOut(hdc, 950, 248, L"PlayerID", lstrlen(L"PlayerID"));
	//TextOut(hdc, 950, 304, L"PlayerID", lstrlen(L"PlayerID"));
}

void CPlayer::Update(float timeElapsed)
{
	std::chrono::duration<double> TimeElapsed = std::chrono::system_clock::now() - m_BommCreatTime;
	if (TimeElapsed.count() > 3.0f) m_CurrentBombNum = 0;

	Animate(timeElapsed);
}

bool CPlayer::IsCollide(const CGameObject& other)
{
	return false;
}

bool CPlayer::IsCollide(CGameObject* other)
{
	RECT rtThis = this->GetCollisionRect();
	RECT rtOther = other->GetCollisionRect();

	bool leftCondition = rtThis.left < rtOther.right;
	bool rightCondition = rtThis.right > rtOther.left;
	bool topCondition = rtThis.top < rtOther.bottom;
	bool bottomCondition = rtThis.bottom > rtOther.top;

	return (leftCondition &&
		rightCondition &&
		topCondition &&
		bottomCondition);
}

void CPlayer::FixCollision()
{
	m_Position = m_PrevPosition;
}

void CPlayer::ChangeState(PlayerState nextState)
{
	m_State = nextState;
	m_currentAnimation = (PlayerImages)nextState;
	m_AnimationIdx = 0;
}

void CPlayer::Move(Direction dir)
{
	m_Dir = dir;
	bool cantMove =
		(m_State == PlayerState::die)
		| (m_State == PlayerState::live)
		| (m_State == PlayerState::trap);
	if (cantMove) return;

	//switch (dir)
	//{
	//case Direction::left:
	//	m_currentAnimation = PlayerImages::left;
	//	break;
	//case Direction::right:
	//	m_currentAnimation = PlayerImages::right;
	//	break;
	//case Direction::up:
	//	m_currentAnimation = PlayerImages::up;
	//	break;
	//case Direction::down:
	//	m_currentAnimation = PlayerImages::down;
	//	break;
	//}
	//m_Dir = dir;
	m_State = PlayerState::move;
}

void CPlayer::Stop()
{
	if (m_State == PlayerState::die) return;
	m_State = PlayerState::wait;
	m_AnimationIdx = 0;
}

void CPlayer::SpeedUp()
{
	m_Vel.x = min(m_Vel.x + PlAYER_SPEED, PlAYER_SPEED * PLAYER_SPEED_LIMIT);
	m_Vel.y = min(m_Vel.y + PlAYER_SPEED, PlAYER_SPEED * PLAYER_SPEED_LIMIT);
	m_Speed = min(m_Speed + 1, PLAYER_SPEED_LIMIT);
} 

void CPlayer::SetDirection(Direction dir)
{
	bool cantMove =
		(m_State == PlayerState::die)
		| (m_State == PlayerState::live)
		| (m_State == PlayerState::trap);
	if (cantMove) return;

	switch (dir)
	{
	case Direction::left:
		m_currentAnimation = PlayerImages::left;
		break;
	case Direction::right:
		m_currentAnimation = PlayerImages::right;
		break;
	case Direction::up:
		m_currentAnimation = PlayerImages::up;
		break;
	case Direction::down:
		m_currentAnimation = PlayerImages::down;
		break;
	}
	m_Dir = dir;
}

bool CPlayer::CanCreateBomb()
{
	return m_CurrentBombNum < m_MaxBomb;
	//return m_Bombs.size() < m_MaxBomb;
}

void CPlayer::SetCreateBombFlag(bool flag)
{
	m_BoomCreateFlag = flag; 
	if (flag)
	{
		m_BommCreatTime = std::chrono::system_clock::now();
	}
}

void CPlayer::LoadImages(CharacterName name)
{
	m_AnimationIdx = 0;
	m_currentAnimation = PlayerImages::down;
	switch (name)
	{
	case CharacterName::Bazzi:
		m_Images[0].Load(_T("assets/player/bazzi/down.png"));
		m_Images[1].Load(_T("assets/player/bazzi/up.png"));
		m_Images[2].Load(_T("assets/player/bazzi/left.png"));
		m_Images[3].Load(_T("assets/player/bazzi/right.png"));
		//m_Images[4].Load(_T("assets/player/bazzi/trap.png"));
		m_Images[4].Load(_T("assets/player/bazzi/trap.bmp"));
		m_Images[5].Load(_T("assets/player/bazzi/die.png"));
		m_Images[6].Load(_T("assets/player/bazzi/live.png"));
		
		m_PortraitImage.Load(_T("assets/player/bazzi/portrait.png"));
		
		m_AnimationSizes[0] = { 64, 76 };
		m_AnimationSizes[1] = { 64, 76 };
		m_AnimationSizes[2] = { 64, 76 };
		m_AnimationSizes[3] = { 64, 76 };
		//m_AnimationSizes[4] = { 88, 144 };
		m_AnimationSizes[4] = { 60, 65 };
		m_AnimationSizes[5] = { 88, 144 };
		m_AnimationSizes[6] = { 88, 144 };

		m_AnimationLen[0] = 8;
		m_AnimationLen[1] = 8;
		m_AnimationLen[2] = 6;
		m_AnimationLen[3] = 6;
		//m_AnimationLen[4] = 13;
		m_AnimationLen[4] = 16;
		m_AnimationLen[5] = 13;
		m_AnimationLen[6] = 5;
		break;

	case CharacterName::Dao:
		m_Images[0].Load(_T("assets/player/dao/down.bmp"));
		m_Images[1].Load(_T("assets/player/dao/up.bmp"));
		m_Images[2].Load(_T("assets/player/dao/left.bmp"));
		m_Images[3].Load(_T("assets/player/dao/right.bmp"));
		m_Images[4].Load(_T("assets/player/dao/trap.bmp"));
		m_Images[5].Load(_T("assets/player/dao/die.png")); 

		m_PortraitImage.Load(_T("assets/player/dao/portrait.png"));

		m_AnimationSizes[0] = { 44, 60 };
		m_AnimationSizes[1] = { 44, 60 };
		m_AnimationSizes[2] = { 44, 60 };
		m_AnimationSizes[3] = { 44, 60 };
		m_AnimationSizes[4] = { 60, 65 };
		m_AnimationSizes[5] = { 88, 110 };

		m_AnimationLen[0] = 7;
		m_AnimationLen[1] = 7;
		m_AnimationLen[2] = 7;
		m_AnimationLen[3] = 7;
		m_AnimationLen[4] = 16;
		m_AnimationLen[5] = 13;
		break;
	default:
		break;
	}
	
}

void CPlayer::LoadSounds()
{
	
}

void CPlayer::Animate(float timeElapsed)
{
	if (m_State == PlayerState::wait) return;

	m_TimeSum += timeElapsed;

	if (m_TimeSum < 0.1f) return;
	m_TimeSum = 0.0f;

	++m_AnimationIdx;
	if (m_State != PlayerState::die)
	{
		if (m_AnimationIdx >= m_AnimationLen[(int)m_currentAnimation])
		{
			m_AnimationIdx = 0;
		}
	}
}
