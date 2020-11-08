#pragma once

#include "GameObject.h"

enum class BlockName
{
	EMPTY = 0,
	BLOCK_YELLOW = 1,
	BLOCK_RED,
	HOUSE_YELLOW,
	HOUSE_RED,
	HOUSE_BLUE,
	BOX,
	TREE,
};

enum class BlockState
{
	Wait,
	Destroyed,
	End
};

class CBlock : public CGameObject
{
private:
	BlockName	m_Name;
	CImage		m_Image;

	Vector2D<int> m_AnimationSizes;
	int m_AnimationIdx;
	int m_AnimationLen;

	float m_TimeSum;

	bool m_IsCanCollide;	// 충돌가능 블록인지 여부
	bool m_IsCanDestroy;	// 파괴가능 블록인지 여부

	BlockState	m_State;

public:
	CBlock(BlockName name, Vector2D<float> position, bool isCanDestroy = true);
	~CBlock();

public:
	virtual void Draw(HDC hdc);
	virtual void Update(float timeElapsed);
	
	virtual bool IsCollide(const CGameObject& other) { return true; }
	virtual bool IsCollide(CGameObject* other) { return true; }

	void ChangeState(BlockState state) { m_State = state; }
	bool IsCanDestroy();

	bool CheckDelete() { return m_State == BlockState::End; }
private:
	void Animate(float timeElapsed);
};

