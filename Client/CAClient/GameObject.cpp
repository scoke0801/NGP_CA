#include "stdafx.h"
#include "GameObject.h"

bool CGameObject::IsCollide(CGameObject* other)
{
	RECT rtThis = this->GetCollisionRect();
	RECT rtOther = other->GetCollisionRect();

	bool leftCondition	 = rtThis.left	 <= rtOther.right;
	bool rightCondition  = rtThis.right  >= rtOther.left;
	bool topCondition	 = rtThis.top	 <= rtOther.bottom;
	bool bottomCondition = rtThis.bottom >= rtOther.top;

	return (leftCondition &&
		rightCondition &&
		topCondition &&
		bottomCondition);
}

bool CGameObject::IsInMap()
{
	RECT rtMap;
	rtMap.left = 24;
	rtMap.right = 807;
	rtMap.top = 52;
	rtMap.bottom = 732;

	if (m_Position.x < rtMap.left) return false;
	if (m_Position.x + m_Size.x > rtMap.right) return false;
	if (m_Position.y < rtMap.top) return false;
	if (m_Position.y + m_Size.y > rtMap.bottom) return false;
	return true;
}

void CGameObject::DrawRect(HDC hdc)
{
	RECT rt = GetCollisionRect();
	Rectangle(hdc, rt.left, rt.top, rt.right, rt.bottom);
}

RECT CGameObject::GetCollisionRect()
{
	RECT rt;
	rt.top = m_Position.y + 4;
	rt.left = m_Position.x + 4;
	rt.bottom = m_Position.y + m_Size.y - 4;
	rt.right = m_Position.x + m_Size.x - 4;
	return rt;
}

Vector2D<float> CGameObject::GetCenter() const
{
	Vector2D<float> center;
	center.x = m_Position.x + m_Size.x * 0.5f;
	center.y = m_Position.y + m_Size.y * 0.5f;
	return center;
}
