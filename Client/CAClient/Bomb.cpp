#include "stdafx.h"
#include "Bomb.h"
#include "Block.h"
#include "Player.h"

#define EXPLOSION_TIME 1.0f 
CBomb::CBomb(Vector2D<float> position, int power)
{
	m_Position = position;
	m_Power = power;

	m_Size = { OBJECT_SIZE, OBJECT_SIZE };

	m_Images[(int)BombState::Wait].Load(_T("assets/bomb/bomb.png"));
	m_Images[(int)Explosion::inPlace].Load(_T("assets/bomb/inPlace.png"));
	m_Images[(int)Explosion::down].Load(_T("assets/bomb/down.png"));
	m_Images[(int)Explosion::up].Load(_T("assets/bomb/up.png"));
	m_Images[(int)Explosion::left].Load(_T("assets/bomb/left.png"));
	m_Images[(int)Explosion::right].Load(_T("assets/bomb/right.png"));

	m_AnimationSizes[0] = { 56,54 };
	m_AnimationLen[0] = 4;

	for (int i = 1; i < BombImageNum; ++i)
	{                   
		m_AnimationSizes[i] = { OBJECT_SIZE, OBJECT_SIZE };
		m_AnimationLen[i] = 11;
	}
	m_AnimationLen[(int)Explosion::inPlace] = 6;
	m_AnimateTimeSum = 0.0f;
	m_ExplosionTime = 0.0f;

	m_AnimationIdx = 0;

	m_State = BombState::Wait;

	m_LastBranchCoords.reserve(m_Power * 4);
	m_PlayerIsOn = false; 
}

void CBomb::Draw(HDC hdc)
{
	
}

void CBomb::Update(float timeElapsed)
{
	//CheckPlayerOut();
	Animate(timeElapsed);
	//m_ExplosionTime += timeElapsed;
}

void CBomb::Draw(HDC hdc, CBlock* blocks[][MAP_WIDTH])
{
	if (m_State == BombState::Wait)
	{ 
		m_Images[(int)m_State].TransparentBlt(
			hdc,
			m_Position.x, m_Position.y,
			m_Size.x, m_Size.y,
			m_AnimationIdx * m_AnimationSizes[(int)m_State].x,
			0,
			m_AnimationSizes[(int)m_State].x, m_AnimationSizes[(int)m_State].y,
			RGB(255, 0, 255));
	}
	else if (m_State == BombState::Explosion)
	{
		m_Images[(int)Explosion::inPlace].TransparentBlt(
			hdc,
			m_Position.x, m_Position.y,
			m_Size.x, m_Size.y,
			m_AnimationIdx * m_AnimationSizes[(int)m_State].x,
			0,
			m_AnimationSizes[(int)m_State].x, m_AnimationSizes[(int)m_State].y,
			RGB(255, 0, 255));

		Vector2D<float> pos[4];
		for (int i = 0; i < 4; ++i)	pos[i] = m_Position;

		for (int i = 0; i < m_Power; ++i)// down
		{
			pos[0].y += m_Size.y;

			Vector2i coord = GetCoordinates(pos[0], m_Size);
			if (!IsInMapCoord(coord)) break;
			bool result = true;
			if (blocks[coord.y][coord.x] != nullptr) result = blocks[coord.y][coord.x]->IsCanDestroy(); 
			if (!result) 
				break;

			m_Images[(int)Explosion::down].TransparentBlt(
				hdc,
				pos[0].x, pos[0].y,
				m_Size.x, m_Size.y,
				m_AnimationIdx * m_AnimationSizes[(int)Explosion::down].x,
				0,
				m_AnimationSizes[(int)Explosion::down].x, m_AnimationSizes[(int)Explosion::down].y,
				RGB(255, 0, 255));
		}
		for (int i = 0; i < m_Power; ++i)// up
		{
			pos[1].y -= m_Size.y;

			Vector2i coord = GetCoordinates(pos[1], m_Size);
			if (!IsInMapCoord(coord)) break;
			bool result = true;
			if (blocks[coord.y][coord.x] != nullptr) result = blocks[coord.y][coord.x]->IsCanDestroy();
			if (!result) 
				break;

			m_Images[(int)Explosion::up].TransparentBlt(
				hdc,
				pos[1].x, pos[1].y,
				m_Size.x, m_Size.y,
				m_AnimationIdx * m_AnimationSizes[(int)Explosion::up].x,
				0,
				m_AnimationSizes[(int)Explosion::up].x, m_AnimationSizes[(int)Explosion::up].y,
				RGB(255, 0, 255));
		}
		for (int i = 0; i < m_Power; ++i) // left
		{
			pos[2].x -= m_Size.x;

			Vector2i coord = GetCoordinates(pos[2] , m_Size);
			if (!IsInMapCoord(coord)) break;
			bool result = true;
			if (blocks[coord.y][coord.x] != nullptr) result = blocks[coord.y][coord.x]->IsCanDestroy();
			if (!result) 
				break;
			m_Images[(int)Explosion::left].TransparentBlt(
				hdc,
				pos[2].x, pos[2].y,
				m_Size.x, m_Size.y,
				m_AnimationIdx * m_AnimationSizes[(int)Explosion::left].x,
				0,
				m_AnimationSizes[(int)Explosion::left].x, m_AnimationSizes[(int)Explosion::left].y,
				RGB(255, 0, 255));
		}
		for (int i = 0; i < m_Power; ++i)// right
		{
			pos[3].x += m_Size.x;

			Vector2i coord = GetCoordinates(pos[3]  , m_Size);
			if (!IsInMapCoord(coord)) break;
			bool result = true;
			if (blocks[coord.y][coord.x] != nullptr) result = blocks[coord.y][coord.x]->IsCanDestroy();
			if (!result) 
				break;

			m_Images[(int)Explosion::right].TransparentBlt(
				hdc,
				pos[3].x, pos[3].y,
				m_Size.x, m_Size.y,
				m_AnimationIdx * m_AnimationSizes[(int)Explosion::right].x,
				0,
				m_AnimationSizes[(int)Explosion::right].x, m_AnimationSizes[(int)Explosion::right].y,
				RGB(255, 0, 255));
		}
	}
}

void CBomb::SetLastBranchCoords(vector<Vector2i>& coords)
{
	if (m_LastBranchCoords.size() > 0) m_LastBranchCoords.clear();

	m_LastBranchCoords = coords;
}

void CBomb::Animate(float timeElapsed)
{
	float timeLimit = (m_State == BombState::Wait) ? 0.3f : 0.1f;
	int animationLen = 4; 
	if (m_State == BombState::Explosion) animationLen = 4;

	m_AnimateTimeSum += timeElapsed;
	if (m_AnimateTimeSum < timeLimit) return;
	m_AnimateTimeSum = 0.0f;

	++m_AnimationIdx;

	if (m_AnimationIdx >= animationLen)
	{
		if (m_State == BombState::Explosion)
			ChangeState(BombState::End);
		else
			m_AnimationIdx = 0;
	}

}

void CBomb::CheckPlayerOut()
{
	if (!m_PlayerIsOn) return;
	RECT rtThis = this->GetCollisionRect();
	RECT rtOther = m_Player->GetCollisionRect();
	bool leftCondition = rtThis.left <= rtOther.right;
	bool rightCondition = rtThis.right >= rtOther.left;
	bool topCondition = rtThis.top <= rtOther.bottom;
	bool bottomCondition = rtThis.bottom >= rtOther.top;

	if (!(leftCondition &&
		rightCondition &&
		topCondition &&
		bottomCondition))
		m_PlayerIsOn = false;
}

bool CBomb::IsCollide(CGameObject* other)
{
	if (other == m_Player)
	{
		if (m_PlayerIsOn) return false;
	}

	return CGameObject::IsCollide(other);
	return false;
}

bool CBomb::IsCollide(const Vector2D<int> coord)
{
	return true;
}

bool CBomb::IsTimeToExplose()
{
	if (m_State != BombState::Wait) return false;
	return m_ExplosionTime > EXPLOSION_TIME;
}

void CBomb::ChangeState(BombState state)
{
	m_AnimateTimeSum = 0;
	m_AnimationIdx = 0;
	m_State = state;
}

void CBomb::SetPlayer(CPlayer* player)
{
	m_Player = player;
	m_PlayerIsOn = true;
}

