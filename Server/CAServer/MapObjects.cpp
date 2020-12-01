#include "stdafx.h"
#include "MapObjects.h"
#include "GameSceneProcessor.h"

CBomb::CBomb(Vector2D<float> position, int power, int index)
{
	m_CreatedTime = std::chrono::system_clock::now();	
	
	m_PlayerIsOn = true;
	m_PlayerPosition = m_Position = position;
	m_Power = power;
	m_State = BombState::Wait;
	m_PlayerIndex = index;

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

void CBomb::CheckPlayerOut(Vector2f playerPosition, int index)
{
	if (!m_PlayerIsOn) return;
	if (index != m_PlayerIndex) return;
	Vector2f pos = GameSceneProcessor::GetPositionCoord(m_Coordinates);

	RECT rtThis = GameSceneProcessor::GetCollisionRect(pos);
	RECT rtOther = GameSceneProcessor::GetCollisionRect(playerPosition);
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

void CBomb::SetLastBranchCoords(vector<Vector2i>& coords)
{
	if (m_LastBranchCoords.size() > 0) m_LastBranchCoords.clear();

	m_LastBranchCoords = coords;
}

CBlock::CBlock(Vector2i coord, bool isCanDestroy, bool isCanMove)
{ 
	m_Coordinates = coord;
	m_IsCanDestroy = isCanDestroy;
	m_IsCanMove = isCanMove;
}

CItem::CItem(ItemName name, Vector2i coord)
{
	m_Name = name;
	m_Coordinates = coord;
	m_Position = GameSceneProcessor::GetInstance()->GetPositionCoord(coord);
}

CPlayer::CPlayer(Vector2f position, int index, string ID, PlayerState state)
{
	m_Position = position;
	m_Index = index;
	m_State = state;
	m_ID = ID;

	m_Speed = 4;
	m_Direction = Direction::down;
	m_Power = 2;
	m_CharacterName = CharacterName::Bazzi;
	m_BombNum = 2;

	m_CreatedTime = std::chrono::system_clock::now();
	m_DeadTime = std::chrono::system_clock::now();
	m_PrevUpdateTime = std::chrono::system_clock::now();
}

void CPlayer::UpdateElapsedTime()
{
	m_TimeElapsed = std::chrono::system_clock::now() - m_PrevUpdateTime;//현재시간과 이전시간을 비교해서
	m_PrevUpdateTime = std::chrono::system_clock::now();
}

double CPlayer::GetDeadTime()
{
	std::chrono::duration<double> TimeElapsed = std::chrono::system_clock::now() - m_DeadTime;
	return TimeElapsed.count();
}

double CPlayer::GetAlivedTime()
{
	std::chrono::duration<double> TimeElapsed = std::chrono::system_clock::now() - m_CreatedTime;
	return TimeElapsed.count();
}
