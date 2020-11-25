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
class CPlayer;

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
	
	vector<Vector2i> m_LastBranchCoords;	// 물풍선 폭발 범위 좌표

	// 물풍선 초기 생성 이후, 플레이어가 물풍선 위에 있는 문제 해결 위한 변수
	bool			m_PlayerIsOn;	
	CPlayer* m_Player;

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

	void SetPlayer(CPlayer* player); 
	CPlayer* GetPlayer() { return m_Player; }

	bool IsPlayerOn() const { return m_PlayerIsOn; }

	void SetLastBranchCoords(vector<Vector2i>& coords);
	
	vector<Vector2i>& GetLastBranchCoords() { return m_LastBranchCoords; }
private:
	void Animate(float timeElapsed);
	void CheckPlayerOut();
};

