#pragma once

#include "GameObject.h"

enum class ItemName
{
	ballon = 0,
	potion,
	nuclear,
	skate,
	count
};
class CItem : public CGameObject
{
private:
	ItemName	m_Name;
	CImage		m_Image;

	Vector2D<int> m_AnimationSizes;
	int m_AnimationIdx;

	float m_TimeSum;
public:
	CItem(ItemName name, Vector2D<float> position);
	~CItem() {}

public:
	virtual void Draw(HDC hdc);
	virtual void Update(float timeElapsed);

	virtual bool IsCollide(const CGameObject& other) { return true; }
	virtual bool IsCollide(CGameObject* other) { return true; }

	ItemName GetItemName() { return m_Name; }
};

