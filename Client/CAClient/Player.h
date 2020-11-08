#pragma once
#include "GameObject.h"
enum class PlayerImages
{
	left = 0,
	right,
	up,
	down,
	trap,
	die,
	live,
	count,
};
class CPlayer : public CGameObject
{
private:
	CImage m_Images[(int)PlayerImages::count];
	UINT m_ImageIndex[(int)PlayerImages::count];

	int m_ImageIdx;
	int m_AnimationIdx;

public:
	CPlayer();
	~CPlayer();

public:
	virtual void Draw(HDC hdc);
	virtual void Update(float timeElapsed);
};

