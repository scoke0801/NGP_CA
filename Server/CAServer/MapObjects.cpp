#include "stdafx.h"
#include "MapObjects.h"
#include "GameSceneProcessor.h"

CBomb::CBomb(Vector2D<float> position, int power)
{
	m_CreatedTime = std::chrono::system_clock::now();
	
	m_Position = position;
	m_Power = power;

	GameSceneProcessor::GetCoordinates(position, { OBJECT_SIZE, OBJECT_SIZE });
}

void CBomb::TimeUpdate()
{
	m_timeElapsed = std::chrono::system_clock::now() - m_CreatedTime;//����ð��� �����ð��� ���ؼ�
}
