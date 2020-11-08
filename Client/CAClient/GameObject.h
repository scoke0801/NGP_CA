#pragma once

#include "Vector2D.h"

#define OBJECT_SIZE 52



class CGameObject
{
protected:
	Vector2D<float> m_Position;	// ��ġ
	Vector2D<int>	m_Size;		// ũ��
	Vector2D<float> m_Vel;		// �ӵ�

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

