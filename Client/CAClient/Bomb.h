#pragma once
#include "GameObject.h"

enum class BombState
{
	Wait = 0,
	Explosion,
	End,
	count
};
enum class Explosion
{
	inPlace	= 1,	// 제자리에서
	down,
	up,
	left,
	right,
}; 

#define BombImageNum 6
#define	MAP_WIDTH 15

class CBlock;

class CBomb : public CGameObject
{
private:
	CImage			m_Images[BombImageNum];

	Vector2D<int>	m_AnimationSizes[BombImageNum];
	int				m_AnimationIdx;
	int				m_AnimationLen[BombImageNum];

	float			m_AnimateTimeSum;
	float			m_ExplosionTime;

	BombState		m_State;
	
	int				m_Power;
public:
	
public:
	CBomb(Vector2D<float> position, int power);
	~CBomb() {}

public:
	virtual void Draw(HDC hdc);
	virtual void Update(float timeElapsed);
	void Draw(HDC hdc, CBlock* blocks[][MAP_WIDTH]);

	virtual bool IsCollide(const CGameObject& other) { return true; }
	virtual bool IsCollide(CGameObject* other); 
	bool IsCollide(const Vector2D<int> coord);

	bool IsTimeToExplose();
	bool IsOnExplosion() const { return m_State == BombState::Explosion; }

	void ChangeState(BombState state);
	BombState GetState() const { return m_State; }

	bool CheckDelete() { return m_State == BombState::End; }

	int GetPower() const { return m_Power; }
private:
	void Animate(float timeElapsed);
};

