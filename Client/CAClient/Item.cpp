#include "stdafx.h"
#include "Item.h"

CItem::CItem(ItemName name, Vector2D<float> position)
{
	m_Name = name;
	switch (name)
	{
	case ItemName::ballon:
		m_Image.Load(_T("assets/item/ballon.png"));
		break;
	case ItemName::potion:
		m_Image.Load(_T("assets/item/potion.png"));
		break;
	case ItemName::nuclear:
		m_Image.Load(_T("assets/item/nuclear.png"));
		break;
	case ItemName::skate:
		m_Image.Load(_T("assets/item/skate.png"));
		break;
	default:
		assert(!"잘못된 아이템");
		break;
	}
	m_TimeSum = 0.0f;
	m_Size = { OBJECT_SIZE, OBJECT_SIZE };
	m_Position = position;
	m_AnimationSizes = { 56,70 };
	m_AnimationIdx = 0;
}

void CItem::Draw(HDC hdc)
{
	m_Image.TransparentBlt(
		hdc, m_Position.x, m_Position.y,
		m_Size.x, m_Size.y,
		m_AnimationIdx * m_AnimationSizes.x, 0,
		m_AnimationSizes.x,
		m_AnimationSizes.y,
		RGB(255, 0, 255));
}

void CItem::Update(float timeElapsed)
{
	m_TimeSum += timeElapsed;

	if (m_TimeSum < 0.3f) return;
	m_TimeSum = 0.0f;

	const int m_AnimationLen = 2;
	++m_AnimationIdx;
	if (m_AnimationIdx >= m_AnimationLen)
	{
		m_AnimationIdx = 0;
	}
}
