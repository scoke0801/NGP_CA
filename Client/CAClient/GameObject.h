#pragma once

#include "Vector2D.h"

class CGameObject
{
protected:
	Vector2D<float> m_Position;
	Vector2D<int>	m_Size;
	Vector2D<float> m_Vel;
	Vector2D<float>	m_Acc;
	
public:
	CGameObject() {}
	virtual ~CGameObject() {}

public:
	virtual void Draw(HDC hdc) = 0;
	virtual void Update(float timeElapsed) = 0;
};

