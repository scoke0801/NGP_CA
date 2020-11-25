#include "stdafx.h"
#include "MapObjects.h"
#include "GameSceneProcessor.h"

CBomb::CBomb(Vector2D<float> position, int power)
{
	m_CreatedTime = std::chrono::system_clock::now();
	
	m_Position = position;
	m_Power = power;
	m_State = BombState::Wait;

	m_Coordinates = GameSceneProcessor::GetCoordinates(position, { OBJECT_SIZE, OBJECT_SIZE });
}

void CBomb::TimeUpdate()
{
	m_timeElapsed = std::chrono::system_clock::now() - m_CreatedTime;//현재시간과 이전시간을 비교해서
}

bool CBomb::IsTimeToExplose()
{
	if (m_State != BombState::Wait) return false;
	return m_timeElapsed.count() > EXPLOSION_TIME;
}

CBlock::CBlock(Vector2i coord, bool isCanDestroy, bool isCanMove)
{ 
	m_Coordinates = coord;
	m_IsCanDestroy = isCanDestroy;
	m_IsCanMove = isCanMove;
}
