#pragma once

#include "Vector2D.h"

#define OBJECT_SIZE 52



class CGameObject
{
protected:
	Vector2D<float> m_Position;	// 위치
	Vector2D<int>	m_Size;		// 크기
	Vector2D<float> m_Vel;		// 속도

public:
	CGameObject() {}
	virtual ~CGameObject() {}

public:
	virtual void Draw(HDC hdc) = 0;
	virtual void Update(float timeElapsed) = 0;

	virtual bool IsCollide(const CGameObject& other) = 0;
	virtual bool IsCollide(CGameObject* other);
	virtual bool IsInMap();
	void DrawRect(HDC hdc);
public:
	Vector2D<int>	GetSize() const		{ return m_Size; }
	Vector2D<float>	GetPosition() const { return m_Position; }
	RECT			GetCollisionRect();
	
};

