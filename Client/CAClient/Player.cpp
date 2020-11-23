#include "stdafx.h"
#include "Player.h"
#include "Bomb.h"
CPlayer::CPlayer(Vector2D<float> position)
{
	m_Position = position;

	m_Power = 3;
	m_MaxBomb = 2;
	m_Speed = 2;
	m_TimeSum = 0.0f;

	m_Size = { OBJECT_SIZE, OBJECT_SIZE };
	m_Vel = { PlAYER_SPEED * m_Speed, PlAYER_SPEED * m_Speed };

	m_Dir = Direction::down;

	LoadImages();
	LoadSounds();

	m_State = PlayerState::wait;
}

CPlayer::~CPlayer()
{
}

void CPlayer::Draw(HDC hdc)
{
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

void CPlayer::Update(float timeElapsed)
{
	m_PrevPosition = m_Position;
	//if (m_State == PlayerState::move)
	//{
	//	if (m_Dir == Direction::left)
	//		m_Position.x = m_Position.x - (m_Vel.x * timeElapsed);
	//	if (m_Dir == Direction::right)
	//		m_Position.x = m_Position.x + (m_Vel.x * timeElapsed);
	//	if (m_Dir == Direction::up)
	//		m_Position.y = m_Position.y - (m_Vel.y * timeElapsed);
	//	if (m_Dir == Direction::down)
	//		m_Position.y = m_Position.y + (m_Vel.y * timeElapsed);
	//}
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

bool CPlayer::CanCreateBomb()
{
	return m_Bombs.size() < m_MaxBomb;
}

void CPlayer::LoadImages()
{
	m_Images[0].Load(_T("assets/player/bazzi/down.png"));
	m_Images[1].Load(_T("assets/player/bazzi/up.png"));
	m_Images[2].Load(_T("assets/player/bazzi/left.png"));
	m_Images[3].Load(_T("assets/player/bazzi/right.png"));
	m_Images[4].Load(_T("assets/player/bazzi/trap.png"));
	m_Images[5].Load(_T("assets/player/bazzi/die.png"));
	m_Images[6].Load(_T("assets/player/bazzi/live.png"));

	m_AnimationIdx = 0;
	m_currentAnimation = PlayerImages::down;

	m_AnimationSizes[0] = { 64, 76 };
	m_AnimationSizes[1] = { 64, 76 };
	m_AnimationSizes[2] = { 64, 76 };
	m_AnimationSizes[3] = { 64, 76 };
	m_AnimationSizes[4] = { 88, 144 };
	m_AnimationSizes[5] = { 88, 144 };
	m_AnimationSizes[6] = { 88, 144 };

	m_AnimationLen[0] = 8;
	m_AnimationLen[1] = 8;
	m_AnimationLen[2] = 6;
	m_AnimationLen[3] = 6;

	m_AnimationLen[4] = 13;
	m_AnimationLen[5] = 13;
	m_AnimationLen[6] = 5;
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
